# UI Layout 1.0 — UI ↔ Core semantic boundary draft

Status: **WORKING DRAFT — UIC-04 CAPABILITIES/LIMITS/ERRORS CANDIDATE — NOT RELEASED**.

Source: frozen `mouse-ui` UI Layout 1.0, `e8adad7919e931c92515bf655ef4050876a8e7a9`.

UIC-00 established semantic ownership, UIC-01 the Snapshot, UIC-02 intents and UIC-03 async ordering. UIC-04 now defines public limits, capability discovery, UTF-8 name semantics, stable errors and version compatibility while leaving the concrete C ABI and backend technology private.

## Goal

Define the smallest semantic boundary required to replace the frontend mock with a real Core without coupling either repository to the other's private state machine.

## 1. Ownership model

Every frozen business rule is classified exactly once as:

- **UI-local** — presentation, navigation, modal behavior, screen-local interaction or desktop/lab behavior that Core does not need to know;
- **Shared-contract semantic** — product state, action, result, ordering or lifetime information that must cross the UI↔Core boundary;
- **Core-local** — transport, hardware, persistence mechanism or backend implementation detail that must remain hidden behind the semantic contract.

The exhaustive rule-level classification is recorded in
`contracts/ui-core/traceability/uic-00-semantic-inventory.md`.

### UIC-00 traceability summary

| Frozen behavior family | Ownership | Boundary consequence |
|---|---|---|
| saved Mouse records, stable identity, confirmed profile | Shared-contract semantic | Snapshot exposes semantic records and confirmed profile state |
| zero/one authoritative connected Mouse | Shared-contract semantic | Snapshot identifies at most one current Mouse |
| Saved Devices ordering/pagination/rendering | UI-local | UI derives presentation order from semantic state |
| first/saved/Pair searches and outcomes | Shared-contract semantic | UI submits semantic search intent and observes correlated status/result |
| 8 s / 15 s user-visible search windows | Shared-contract semantic | Contract must preserve timeout semantics without prescribing timer implementation |
| Pair New unsaved-only qualification and handoff | Shared-contract semantic | Candidate qualification and authoritative handoff are semantic outcomes |
| Back/Help/Lock/screen resolver behavior | UI-local | No screen IDs or navigation commands enter the Core contract |
| profile and Custom apply confirmation | Shared-contract semantic | ACTIVE requires correlated successful semantic completion |
| Custom editor draft/dirty state | UI-local | UI owns uncommitted editing state |
| persistent global Custom template and applied mapping | Shared-contract semantic | Core-visible confirmed/persistent Custom semantic state is required |
| disconnect truth and active→not-active projection | Shared-contract semantic | Core reports connection truth; UI chooses the screen projection |
| remove target identity and remove completion | Shared-contract semantic | Removal is identity-based and asynchronous/correlated |
| late/stale result rejection | Shared-contract semantic | Requests/results need correlation/lifetime semantics |
| screen IDs, pixels, colors, hints, SDL, Inspector, lab controls | UI-local | Explicitly excluded from normative contract |
| BTstack/HCI/GATT/HOGP/HID++/TinyUSB/flash/GPIO/SPI | Core-local | Explicitly excluded from normative contract |
| desktop scale/backlight gain | UI-local | Development shell behavior only |

## 2. Normative Core → UI semantic Snapshot

UIC-01 defines the normative Snapshot semantics in:

`contracts/ui-core/drafts/uic-01-snapshot-model.md`

The Snapshot is one atomic revisioned value containing only:

- saved Mouse records with stable identity, semantic name and confirmed profile;
- zero or one authoritative current Mouse identity;
- confirmed global Custom mapping;
- at most one observable search activity;
- at most one observable operation;
- opaque activity correlation identity sufficient for later intent/result gates.

The authoritative invariants include:

- current identity is null or references exactly one saved record;
- saved identities are unique;
- confirmed profile is distinct from requested/pending profile;
- UI-local Custom draft/dirty state is absent;
- publication is atomic per revision;
- screen/navigation/pixel/SDL/Inspector state is forbidden.

Language-neutral fixtures live under `contracts/ui-core/fixtures/uic-01/`.

Exact C layout, activity-ID representation, async delivery and cancellation semantics remain deferred.

## 3. Normative UI → Core semantic intents

UIC-02 defines the normative command vocabulary in:

