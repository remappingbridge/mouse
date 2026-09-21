# mouse

Public integration repository for the Remapping Bridge Mouse product.

The project is intentionally split into:

- `remappingbridge/mouse-ui` — exploratory frontend/UX;
- `remappingbridge/mouse-core` — backend/core implementation;
- `remappingbridge/mouse` — public product integration, shared contracts, release composition, and final user-facing application.

## Current phase

Only shared contract governance and migration traceability are being established here. Product integration code is intentionally not started yet.

## Shared contracts

The canonical cross-repository contract namespace is [`contracts/`](contracts/README.md). The UI↔Core boundary is under [`contracts/ui-core/`](contracts/ui-core/README.md).

No stable UI↔Core contract version has been released yet. MBR-08-derived material is retained as a draft baseline, not as a frozen ABI.
