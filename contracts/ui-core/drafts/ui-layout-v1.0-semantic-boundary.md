# UI Layout 1.0 — UI ↔ Core semantic boundary draft

Status: **WORKING DRAFT — UIC-02 INTENT CANDIDATE — NOT RELEASED**.

Source: frozen `mouse-ui` UI Layout 1.0, `e8adad7919e931c92515bf655ef4050876a8e7a9`.

UIC-00 established semantic ownership and UIC-01 established the accepted language-neutral Snapshot semantics. UIC-02 now defines the normative UI→Core intent vocabulary and submission semantics while leaving result delivery, C ABI, transport and backend implementation unfrozen.

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

## 4. Candidate Core → UI results/events

The asynchronous model must support:

- authoritative Mouse connection changed/disconnected;
- search found / timeout / failure / cancelled;
- Pair New candidate qualification and handoff completion/failure;
- profile apply success/failure/cancelled;
- Custom apply success/failure/cancelled;
- remove success/failure/cancelled;
- any state revision or event correlation needed to reject stale/late outcomes.

A terminal result must identify the request/lifetime it completes. A result for abandoned ownership must not be interpreted as confirmation of a newer UI action.

## 5. Ordering and transaction semantics

### Single live authority

At most one Mouse is authoritative for product input at any instant.

### Pair New

The old current Mouse may remain live while an unsaved candidate is qualified. Promotion of the candidate is a single logical handoff. Cancellation/timeout before handoff completion must not silently promote the candidate.

### Profile apply

The UI may present ACTIVE only after a correlated successful completion. What persistence mechanism produces that success is Core-local.

### Custom

The UI owns the editable draft and dirty state. The shared contract carries only the semantic Custom data needed to request/confirm application and to recover confirmed persistent state.

### Removal

Removal is addressed by stable Mouse identity, not by current list index. If the target is the live Mouse, successful completion semantically implies that it no longer remains authoritative. The exact transport and credential cleanup procedure is Core-local.

### Cancellation and stale/late outcomes

Leaving the UX state that owns an asynchronous action may abandon that action. The shared boundary must let the UI distinguish results belonging to abandoned work from results belonging to the current request. UIC-03 will decide the exact cancellation guarantee and result-delivery model.

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

## 11. Compatibility analysis

UIC-00 introduces no ABI and therefore creates no binary compatibility commitment.

Compatibility obligations identified for later gates:

- semantic identity must remain stable across list reorderings;
- UI and Core must agree on profile and Custom value domains;
- request/result correlation must survive asynchronous delivery;
- future contract revisions must define how unsupported capabilities/limits are represented;
- private UI/Core types may change freely as long as the released semantic contract remains satisfied.

## 12. Open decisions

The authoritative unresolved-decision list is
`contracts/ui-core/drafts/uic-00-unresolved-decisions.md`.

UIC-01 resolved Snapshot schema/atomicity. UIC-02 resolves caller-owned intent identity allocation, atomic ACCEPTED/REPLAY/REJECTED submission semantics and whole-mapping Custom ownership.

Still deferred:

- concrete intent/activity ID type and width;
- cancellation guarantee and async result delivery;
- stale/late result retention;
- exact timeout ownership;
- public error taxonomy;
- capability/limit representation;
- exact C ABI, memory ownership and thread-safety;
- version negotiation and compatibility rules.

Those decisions belong to UIC-03 and later gates.