`contracts/ui-core/drafts/uic-02-intent-model.md`

The complete intent set is:

- `START_FIRST_SEARCH`
- `START_SAVED_SEARCH`
- `START_PAIR_NEW`
- `CANCEL_ACTIVITY`
- `APPLY_PROFILE`
- `APPLY_CUSTOM`
- `REMOVE_MOUSE`

Every intent carries an opaque caller-generated `intent_id`. Replaying the same ID with identical payload is safe and cannot duplicate side effects; reusing the same ID with different payload is invalid.

Mouse-targeting intents use stable `mouse_id`, never list/page indexes.

Custom editing and dirty draft state remain UI-local. Only `APPLY_CUSTOM` crosses the boundary, carrying the complete five-source mapping atomically.

HOME, Back, Help, Lock, selection, pagination, screen transitions, Inspector and lab controls are not Core intents. When those UI-local actions abandon shared asynchronous work, the only shared command is `CANCEL_ACTIVITY` for the owned activity.

Language-neutral intent fixtures live under `contracts/ui-core/fixtures/uic-02/`.

## 4. Normative async results and ownership

UIC-03 defines the normative asynchronous model in:

`contracts/ui-core/drafts/uic-03-async-results.md`

Core allocates a unique non-reused `activity_id` for every asynchronous activity and
links it to the caller-owned `origin_intent_id`.

Terminal activity states are unique and correlated:

- search: SUCCEEDED / TIMED_OUT / FAILED / CANCELLED;
- operation: SUCCEEDED / FAILED / CANCELLED.

Core exposes an ordered logical notification stream with `ACTIVITY_RESULT` and
`CONNECTION_CHANGED` notifications. The concrete callback/poll/queue binding remains
unfrozen.

Cancellation is normative logical invalidation plus best-effort physical cancellation.
If cancellation commits first, any later backend success cannot mutate confirmed product
truth. If success commits first, a later cancellation is rejected because the activity
is already terminal.

Connection truth changes independently of screen state.

Language-neutral race fixtures live under `contracts/ui-core/fixtures/uic-03/`.

## 5. Normative ordering and commit points

### Single live authority

At most one Mouse is authoritative for product input at any instant.

### Pair New

PAIR_NEW search success only qualifies an unsaved candidate. A separate Core-owned
HANDOFF activity performs promotion under the same originating intent. The old current
Mouse remains authoritative until the atomic handoff success commit. The successful
commit adds/validates the new saved record and changes authority to the new Mouse without
any intermediate dual-authority revision.

### Profile apply

Confirmed profile changes only in the correlated success commit. Pending, failed,
cancelled or late/stale completions do not change confirmed profile.

### Custom apply

Confirmed global Custom mapping and the Mouse's confirmed CUSTOM profile change together
in one successful correlated commit.

### Removal

Offline removal deletes the saved record at its success commit. Live removal first
releases current authority in an earlier semantic commit, then deletes the saved record
in the removal success commit.

### Cancellation and stale/late outcomes

An accepted cancellation makes the target activity terminal CANCELLED logically. Backend
work may finish later physically, but such completion cannot confirm abandoned or newer
UI work.

## 6. Explicit UI-local boundary

The following are proven UI-local because changing them does not alter the semantic obligations of the real Core:

- all 30 screen IDs and exact screen copy;
- HOME resolver implementation and parent-screen navigation;
- Back exceptions and Help modal consumption;
- Lock presentation, lock screen, backlight zeroing/restoration and unlock-to-HOME routing;
- Saved Devices pagination, cursor/selection and cyan presentation;
- display-name formatting/truncation and the ` MOUSE` suffix policy;
- text colors, yellow/cyan/magenta hints, geometry and RGB565 projection;
- SDL2, desktop scaling, virtual backlight gain, Inspector, scenarios, fault controls, event log and evidence bundle;
- private `mouse-ui` product-view/navigation/mock structs, enums and helper APIs.

Some UI-local behavior consumes shared semantic state (for example connection truth), but that does not make the presentation mechanism part of the contract.

## 7. Explicit Core-local boundary

The following are proven Core-local because the frozen product behavior requires only their observable semantic effect, never their representation:

