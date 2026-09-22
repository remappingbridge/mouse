# UI ↔ Core Contract v1 Candidate

Status: **UIC-05 RELEASE CANDIDATE — NOT RELEASED**.

Candidate semantic version: **1.0**.

Accepted source baselines:

~~~text
mouse-ui UI Layout 1.0:
e8adad7919e931c92515bf655ef4050876a8e7a9

remappingbridge/mouse accepted through UIC-04:
dff0b9c0fadb856b4fe0f2c7f9f4b76f84de984a

remappingbridge/repo-planner accepted through UIC-04:
3fd2c1e0e552833edb22eb443d43dc11338b326f
~~~

This document is the consolidated language-neutral normative candidate assembled from
UIC-00 through UIC-04. It is the semantic source of truth for the proposed C binding in
`uic-05-c-binding.h`.

It is **not** yet an immutable release. UIC-06 and UIC-07 must prove both implementation
sides before UIC-08 may publish a release directory.

Normative keywords **MUST**, **MUST NOT**, **SHOULD**, **MAY** are used in their ordinary
requirements sense.

## 1. Contract identity and compatibility

The candidate contract descriptor contains:

~~~text
major = 1
minor = 0
max_saved_mice = 16
max_mouse_name_utf8_bytes = 63
capabilities = required semantic capability set
~~~

Required capabilities:

- FIRST_DISCOVERY
- SAVED_RECONNECT
- PAIR_NEW
- PROFILE_PASSTHROUGH
- PROFILE_STANDARD
- PROFILE_ESCAPE
- PROFILE_CUSTOM
- REMOVE_MOUSE
- ESCAPE_OUTPUT

A provider is compatible when:

1. provider major equals consumer required major;
2. provider minor >= consumer minimum minor;
3. all required capabilities are present;
4. public major-1 limits are compatible.

Later compatible minors MAY add optional capabilities and trailing fields but MUST NOT
change existing semantics, increase the major-1 saved/name bounds, or require older
consumers to understand new enum values during baseline v1.0 behavior.

## 2. Scalar identities

The language-neutral model uses unsigned non-negative integer identities.

The candidate C binding represents these as unsigned 64-bit values:

- `mouse_id`
- `intent_id`
- `activity_id`
- `notification_seq`
- Snapshot `revision`

Value zero is reserved as **none/invalid** for identities. Valid allocated IDs and
sequences are non-zero.

Allocation ownership:

- UI/caller allocates `intent_id`;
- Core allocates `activity_id`, `notification_seq`, Snapshot revisions and stable
  `mouse_id` values;
- an ID allocated within one producer lifetime MUST NOT be reused in that lifetime.

## 3. Bounded text

### Mouse name

A semantic Mouse name:

- is valid UTF-8;
- contains 0..63 encoded bytes;
- contains no U+0000;
- is not semantically NUL-terminated.

Core sanitizes backend/device text before publication and truncates only on a Unicode
scalar boundary.

UI-specific title truncation, suffixing and fallback remain UI-local.

### Diagnostics

Provider diagnostic code/message are optional opaque diagnostics and are not stable
product semantics.

The C candidate bounds them for safe crossing:

- diagnostic code: max 31 UTF-8 bytes;
- diagnostic message: max 127 UTF-8 bytes.

Consumers MUST NOT branch on diagnostic contents.

## 4. Profiles and Custom mapping

Profiles:

- PASSTHROUGH
- STANDARD
- ESCAPE
- CUSTOM

Custom has exactly five sources:

- LEFT
- RIGHT
- MIDDLE
- FORWARD
- BACKWARD

Each source maps to exactly one target:

- LEFT
- RIGHT
- MIDDLE
- ESCAPE
- FORWARD
- BACKWARD

The C binding array order for the five sources is fixed as:
LEFT, RIGHT, MIDDLE, FORWARD, BACKWARD.

The confirmed Custom mapping is one **global semantic product value**.

The UI owns editable draft/dirty state. Only a complete five-source mapping crosses the
contract in APPLY_CUSTOM.

How Core physically persists or derives the global template is Core-local and MAY use
any internal per-device/global representation as long as the externally confirmed
semantics remain exactly one global mapping.

## 5. Stable error semantics

Public error object:

~~~text
category
retryability
visibility
optional diagnostic_code
optional diagnostic_message
~~~

