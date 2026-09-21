# UI ↔ Core contract

Status: **DRAFTS ONLY — NO RELEASED VERSION**.

This boundary lets `mouse-ui` consume product state and emit semantic intentions without depending on BLE, USB, flash, GPIO, or other backend implementation details.

## Directories

- `drafts/` — proposals and exploratory boundary definitions;
- `releases/` — immutable released contract versions once promoted;
- `traceability/` — exact historical source documents that informed the first draft;
- `governance.md` — ownership and promotion rules;
- `versioning.md` — compatibility/version policy.

## Fundamental rule

Frontend mocks are not automatically public contracts. A requirement discovered in UX becomes a cross-repository obligation only after explicit promotion here.
