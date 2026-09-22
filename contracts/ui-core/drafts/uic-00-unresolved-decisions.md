# UIC-00 — unresolved decisions

Status: **OPEN ITEMS — UIC-01 AND UIC-02 DECISIONS RECORDED BELOW**.

Source baseline: frozen `mouse-ui` UI Layout 1.0 at
`e8adad7919e931c92515bf655ef4050876a8e7a9`.

UIC-00 intentionally does not resolve these questions. They are listed so later gates can close them without smuggling implementation choices into the semantic inventory.

| ID | Open decision | Why unresolved at UIC-00 | Earliest intended gate |
|---|---|---|---|
| OD-003 | Concrete intent/activity ID representation and width | Opaque identity semantics/allocation are fixed; binding type remains open | UIC-05 |
| OD-006 | Result delivery model: poll/event/callback/queue | Async transport concern | UIC-03 |
| OD-007 | Cancellation guarantee: hard cancel, best effort or logical abandonment | Requires async lifecycle design | UIC-03 |
| OD-008 | Late-result retention/unknown-ID behavior | Requires result lifecycle design | UIC-03 |
| OD-009 | Whether 8 s / 15 s timers are Core-owned, UI-owned or negotiated | Product timeout is fixed; timer placement is architecture | UIC-03/UIC-04 |
| OD-010 | Error taxonomy and retryability | Needs Core failure classes and UI needs | UIC-04 |
| OD-011 | Capability/limit representation | Needs product-vs-implementation decision | UIC-04 |
| OD-012 | Whether saved capacity 16 is frozen contract limit or v1 capability value | UI 1.0 exposes 16; future Core constraints need review | UIC-04 |
| OD-013 | Display-name encoding/maximum raw length | UI display formatting is local; shared storage representation is not yet chosen | UIC-04 |
| OD-014 | Escape keyboard-output capability advertisement | Semantic support exists; representation is deferred | UIC-04 |
| OD-016 | Whether Custom template is global persisted state or derived per-device storage internally | Product semantics are global; physical storage is Core-local | UIC-05 |
| OD-017 | Exact C ABI structs/enums/function signatures | Forbidden to freeze before semantic model stabilizes | UIC-05 |
| OD-018 | Memory ownership/lifetime rules for C binding | ABI concern | UIC-05 |
| OD-019 | Thread-safety/reentrancy requirements | Integration/runtime concern | UIC-05 |
| OD-020 | Contract version negotiation/compatibility metadata | Release concern | UIC-05/UIC-08 |
| OD-021 | UI adapter mapping from public contract to private Product View | Must wait for candidate contract | UIC-06 |
| OD-022 | Core conformance harness fixture format | Must wait for candidate contract | UIC-07 |
| OD-023 | Immutable release package contents | Requires both-side conformance | UIC-08 |

## Non-decisions already closed by UIC-00

These are no longer open:

- screen IDs, Help, Lock, pixel layout, SDL, Inspector and lab controls are UI-local;
- BTstack/HCI/GATT/HOGP/HID++/TinyUSB/flash/GPIO/SPI details are Core-local;
- stable Mouse identity, confirmed profile/current state, search/operation semantics, Custom confirmed state and async correlation are shared-contract semantics;
- private `mouse-ui` and `mouse-core` implementation types are not normative contract types.


## Resolved by UIC-01

- **OD-001 — Exact language-neutral snapshot schema:** resolved by `drafts/uic-01-snapshot-model.md`.
- **OD-002 — Snapshot atomicity/revision semantics:** resolved as atomic immutable value semantics with monotonic revision within one producer lifetime.

These resolutions do not freeze a C ABI or async transport API.


## Resolved by UIC-02

- **OD-004 — Who allocates request IDs:** resolved; the UI/caller allocates opaque `intent_id` values and reuses the same ID for retry of the same logical request.
- **OD-005 — Command submission model:** resolved semantically as atomic `ACCEPTED | REPLAY | REJECTED`. Accepted work may still be internally queued; asynchronous completion remains UIC-03.
- **OD-015 — Custom commit granularity:** resolved; Custom draft/editing is UI-local and only the complete five-source mapping crosses the boundary through `APPLY_CUSTOM`.

## Partially resolved by UIC-02

- **OD-003 — Request/correlation identity:** opaque identity semantics and caller allocation are fixed. Concrete type/width remain open for the later binding.