Categories:

- INVALID_REQUEST
- CONFLICT
- STALE_STATE
- LIMIT_REACHED
- UNSUPPORTED
- INCOMPATIBLE_CONTRACT
- AUTHORITY_LOST
- TEMPORARY_UNAVAILABLE
- PERSISTENCE_FAILURE
- INTERNAL_FAILURE

Retryability:

- NOT_RETRYABLE
- AFTER_REFRESH
- AFTER_STATE_CHANGE
- LATER

Visibility:

- DIAGNOSTIC_ONLY
- GENERIC_USER_FAILURE
- USER_ACTIONABLE

Raw HCI/GATT/HID++/USB/vendor status values MUST NOT become public categories.

## 6. Snapshot read result

Reading product truth produces one `SnapshotReadResult`.

It is exactly one of:

- `SNAPSHOT` — carries one complete atomic Snapshot;
- `UNAVAILABLE` — carries one stable public error explaining why no safe product
  Snapshot can currently be published.

`UNAVAILABLE` exists for initialization/migration/provider failures such as an
unrecoverable PERSISTENCE_FAILURE before normal product truth is safe to expose.

A consumer MUST NOT fabricate or retain misleading confirmed state as current truth when
the provider explicitly returns UNAVAILABLE.

## 7. Snapshot

A Snapshot is an immutable atomic point-in-time product value containing:

~~~text
revision
saved_mice[0..16]
current_mouse_id?        # zero or one authoritative Mouse
custom_confirmed
search?
operation?
~~~

### Saved Mouse record

Each record contains:

- stable non-zero `mouse_id`;
- bounded semantic `name`;
- one confirmed profile.

Saved IDs are unique.

### Current authority

At most one current Mouse exists.

When present, `current_mouse_id` MUST identify a record in `saved_mice`.

No independent per-record connected flag exists.

### Confirmed versus requested state

Confirmed profile/Custom values are never overwritten merely because an operation is
pending.

Requested values appear only inside the pending/terminal operation describing that
request.

## 8. Search state

At most one observable search slot exists.

Fields:

~~~text
activity_id
origin_intent_id
purpose
status
candidate?
error?
~~~

Purposes:

- FIRST
- SAVED
- PAIR_NEW

Snapshot statuses:

- RUNNING
- FOUND
- TIMED_OUT
- FAILED
- CANCELLED

Rules:

- FOUND MUST carry a candidate;
- FAILED MUST carry exactly one public error;
- other statuses MUST NOT require/synthesize an error;
- Core owns semantic deadlines from accepted activity start:
  - FIRST 8 seconds;
  - SAVED 8 seconds;
  - PAIR_NEW 15 seconds.

Candidate carries stable candidate `mouse_id`, semantic `name`, and
`already_saved`.

## 9. Operation state

At most one observable product-mutation operation slot exists.

Fields:

~~~text
activity_id
origin_intent_id
kind
status
target_mouse_id
requested_profile?
requested_custom?
error?
~~~

Kinds:

- PROFILE_APPLY
- CUSTOM_APPLY
- REMOVE
- HANDOFF

Statuses:

- PENDING
- SUCCEEDED
- FAILED
- CANCELLED

Rules:

- PROFILE_APPLY requires a non-CUSTOM requested profile;
- CUSTOM_APPLY requires requested profile CUSTOM and a complete requested mapping;
- FAILED carries exactly one public error;
- other statuses do not synthesize an error.

## 10. Intent envelope and submission

Every intent contains:

~~~text
intent_id
kind
kind-specific payload
~~~

Kinds:

- START_FIRST_SEARCH
- START_SAVED_SEARCH
- START_PAIR_NEW
- CANCEL_ACTIVITY
- APPLY_PROFILE
- APPLY_CUSTOM
- REMOVE_MOUSE

Submission disposition is exactly one of:

- ACCEPTED
- REPLAY
- REJECTED

REJECTED carries exactly one public error.

For START_FIRST_SEARCH, START_SAVED_SEARCH, START_PAIR_NEW, APPLY_PROFILE, APPLY_CUSTOM
and REMOVE_MOUSE, ACCEPTED/REPLAY MUST carry the Core `activity_id` for that logical
request. REPLAY returns the original activity ID.

CANCEL_ACTIVITY does not create a second activity; it targets an existing activity and
therefore ACCEPTED/REPLAY carries no new activity ID.

