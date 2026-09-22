# UIC-04 — Capabilities, Limits & Errors

Status: **CANDIDATE — AWAITING HUMAN REVIEW**.

Source baseline:

~~~text
UIC-03 accepted
mouse main: 9283913112b46676ba466a1fd862a6fd03cfcadb
repo-planner main: 404b48cfcf147aca299b6e8f8554f7a0b9626380
mouse-ui UI Layout 1.0: e8adad7919e931c92515bf655ef4050876a8e7a9
~~~

This document defines the public semantic limits, capability discovery, name encoding,
error taxonomy, retryability and cross-component compatibility rules required before a
concrete v1 binding can be assembled.

It does **not** expose Bluetooth, HID++, USB, flash-schema or vendor packet structures.

## 1. Contract descriptor

Before consuming normal Snapshot/Intent semantics, a UI adapter must be able to obtain one
read-only `ContractDescriptor`.

Language-neutral shape:

~~~text
contract:
  major
  minor

limits:
  max_saved_mice
  max_mouse_name_utf8_bytes

capabilities:
  set of semantic capability tokens
~~~

For the UIC v1 candidate line:

~~~text
contract.major = 1
contract.minor = 0
limits.max_saved_mice = 16
limits.max_mouse_name_utf8_bytes = 63
~~~

The descriptor is semantic metadata. Its C representation is deferred to UIC-05.

## 2. Public saved-Mouse limit

UIC-04 promotes the frozen UI capacity of 16 from a private implementation constant to
the **public v1 product maximum**:

~~~text
max_saved_mice = 16
~~~

Normative consequences:

- a valid v1 Snapshot contains 0..16 saved Mouse records;
- Core must not publish a seventeenth saved record under contract major 1;
- `START_PAIR_NEW` is rejected with `LIMIT_REACHED` when 16 records are already saved;
- an already-running Pair flow must not commit a seventeenth record;
- Remove can reduce the count and make Pair New legal again;
- a Core implementation may internally support more identities, but it cannot expose more
  than 16 through contract major 1.

Changing this public maximum requires a new contract major because the accepted UI 1.0
has fixed capacity for 16 records.

## 3. Mouse display-name semantic

### 3.1 Encoding and maximum

The contract Mouse `name` is:

- valid UTF-8;
- 0..63 encoded bytes;
- not NUL-terminated at the semantic level;
- forbidden from containing U+0000.

The 63-byte maximum is intentionally promoted from the accepted UI's 64-byte bounded
storage (one byte reserved by its C implementation for termination) into an explicit v1
cross-component limit rather than remaining a silent private assumption.

### 3.2 Core sanitation responsibility

When backend/device text does not already satisfy the contract, Core must produce a valid
contract name before publication:

- malformed input sequences are replaced with U+FFFD or otherwise converted to valid UTF-8;
- U+0000 is removed/replaced;
- text longer than 63 UTF-8 bytes is truncated at a Unicode scalar boundary;
- Core does not append the word `MOUSE`;
- Core does not apply the UI's 15-character display truncation.

Unicode NFC/NFKC canonical normalization is **not required** by contract v1. Requiring a
full Unicode normalization implementation would add backend coupling without changing
the frozen UI behavior.

### 3.3 UI display responsibility

The UI remains responsible for frozen presentation rules:

- projection into the supported glyph set;
- removing display-trailing spaces as required by UI 1.0;
- display truncation to fit the frozen layout;
- the isolated-word `MOUSE` suffix rule;
- `UNKNOWN MOUSE` fallback for empty/illegible display output.

Therefore a semantic name and its rendered title are intentionally not identical types.

## 4. Required semantic capabilities

Contract major 1, minor 0 defines these capability tokens:

- `FIRST_DISCOVERY`
- `SAVED_RECONNECT`
- `PAIR_NEW`
- `PROFILE_PASSTHROUGH`
- `PROFILE_STANDARD`
- `PROFILE_ESCAPE`
- `PROFILE_CUSTOM`
- `REMOVE_MOUSE`
- `ESCAPE_OUTPUT`

A provider is compatible with the frozen UI 1.0 only when all tokens above are present.

### 4.1 Escape output

`ESCAPE_OUTPUT` means the semantic target `ESCAPE` can produce the required synthetic
keyboard Escape output.

It does **not** expose:

- TinyUSB descriptors;
- USB endpoint numbers;
- keyboard report structs;
- HID report IDs.

Those remain Core-local.

Because the frozen UI always exposes the Escape profile/target and has no
"feature unavailable" UX, `ESCAPE_OUTPUT` is required rather than an optional UI branch.

### 4.2 HID++ and vendor behavior

HID++ is **not** a public capability token in v1.

The UI never branches on HID++ availability. Logitech/vendor support is an implementation
choice used to realize the same Mouse semantics. Vendor packet IDs, feature pages and
errors must remain diagnostic/private.

If a future user-visible semantic feature genuinely depends on vendor functionality, that
feature may receive a new semantic capability token in a future contract revision; the
vendor protocol itself still remains private.

## 5. Capability discovery and compatibility rules

A consumer declares:

- required contract major;
- minimum contract minor;
- required semantic capability tokens.

