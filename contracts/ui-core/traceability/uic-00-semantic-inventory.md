# UIC-00 — UI Layout 1.0 semantic ownership inventory

Status: **CANDIDATE — AWAITING HUMAN ARCHITECTURE REVIEW**.

Normative product source:

~~~text
repository: remappingbridge/mouse-ui
document: docs/product/ui-layout-v1.0.md
commit: e8adad7919e931c92515bf655ef4050876a8e7a9
stable ref: release/ui-layout-v1.0
~~~

This inventory classifies the frozen product behavior without freezing any C layout or transport API.

## Classification rule

Each atomic frozen rule below has exactly one owner:

- **UI-local** — screen/navigation/presentation/local draft/lab behavior.
- **Shared-contract semantic** — information, intent, outcome, ordering or lifetime semantics crossing UI↔Core.
- **Core-local** — backend/hardware/transport/persistence mechanism invisible to UI semantics.

## Rule-level matrix

| ID | Source | Atomic frozen rule | Ownership | Contract consequence / rationale |
|---|---|---|---|---|
| BR-001 | §1 | Product presents a local 240×240 interface | UI-local | Logical surface size is presentation, not Core state |
| BR-002 | §1 | Product connects a Bluetooth Mouse | Shared-contract semantic | UI needs semantic discovery/connection truth; transport is hidden |
| BR-003 | §1 | Product chooses a remapping profile | Shared-contract semantic | Profile identity/application crosses the boundary |
| BR-004 | §1 | Product keeps saved devices | Shared-contract semantic | Saved-record membership and identity must be observable |
| BR-005 | §1 | Product recovers after disconnect | Shared-contract semantic | Connection truth/reconnect search outcomes cross the boundary |
| BR-006 | §1 | HAT teaching behavior exists | UI-local | Teaching interaction is presentation/navigation only |
| BR-007 | §1 | Lock hides presentation without redefining product state | UI-local | Lock is not a Core mode |
| BR-008 | §2 | Reference product supports up to 16 saved Mouse records | Shared-contract semantic | Limit/capability must be represented or agreed; exact enforcement deferred |
| BR-009 | §2 | Zero or one Mouse is authoritative/connected at a time | Shared-contract semantic | Core and UI must agree on single-live authority |
| BR-010 | §2 | Connected Mouse appears first in Saved Devices | UI-local | Ordering is a UI projection of current identity |
| BR-011 | §2 | Every saved Mouse has stable identity | Shared-contract semantic | Removal/reorder/current targeting requires identity |
| BR-012 | §2 | Every saved Mouse has a display name | Shared-contract semantic | UI needs semantic name data |
| BR-013 | §2 | Every saved Mouse has a confirmed profile | Shared-contract semantic | Confirmed profile must be observable |
| BR-014 | §2 | Profile domain is Passthrough/Standard/Escape/Custom | Shared-contract semantic | Shared value domain |
| BR-015 | §2 | Custom uses one global five-source template | Shared-contract semantic | Confirmed persistent Custom mapping is shared product state |
| BR-016 | §2 | Screen does not define saved/connected truth | UI-local | UI must project state rather than make screen identity normative |
| BR-017 | §2 | Escape produces synthetic USB Keyboard output | Shared-contract semantic | Escape output capability is product semantics |
| BR-018 | §2 | UI 1.0 does not discover/pair a Bluetooth keyboard | Shared-contract semantic | Capability boundary: no keyboard discovery intent in v1 |
| BR-019 | §3 | With no saved Mouse, HOME resolves to searching-first | UI-local | HOME resolver/screen choice stays in UI |
| BR-020 | §3 | Initial search uses deterministic 8 s cycles | Shared-contract semantic | User-visible timeout semantics cross boundary |
| BR-021 | §3 | Initial search restarts until valid Mouse accepted | Shared-contract semantic | Repeated semantic search lifecycle must be possible |
| BR-022 | §3 | searching-first controls are didactic and do not navigate/lock | UI-local | Input handling is UI-only |
| BR-023 | §3 | first-mouse-connected A/B/X/joystick are didactic; B inert; Y locks | UI-local | Screen-local interaction only |
| BR-024 | §3 | First completed interaction unlocks, is consumed, then resolves HOME | UI-local | Lock/unlock input consumption and routing are UI-only |
| BR-025 | §4 | HOME state resolver uses saved/current semantic state | UI-local | UI chooses screen from shared state |
| BR-026 | §4 | Saved-Mouse reconnect search lasts 8 s | Shared-contract semantic | User-visible search timeout |
| BR-027 | §4 | Saved search can succeed, time out or be cancelled | Shared-contract semantic | Observable search result/status domain |
| BR-028 | §4 | Retry A starts another search | UI-local | Button-to-intent mapping/navigation is UI-owned |
| BR-029 | §4 | HOME connected has four ordered options | UI-local | Menu structure/order is presentation |
| BR-030 | §4 | HOME summary shows confirmed state, never pending request | UI-local | Rendering rule; relies on shared confirmed profile/operation status |
| BR-031 | §5 | Pair New searches for unsaved Mouse for 15 s | Shared-contract semantic | Unsaved-only purpose plus timeout must cross boundary |
| BR-032 | §5 | Existing current Mouse remains authoritative while candidate qualifies | Shared-contract semantic | Concurrent qualification cannot prematurely change authority |
| BR-033 | §5 | Already-saved candidate is not accepted as new | Shared-contract semantic | Pair qualification rule |
| BR-034 | §5 | Pair success performs one logical handoff to new Mouse | Shared-contract semantic | Ordered authoritative handoff result |
| BR-035 | §5 | Abandoned Pair work cannot complete later | Shared-contract semantic | Correlation/lifetime semantics required |
| BR-036 | §5 | Pair Back/Lock/timeout choose UI destinations | UI-local | Screen routing/hints remain UI-only |
| BR-037 | §6 | KEY B is primary Back with explicit screen exceptions | UI-local | Navigation policy |
| BR-038 | §6 | home-searching B means cancel then retry | UI-local | Input/navigation mapping; cancellation intent itself is shared elsewhere |
| BR-039 | §6 | Profile/Custom/Remove/Pair Back destinations are fixed | UI-local | Parent-screen navigation |
| BR-040 | §6 | remapper-options uses KEY B: BACK hint | UI-local | Copy/hint contract is UI-local |
| BR-041 | §7 | Y globally locks product screens when saved Mouse exists, except Help | UI-local | UI interaction policy |
| BR-042 | §7 | Help consumes Y instead of locking | UI-local | Modal input priority |
| BR-043 | §7 | Entering Lock abandons screen-owned async work and invalidates late completion | Shared-contract semantic | UI must be able to cancel/abandon and reject stale result |
| BR-044 | §7 | Lock does not remove Mouse/change confirmed profile/disconnect by itself | UI-local | Lock emits no such Core intents |
| BR-045 | §7 | Locked effective backlight is 0 and restores after unlock | UI-local | Presentation hardware policy is outside UI↔Core product contract |
| BR-046 | §7 | Unlock interaction is consumed and resolves HOME | UI-local | UI input/navigation |
| BR-047 | §8 | Help is modal and consumes complete interaction | UI-local | UI modal behavior |
| BR-048 | §8 | Opening Help abandons source-screen async work | Shared-contract semantic | Owned request lifetime ends |
| BR-049 | §8 | Search Help returns to coherent retry, not cancelled search | UI-local | UI projection/navigation; cancelled work remains invalid |
| BR-050 | §9 | Passthrough mapping semantics | Shared-contract semantic | Core must realize named profile behavior |
| BR-051 | §9 | Standard mapping semantics | Shared-contract semantic | Core must realize named profile behavior |
| BR-052 | §9 | Escape mapping semantics | Shared-contract semantic | Core must realize named profile behavior including Escape output |
| BR-053 | §9 | Custom target domain includes six targets | Shared-contract semantic | Shared Custom value domain |
| BR-054 | §9 | Profile apply is asynchronous and ACTIVE only after success | Shared-contract semantic | Correlated operation status/result required |
| BR-055 | §9 | Desktop normal mode may auto-confirm; lab may inject pending/fail/stale/late | UI-local | Mock/lab behavior is not production contract |
| BR-056 | §9 | Disconnect while profile-active immediately removes connected truth | Shared-contract semantic | Connection truth must update independently of screen |
| BR-057 | §9 | UI remains in same profile family and shows not-active after disconnect | UI-local | Screen projection from shared connection truth |
| BR-058 | §9 | escape-active hints contain only B and Y | UI-local | Copy/hints |
| BR-059 | §10 | Editing Custom changes local draft and may leave it dirty | UI-local | Uncommitted editor draft is private UI state |
| BR-060 | §10 | Applied Custom and draft are distinct | Shared-contract semantic | Shared state represents confirmed applied mapping, not local edits |
| BR-061 | §10 | Only successful apply confirmation makes Custom active | Shared-contract semantic | Operation completion gates confirmed state |
| BR-062 | §11 | Saved Devices shows one record/page with circular pagination | UI-local | Presentation |
| BR-063 | §11 | Connected record is first/cyan; others disconnected | UI-local | Ordering/color projection |
| BR-064 | §11 | Display-name truncation/suffix/fallback formatting policy | UI-local | Core supplies semantic name; UI formats display copy |
| BR-065 | §11 | Connected identity changes may reorder list without changing record identity | Shared-contract semantic | Stable identity independent of presentation order |
| BR-066 | §11 | Remove This target remains same identity if list order changes | Shared-contract semantic | Removal targets stable identity, never list index |
| BR-067 | §12 | KEY A requests removal and normal simulator auto-completes | Shared-contract semantic | Removal request/completion crosses; simulator auto-completion is non-normative mechanism |
| BR-068 | §12 | Removing live Mouse ends authority before logical removal success | Shared-contract semantic | Successful remove implies no remaining live authority for target |
| BR-069 | §12 | Removing last record leads to searching-first; otherwise valid Saved page | UI-local | Post-result navigation |
| BR-070 | §12 | KEY B cancels Remove | UI-local | Button/navigation policy; operation abandonment semantics covered separately |
| BR-071 | §12 | Leaving/Lock/Help prevents old remove result from acting later | Shared-contract semantic | Correlation/lifetime safety |
| BR-072 | §13 | 240×240 RGB565 projection | UI-local | Pixel format/surface |
| BR-073 | §13 | 30 canonical screens | UI-local | Screen inventory |
| BR-074 | §13 | 21-column semantic text width | UI-local | Layout |
| BR-075 | §13 | gray/white/cyan visual meanings | UI-local | Presentation |
| BR-076 | §13 | searching-first/first-connected/learn backgrounds/hints/colors | UI-local | Presentation |
| BR-077 | §14 | Desktop starts at 200% scale | UI-local | Development shell |
| BR-078 | §14 | Desktop starts with 750% virtual backlight | UI-local | Development shell |
| BR-079 | §14 | Inspector/scenarios/fault injection/log/evidence are dev tools | UI-local | Explicitly non-product |
| BR-080 | §15 | BLE/HCI/GATT/HOGP implementation details | Core-local | Transport internals |
| BR-081 | §15 | Real bonding implementation/persistence mechanism | Core-local | Storage mechanism, while saved-state semantics are shared |
| BR-082 | §15 | HID++ details | Core-local | Backend protocol internals |
| BR-083 | §15 | TinyUSB details | Core-local | USB implementation internals |
| BR-084 | §15 | GPIO/SPI/ST7789/RP2350 details | Core-local | Hardware internals |
| BR-085 | §16 | Boundary needs Mouse/profile/search/operation semantic snapshot | Shared-contract semantic | Candidate Core→UI state family |
| BR-086 | §16 | Boundary needs search/apply/remove/Custom intents | Shared-contract semantic | Candidate UI→Core action family |
| BR-087 | §16 | Boundary needs correlatable async results | Shared-contract semantic | Candidate result/lifetime family |
| BR-088 | §16 | Screen IDs/selection/Help/Lock/SDL/pixels/Inspector/lab stay private | UI-local | Explicit negative boundary |