Replay rules:

- same intent_id + same kind/payload => REPLAY and no duplicate side effect;
- same intent_id + different kind/payload => REJECTED / INVALID_REQUEST.

### Search intents

START_FIRST_SEARCH preconditions:

- no saved Mouse;
- no current Mouse;
- no active search/conflicting operation.

START_SAVED_SEARCH preconditions:

- at least one saved Mouse;
- no current Mouse;
- no active search/conflicting operation.

START_PAIR_NEW preconditions:

- one saved/current authoritative Mouse;
- saved count < 16;
- no active search/conflicting mutation.

Pair New is unsaved-only.

### CANCEL_ACTIVITY

Payload: `target_activity_id`.

Target MUST be the currently non-terminal activity being abandoned.

Accepted cancellation logically makes the target terminal CANCELLED and physical/backend
cancellation is best effort.

### APPLY_PROFILE

Payload:

~~~text
target_mouse_id
profile = PASSTHROUGH | STANDARD | ESCAPE
~~~

Target MUST be saved and current.

### APPLY_CUSTOM

Payload:

~~~text
target_mouse_id
complete five-source mapping
~~~

Target MUST be saved and current.

### REMOVE_MOUSE

Payload: stable `target_mouse_id`.

Target MUST exist in saved records. It may be current or offline.

## 11. Activity lifecycle and results

Core allocates one non-zero `activity_id` for every asynchronous activity.

Every activity has exactly one `origin_intent_id` and one terminal transition.

Search terminal states:

- SUCCEEDED
- TIMED_OUT
- FAILED
- CANCELLED

Operation terminal states:

- SUCCEEDED
- FAILED
- CANCELLED

Classification:

- SUCCEEDED means the normative commit point was reached;
- TIMED_OUT is search-only and means the Core deadline committed first;
- CANCELLED means accepted logical cancellation committed first;
- FAILED means environment/backend/product failure prevented success.

A late physical/backend completion after terminal cancellation/failure MUST NOT mutate
confirmed state.

## 12. Logical notification stream

Core exposes one ordered logical stream.

Notifications have strictly increasing non-zero `notification_seq`.

Kinds:

- ACTIVITY_RESULT
- CONNECTION_CHANGED

### ACTIVITY_RESULT

Contains:

- activity_id;
- origin_intent_id;
- activity kind;
- terminal status;
- commit_revision;
- optional candidate;
- optional cancel_intent_id;
- public error exactly when terminal status FAILED.

### CONNECTION_CHANGED

Contains:

- previous current Mouse ID or none;
- new current Mouse ID or none;
- commit_revision;
- reason.

Reasons:

- PHYSICAL_DISCONNECT
- PHYSICAL_CONNECT
- SEARCH_RECONNECT
- FIRST_ACCEPT
- HANDOFF
- REMOVE_RELEASE

Connection truth is independent of UI screen state.

For one success commit that changes authority, logical ordering is:

1. CONNECTION_CHANGED
2. ACTIVITY_RESULT(SUCCEEDED)

## 13. Commit points

### FIRST

Success atomically creates the saved record and makes it the single current authority.

### SAVED reconnect

Success makes an existing saved identity current.

### Pair New / HANDOFF

Candidate qualification does not change authority.

HANDOFF success atomically:

- makes candidate a valid saved record;
- switches authority from old current to candidate;
- never exposes dual authority.

### Profile

PROFILE_APPLY success atomically changes confirmed profile.

### Custom

CUSTOM_APPLY success atomically changes both:

- global `custom_confirmed`;
- target Mouse confirmed profile to CUSTOM.

### Remove offline

Success removes saved record.

### Remove live

Two commits are required:

1. release current authority while target remains saved and REMOVE remains pending;
2. remove saved record and mark REMOVE SUCCEEDED.

## 14. Cancellation, stale and late work

Cancellation is logical invalidation plus best-effort physical cancellation.

Success-vs-cancel races are decided by semantic commit order.

Matching by screen, operation kind or Mouse alone is insufficient. Consumers correlate
work by `activity_id`.

Late/stale work MUST NOT:

- change confirmed profile/Custom;
- remove a Mouse;
- promote Pair candidate;
- replace current authority;
- complete a newer activity.

## 15. Persistence boundary

