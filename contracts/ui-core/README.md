# UI ↔ Core contract

Status: **DRAFT DEFINITION PROGRAM — NO RELEASED CONTRACT VERSION**.

This boundary lets `mouse-ui` consume product truth and emit semantic intentions without depending on BLE, USB, flash, GPIO, screen layout, or other implementation details.

## Directories

- `drafts/` — proposals and working semantic/C-binding definitions;
- `releases/` — immutable released versions once UIC validation is complete;
- `traceability/` — accepted/historical product inputs;
- `governance.md` — ownership and promotion rules;
- `versioning.md` — compatibility/version policy.

## Active frontend source

~~~text
remappingbridge/mouse-ui
UI Layout 1.0
main @ e8adad7919e931c92515bf655ef4050876a8e7a9
release/ui-layout-v1.0 @ same commit
~~~

Business behavior is documented in `mouse-ui/docs/product/ui-layout-v1.0.md`; implementation architecture in `mouse-ui/docs/architecture/ui-layout-v1.0.md`.

Traceability: [`traceability/ui-layout-v1.0.md`](traceability/ui-layout-v1.0.md).

Working contract draft: [`drafts/ui-layout-v1.0-semantic-boundary.md`](drafts/ui-layout-v1.0-semantic-boundary.md).

Current accepted/candidate semantic layers:

- UIC-01 Snapshot: `drafts/uic-01-snapshot-model.md`;
- UIC-02 Intents: `drafts/uic-02-intent-model.md`;
- UIC-03 Async results/ordering: `drafts/uic-03-async-results.md`;
- UIC-04 capabilities/limits/errors candidate: `drafts/uic-04-capabilities-limits-errors.md`.

Language-neutral fixtures live under `fixtures/uic-01/` through `fixtures/uic-04/`.

There is still **no released contract ABI/API**. UIC-05 is responsible for assembling the v1 candidate binding after UIC-04 acceptance.

## Fundamental rule

Frontend mocks are not public contracts. Screen IDs, menu selection, Help, Lock, SDL, framebuffer, Inspector, scenario controls and private C structs remain frontend-owned unless a separate product decision explicitly promotes them.

Likewise, BTstack/HCI/GATT handles, TinyUSB report structs, flash layout and hardware timing remain Core-private.

Only semantic facts/actions/results required across the repository boundary belong here.