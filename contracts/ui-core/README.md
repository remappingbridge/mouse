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


## Accepted frontend input

The current accepted frontend baseline is:

~~~text
remappingbridge/mouse-ui@537b0f6fdd188b283cf10648b1cc6dbdacbfe20d
baseline/mui-08-accepted
MUI-08 ACCEPTED
~~~

See `traceability/mui-08-accepted-frontend.md`.

This acceptance freezes a trustworthy UX/reference implementation for contract analysis. It does **not** make frontend-private Product View structs, mock intents/results, navigation enums, Inspector metadata, scenario controls, SDL behavior, or timing implementation details part of the public UI↔Core contract.