The contract exposes confirmed semantic state only.

Private Core persistence may use any schema/layout and may change independently.

The contract does not expose flash addresses, bonding records, migration journals or
private schema versions.

The one global confirmed Custom mapping is public semantics; its physical storage
topology is not.

## 16. C binding execution model

The C binding is an in-process **pull/submission API**:

- descriptor is read by value;
- SnapshotReadResult is read by value;
- intent is submitted by value;
- submission result is returned by value;
- notifications are polled and copied by value.

No public call returns provider-owned data pointers.

### Ownership/lifetime

- Core/integration owns the provider object;
- caller owns every input/output struct buffer;
- provider MUST copy any input data it needs before the call returns;
- provider MUST NOT retain pointers into caller structs;
- output contents remain valid because they are caller-owned copies;
- no allocator/free function crosses this contract.

### Threading/reentrancy

Calls against the **same provider instance MUST be externally serialized by the caller**.

The candidate API is not reentrant for one provider instance.

Different provider instances MAY be called independently.

Core MAY internally use threads/interrupts/event loops, but it MUST serialize publication
into the contract's Snapshot revision and notification ordering.

The binding does not expose mutexes, tasks or callbacks.

## 17. Snapshot availability and initialization

`get_descriptor` MUST be available before normal product state.

If Core cannot safely load/migrate confirmed persistent state:

- descriptor remains readable when possible;
- snapshot read returns UNAVAILABLE + PERSISTENCE_FAILURE (or INTERNAL_FAILURE when more
  appropriate);
- no fabricated empty saved registry may be presented as confirmed truth.

Once the provider becomes able to publish coherent truth, subsequent reads MAY return
SNAPSHOT.

## 18. C extension policy

The C candidate uses:

- explicit fixed-width integer types;
- bounded inline arrays;
- `struct_size` at public top-level call objects;
- explicit `reserved[]` fields initialized to zero.

Input rules:

- caller MUST zero-initialize top-level call structs;
- caller MUST set each top-level `struct_size` to the allocated buffer size;
- reserved fields MUST be zero;
- provider accepts a top-level buffer when `struct_size` is at least the v1.0 minimum
  for the same major and ignores unknown caller trailing bytes.

Output rules:

- provider writes no more than the caller-declared top-level `struct_size`;
- provider sets all known reserved output fields to zero;
- when returning SNAPSHOT, provider sets the nested `snapshot.struct_size` to the
  Snapshot shape it produced;
- consumer uses the produced/known size and ignores later-minor trailing bytes;
- consumer ignores unknown optional capability bits from a later compatible minor.

Only top-level size-tagged structs may grow by trailing append in a compatible minor.
Embedded structs keep their v1.0 size; later minors may consume explicitly reserved
embedded storage only when capability-gated and without changing existing offsets.

A provider serving a v1.0 consumer MUST NOT emit unknown enum/kind values in baseline
v1.0 flows.

Repurposing fields, enum values, required capability bits or public major-1 limits
requires a new contract major.

## 19. Candidate C call surface

The candidate binding exposes the semantic operations:

~~~text
get_descriptor(provider, out_descriptor)
read_snapshot(provider, out_snapshot_read_result)
submit_intent(provider, intent, out_submission_result)
poll_notification(provider, out_notification)
~~~

Call-level status is intentionally narrow:

- OK
- EMPTY (poll only)
- INVALID_ARGUMENT
- STRUCT_SIZE_MISMATCH

`EMPTY` is valid only for poll_notification when no logical notification is currently
available. Product failures are never encoded as call-level vendor/backend statuses.

Product failures belong in public semantic Error objects, not call-level backend codes.

## 20. Machine-readable schema and conformance vectors

Normative machine-readable candidate schema:

`uic-05-v1.schema.json`

Proposed C binding:

`uic-05-c-binding.h`

Compile-only binding smoke:

`uic-05-c-binding-smoke.c`

Independent conformance vectors:

`../fixtures/uic-05/`

UIC-00 coverage audit:

`../traceability/uic-05-v1-candidate-coverage.json`

## 21. No release yet

UIC-05 creates a reviewable implementable candidate only.

It MUST NOT create an immutable `releases/v1...` package.

UIC-06 must prove the UI adapter. UIC-07 must prove Core conformance. UIC-08 alone may
publish the immutable release package after both are accepted.
