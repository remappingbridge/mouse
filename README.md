# mouse

Public integration repository for the Remapping Bridge Mouse product.

The project is intentionally split into:

- `remappingbridge/mouse-ui` — frozen frontend/UX reference plus future versioned UX evolution;
- `remappingbridge/mouse-core` — backend/Core implementation;
- `remappingbridge/mouse` — neutral public contracts, final composition and product releases.

## Current product input

The frontend has reached its first product-level freeze:

~~~text
repository: remappingbridge/mouse-ui
application version: 1.0.0
UI Layout: 1.0
status: FROZEN / ACCEPTED
main: e8adad7919e931c92515bf655ef4050876a8e7a9
stable ref: release/ui-layout-v1.0
~~~

This supersedes MUI-08 as the current semantic source for new contract work. MUI-08 remains historical reconstruction evidence.

## Current phase

Product integration code is still intentionally not started. The active next phase is **UI ↔ Core Contract v1 definition and conformance planning**.

The contract must be derived from the frozen UI Layout 1.0 **business semantics**, not by publishing `mouse-ui` private C structs. The desired boundary is conceptually:

~~~text
Core -> Snapshot / Event -> UI adapter -> private mouse-ui Product View
UI navigation -> Intent -> UI adapter -> Core
~~~

## Shared contracts

The canonical cross-repository namespace is [`contracts/`](contracts/README.md). The UI↔Core boundary is under [`contracts/ui-core/`](contracts/ui-core/README.md).

No stable UI↔Core contract version has been released yet. `contracts/ui-core/drafts/ui-layout-v1.0-semantic-boundary.md` is the new working boundary draft. It is not ABI v1 and may still change during UIC gates.