- BTstack and HCI event/callback types;
- GATT/HOGP discovery/session structures;
- raw Bluetooth HID reports;
- Logitech HID++ transaction details;
- TinyUSB descriptors, report structs and endpoint details;
- bonding storage format, flash addresses/sectors and wear-management strategy;
- RP2350/CYW43 scheduling and hardware-driver details;
- GPIO/SPI/ST7789 implementation details;
- internal Core state machine names, queues, mutexes, timers and task topology.

No type from these mechanisms is normative at UIC-00.

## 8. Candidate shared data inventory

The semantic candidates below are traceability targets for later UIC gates, not frozen C fields:

| Candidate datum | Why it crosses the boundary |
|---|---|
| stable Mouse identity | Saved records, remove target, connected authority and stale-safe targeting require identity independent of UI order |
| display name | HOME/Saved/Remove presentation needs a user-visible name |
| saved-record membership | HOME resolver and Saved Devices depend on whether records exist |
| confirmed profile | HOME summary and profile ACTIVE state must reflect confirmed truth |
| authoritative current Mouse identity | single-live invariant and disconnect projection require current identity |
| confirmed Custom mapping/template | applied Custom behavior must survive UI navigation/restart semantics |
| search purpose/status | UI distinguishes first, saved and Pair searches and their outcomes |
| operation kind/status | UI distinguishes apply/remove/handoff progress and completion |
| request/correlation identity | stale/late results must not complete abandoned/new work |
| semantic failure category | retry/error presentation eventually needs stable meaning |
| semantic capabilities/limits | UI must not assume unsupported product capabilities or limits |

## 9. Candidate shared action inventory

| Candidate action | Product origin |
|---|---|
| start first search | no saved Mouse first-use flow |
| start saved reconnect search | HOME with saved-but-disconnected state |
| start Pair New | Pair New flow |
| cancel/abandon search | Back/Help/Lock/leave-screen ownership semantics |
| apply named profile | Passthrough/Standard/Escape |
| apply Custom semantic mapping | Custom Apply |
| remove saved Mouse by identity | Remove This |
| cancel/abandon operation | leave-screen/Lock/Help stale-result protection |

## 10. Candidate shared result inventory

| Candidate result/event | Product origin |
|---|---|
| search found candidate/current Mouse | first/saved/Pair search success |
| search timeout | 8 s / 15 s observable windows |
| search failed | failure/retry behavior |
| search cancelled/abandoned | Back/Help/Lock/leave-screen behavior |
| authoritative connection changed | HOME and active-profile disconnect behavior |
| profile/Custom apply succeeded | ACTIVE only after confirmation |
| profile/Custom apply failed | not-active/retry-safe behavior |
| remove succeeded/failed | removal flow |
| handoff succeeded/failed | Pair New authoritative replacement |
| stale/unknown correlation ignored | late-result safety |

## 11. Normative limits, capabilities and errors

UIC-04 defines these semantics in:

`contracts/ui-core/drafts/uic-04-capabilities-limits-errors.md`

Contract-major-1 public limits are:

- maximum 16 saved Mouse records;
- semantic Mouse name is valid UTF-8, maximum 63 encoded bytes.

A read-only ContractDescriptor publishes major/minor, limits and semantic capability
tokens. Frozen UI 1.0 requires all v1.0 capabilities, including semantic ESCAPE_OUTPUT.

HID++, HCI, GATT, USB descriptors and vendor packet/status values are not public
capabilities.

Stable error semantics consist of category + retryability + visibility. Provider
diagnostic codes/messages may exist but are opaque and UI logic must not branch on them.

Core owns the 8 s FIRST/SAVED and 15 s PAIR_NEW semantic deadlines.

Physical persistence schema remains Core-local; cross-component compatibility depends on
contract version/capabilities/public limits, not flash record layout.

## 11.1 Compatibility analysis

Contract major 1 now carries explicit compatibility commitments for saved capacity, name
bound, required capabilities, error category meanings and search deadlines.

Private UI/Core types, backend technology, flash schema and user-facing error copy may
change freely when these public semantics remain satisfied.

## 12. Open decisions

The authoritative unresolved-decision list is
`contracts/ui-core/drafts/uic-00-unresolved-decisions.md`.

UIC-04 resolves public limit/capability/error/name/timeout semantics and the discovery
part of version compatibility.

Still deferred:

- concrete integer/enum/string/descriptor C representation;
- exact C ABI and function signatures;
- memory ownership and thread-safety;
- internal Custom persistence layout;
- final release version/package/tag.

Those decisions belong to UIC-05 and later gates.
