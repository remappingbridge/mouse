# UIC-00 — unresolved decisions

Status: **OPEN ITEMS — UIC-01 THROUGH UIC-04 DECISIONS RECORDED BELOW**.

Source baseline: frozen `mouse-ui` UI Layout 1.0 at
`e8adad7919e931c92515bf655ef4050876a8e7a9`.

UIC-00 intentionally does not resolve these questions. They are listed so later gates can close them without smuggling implementation choices into the semantic inventory.

| ID | Open decision | Why unresolved at UIC-00 | Earliest intended gate |
|---|---|---|---|
| OD-003 | Concrete intent/activity ID representation and width | Opaque identity semantics/allocation are fixed; binding type remains open | UIC-05 |
| OD-016 | Whether Custom template is global persisted state or derived per-device storage internally | Product semantics are global; physical storage is Core-local | UIC-05 |
| OD-017 | Exact C ABI structs/enums/function signatures | Forbidden to freeze before semantic model stabilizes | UIC-05 |
| OD-018 | Memory ownership/lifetime rules for C binding | ABI concern | UIC-05 |
| OD-019 | Thread-safety/reentrancy requirements | Integration/runtime concern | UIC-05 |
| OD-020 | Final released version constants/package compatibility metadata | Discovery/major-minor rules are defined; release binding/tag remains open | UIC-05/UIC-08 |
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


## Resolved by UIC-03

- **OD-006 — Result delivery model:** resolved semantically as one ordered logical
  notification stream per Core producer lifetime. Concrete poll/callback/queue binding is
  intentionally not frozen.
- **OD-007 — Cancellation guarantee:** resolved as immediate logical invalidation plus
  best-effort physical/backend cancellation. Semantic commit order decides cancel-vs-success races.
- **OD-008 — Late-result retention/unknown-ID behavior:** every activity has one terminal
  transition; late backend completion after cancellation cannot mutate confirmed product
  truth, and delayed notification cannot complete newer work.
- **OD-003 activity side:** Core allocates unique non-reused `activity_id` values and
  links every activity to one caller-owned `origin_intent_id`.

Concrete ID representation/width remains OD-003 work for UIC-05.


## Resolved by UIC-04

- **OD-009 — Search timer ownership:** Core owns semantic deadlines: 8 s FIRST, 8 s SAVED,
  15 s PAIR_NEW, using a monotonic source. Hardware timer/tick implementation remains private.
- **OD-010 — Error taxonomy/retryability:** stable categories, retryability and visibility
  are defined; backend/vendor codes are diagnostic-only.
- **OD-011 — Capability/limit representation:** a read-only semantic ContractDescriptor
  publishes major/minor, public limits and semantic capability tokens.
- **OD-012 — Saved capacity:** 16 is the public maximum for contract major 1.
- **OD-013 — Display-name contract:** valid UTF-8, maximum 63 encoded bytes; Core ensures
  contract validity and UI retains display-specific truncation/suffix/fallback behavior.
- **OD-014 — Escape output:** ESCAPE_OUTPUT is a required semantic capability; USB/TinyUSB
  representation remains private.
- **OD-020 discovery portion:** major/minor compatibility and required-capability discovery
  are defined. Concrete binding constants and immutable release package remain later work.