## Candidate shared datum/action/result cross-check

Every shared-contract row maps to at least one candidate boundary family:

- **data**: stable Mouse identity, display name, saved membership, confirmed profile, authoritative current identity, confirmed Custom mapping, search purpose/status, operation kind/status, correlation identity, semantic failures, capabilities/limits;
- **actions**: start first search, start saved reconnect, start Pair New, cancel/abandon search, apply named profile, apply Custom mapping, remove by stable identity, cancel/abandon operation;
- **results/events**: search found/timeout/failure/cancelled, current connection changed, profile/Custom apply outcome, remove outcome, handoff outcome, stale/unknown correlation rejection.

No row requires a screen ID, `mui_*` private type, BTstack type, HCI/GATT type, HID++ packet, TinyUSB struct, flash address or GPIO identifier.

## Ownership proofs

### UI-local proof

The UI-local rules can be changed while keeping the same semantic state/actions/results at the boundary. A different screen graph, copy, color, lock presentation, desktop shell or Inspector implementation would therefore not require Core source changes.

### Core-local proof

The Core-local rules can be replaced by another Bluetooth stack, USB stack, persistence layout or hardware driver while preserving the same semantic saved/current/profile/search/operation behavior. Therefore those implementation types cannot be normative contract types.

### Shared-contract proof

Shared rules either (a) originate from a user action that requires backend effect, (b) represent backend truth the UI must present, or (c) constrain ordering/lifetime of asynchronous work in a user-visible way. They therefore require language-neutral semantics before any C binding is frozen.