Compatibility requires all of:

1. provider major equals consumer required major;
2. provider minor is greater than or equal to consumer minimum minor;
3. every consumer-required capability is present;
4. major-1 fixed limits are exactly compatible with this contract line.

For UI Layout 1.0 the required descriptor is effectively:

~~~text
major = 1
minor >= 0
max_saved_mice = 16
max_mouse_name_utf8_bytes = 63
all v1.0 required capability tokens present
~~~

Unknown **optional** capabilities introduced by a later compatible minor must be ignored
by an older consumer.

A later minor must not:

- increase the major-1 saved-Mouse public maximum;
- increase the major-1 name bound;
- remove/repurpose a required capability;
- change existing enum/value semantics.

Those changes require a new major.

## 6. Persistence and schema compatibility

Physical persistence format is Core-local.

The shared UI↔Core contract does **not** expose:

- flash addresses;
- sectors/pages;
- storage record structs;
- on-flash schema version;
- migration journal format;
- bonding database format.

Before Core publishes a compatible ContractDescriptor and first normal Snapshot, it is
responsible for loading/migrating its persisted state into valid contract semantics.

Cross-component compatibility therefore depends on **contract version + capabilities +
public limits**, not on the private persistence schema version.

If Core cannot migrate/read its persisted state safely:

- it must not expose a misleading partially valid confirmed state;
- initialization/discovery reports `PERSISTENCE_FAILURE`;
- diagnostics may include a private provider code/message;
- UI must not branch on the private diagnostic code.

This keeps storage migrations independent from frontend releases.

## 7. Stable error object

Where an error is semantically required, the public object is:

~~~text
category
retryability
visibility
optional diagnostic_code
optional diagnostic_message
~~~

`diagnostic_code` and `diagnostic_message` are opaque provider diagnostics:

- they are not stable contract identifiers;
- they may contain backend/vendor-specific information;
- UI logic must never branch on them;
- they are not frozen user-facing copy.

Only `category`, `retryability` and `visibility` are public semantics.

## 8. Stable error categories

The v1 categories are:

### INVALID_REQUEST

Meaning: malformed command/value or a request that can never be valid as submitted.

Examples:

- APPLY_PROFILE with CUSTOM;
- incomplete Custom mapping;
- intent-ID collision.

Default:

~~~text
retryability = NOT_RETRYABLE
visibility = DIAGNOSTIC_ONLY
~~~

### CONFLICT

Meaning: otherwise-valid request cannot start because another activity currently owns a
mutually exclusive product resource.

Examples:

- new mutation while another mutation is pending;
- search start while another search is active.

Default:

~~~text
retryability = AFTER_STATE_CHANGE
visibility = GENERIC_USER_FAILURE
~~~

### STALE_STATE

Meaning: request targeted semantic state that is no longer current.

Examples:

- profile apply to a Mouse that is saved but no longer authoritative;
- cancel of an activity that has already become terminal.

Default:

~~~text
retryability = AFTER_REFRESH
visibility = GENERIC_USER_FAILURE
~~~

### LIMIT_REACHED

Meaning: a public product limit prevents the request.

Example:

- Pair New requested while 16 Mouse records are saved.

Default:

~~~text
retryability = AFTER_STATE_CHANGE
visibility = USER_ACTIONABLE
~~~

### UNSUPPORTED

Meaning: a requested semantic feature/capability is absent.

This is primarily a compatibility/integration error under frozen UI 1.0 because all v1.0
capabilities are required.

Default:

~~~text
retryability = NOT_RETRYABLE
visibility = USER_ACTIONABLE
~~~

### INCOMPATIBLE_CONTRACT

Meaning: major/minor/required-capability/public-limit compatibility failed before normal
operation.

Default:

~~~text
retryability = NOT_RETRYABLE
visibility = USER_ACTIONABLE
~~~

### AUTHORITY_LOST

Meaning: an operation that required the authoritative current Mouse lost that authority
before its commit point.

Examples:

- current Mouse physically disconnects during PROFILE_APPLY;
- old current authority disappears during HANDOFF.

Default:

~~~text
retryability = AFTER_STATE_CHANGE
visibility = GENERIC_USER_FAILURE
~~~

### TEMPORARY_UNAVAILABLE

Meaning: Core cannot currently complete the semantic operation for a transient
environmental/backend reason that is not more specifically classified.

Default:

~~~text
retryability = LATER
visibility = GENERIC_USER_FAILURE
~~~

### PERSISTENCE_FAILURE

Meaning: confirmed/persistent state could not be safely loaded, migrated or committed.

Default:

~~~text
retryability = LATER
visibility = GENERIC_USER_FAILURE
~~~

### INTERNAL_FAILURE

Meaning: unexpected provider failure with no safer public category.

Default:

~~~text
retryability = LATER
visibility = GENERIC_USER_FAILURE
~~~

Raw Bluetooth/HCI/GATT/HID++/USB/vendor error codes never become public categories.

## 9. Retryability values

Public retryability is one of:

