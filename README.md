# mouse

Public integration repository for the Remapping Bridge Mouse product.

The project is intentionally split into:

- `remappingbridge/mouse-ui` — exploratory frontend/UX;
- `remappingbridge/mouse-core` — backend/core implementation;
- `remappingbridge/mouse` — public product integration, shared contracts, release composition, and final user-facing application.

## Current phase

The frontend reconstruction is complete and accepted:

~~~text
remappingbridge/mouse-ui
MUI-08 ACCEPTED
commit: 537b0f6fdd188b283cf10648b1cc6dbdacbfe20d
stable baseline ref: baseline/mui-08-accepted
~~~

Product integration code is still intentionally not started.

The next product-level phase is to derive and review the first UI↔Core contract from **accepted frontend semantics**, without exposing frontend-private mock structs, navigation state, SDL tooling, or implementation-specific types.

## Shared contracts

The canonical cross-repository contract namespace is [`contracts/`](contracts/README.md). The UI↔Core boundary is under [`contracts/ui-core/`](contracts/ui-core/README.md).

No stable UI↔Core contract version has been released yet. The accepted MUI-08 frontend baseline is now recorded as contract input/traceability, while the older MBR-08-derived material remains a non-normative historical draft. Acceptance of the frontend does **not** by itself release an ABI.