- `NOT_RETRYABLE` — repeating under the same conditions is not a supported recovery;
- `AFTER_REFRESH` — obtain current semantic state before deciding whether to issue a new intent;
- `AFTER_STATE_CHANGE` — wait for/remove the blocking semantic condition, then issue a new intent;
- `LATER` — a new intentional attempt may be made later.

UIC-02 retry identity rules still apply:

- replaying the same `intent_id` retrieves/replays the same logical request;
- a deliberate retry after terminal failure uses a **new** `intent_id`.

Retryability never instructs the UI to reuse a failed activity ID.

## 10. Visibility values

Public visibility is one of:

- `DIAGNOSTIC_ONLY` — frontend should not invent user blame/action copy from this category;
- `GENERIC_USER_FAILURE` — UI may present its frozen generic failure/retry behavior;
- `USER_ACTIONABLE` — semantic condition may legitimately affect a user decision/action.

Exact screen copy, colors and error screen selection remain UI-local.

UIC-04 intentionally does not freeze text such as "Bluetooth error", "USB error" or
vendor-specific wording.

## 11. Error attachment rules

### 11.1 Submission rejection

A UIC-02 `REJECTED` submission carries exactly one public error object.

Typical mapping:

| Invalid condition | Category |
|---|---|
| malformed payload / invalid enum / ID collision | INVALID_REQUEST |
| mutually exclusive activity already active | CONFLICT |
| stale current Mouse or stale activity target | STALE_STATE |
| saved count already 16 for Pair New | LIMIT_REACHED |
| semantic capability unavailable | UNSUPPORTED |

### 11.2 Activity result

`ACTIVITY_RESULT`:

- SUCCEEDED carries no error;
- CANCELLED carries no error;
- TIMED_OUT carries no error because timeout is already an explicit terminal status;
- FAILED carries exactly one public error object.

Typical FAILED mappings:

| Failure | Category |
|---|---|
| required current authority physically lost | AUTHORITY_LOST |
| confirmed state cannot be persisted | PERSISTENCE_FAILURE |
| transient backend inability | TEMPORARY_UNAVAILABLE |
| unexpected provider fault | INTERNAL_FAILURE |

### 11.3 Snapshot projection

When `search.status = FAILED` or `operation.status = FAILED`, the same stable public
error category/retryability/visibility associated with the terminal activity is available
in that Snapshot terminal state.

Other terminal states do not synthesize an error merely to carry diagnostics.

## 12. Search deadline ownership

UIC-04 resolves timeout ownership.

Core owns the semantic search deadline because Core owns search activities and the
TIMED_OUT terminal transition.

Product deadlines are:

- FIRST: 8 seconds from accepted search activity start;
- SAVED: 8 seconds from accepted search activity start;
- PAIR_NEW: 15 seconds from accepted search activity start.

Rules:

- time is measured by a Core monotonic time source;
- wall-clock/date/timezone are irrelevant;
- UI may show progress but does not decide semantic timeout;
- if success and timeout race, UIC-03 semantic commit order determines the single terminal outcome;
- if TIMED_OUT commits first, a later candidate/backend success is late and cannot change product truth.

Exact timer peripheral, tick frequency and scheduling implementation remain Core-local.

## 13. Unsupported/limit/error fixtures

Language-neutral fixtures live under:

`contracts/ui-core/fixtures/uic-04/`

They cover:

- compatible descriptor;
- missing required Escape capability;
- contract major mismatch;
- Pair New at the 16-record limit;
- 63-byte name boundary and overlong-name sanitation;
- stale target submission;
- authority-lost FAILED result;
- persistence FAILED result with private diagnostic;
- timeout as terminal status without an error object;
- unknown vendor diagnostic remaining non-normative.

## 14. Compatibility analysis

The following are public major-version commitments for contract major 1:

- maximum 16 saved Mouse records;
- maximum 63 UTF-8 bytes for semantic Mouse name;
- required semantic capability vocabulary/meanings;
- stable error categories/retryability/visibility meanings;
- Core-owned search timeout semantics.

The following remain replaceable without contract change:

- Bluetooth/USB/HID++ libraries;
- vendor packet/status numbers;
- flash schema/layout;
- persistence migration mechanism;
- timer peripheral/task/event-loop implementation;
- user-facing error copy.

## 15. Decisions resolved by UIC-04

UIC-04 resolves:

- **OD-009** — Core owns the semantic 8 s / 8 s / 15 s search deadlines;
- **OD-010** — stable public error taxonomy, retryability and visibility;
- **OD-011** — semantic capability/limit representation uses a ContractDescriptor;
- **OD-012** — 16 saved Mouse records is the public contract-major-1 maximum;
- **OD-013** — Mouse name is valid UTF-8, max 63 encoded bytes; Core sanitizes contract
  validity while UI owns display formatting;
- **OD-014** — Escape output is a required semantic capability; raw USB technology is not;
- the discovery part of **OD-020** — contract major/minor and capability compatibility
  rules are defined, while final released version constants/binding representation remain UIC-05/UIC-08.

Still deferred:

- concrete identity/version/descriptor C representations;
- final C ABI and enum integer values;
- memory ownership and thread-safety;
- internal Custom persistence layout;
- release packaging/version tag.
