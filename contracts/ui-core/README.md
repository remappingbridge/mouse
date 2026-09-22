# UI ↔ Core contract

Status: **v1.0.0 RELEASE CONTENT FROZEN BY UIC-08**.

This boundary lets `mouse-ui` consume product truth and emit semantic intentions without
depending on BLE, USB, flash, GPIO, screen layout or other implementation details.

## Active release

The integration target is:

`releases/v1.0.0/`

Primary artifacts:

- `releases/v1.0.0/contract.md` — normative semantics;
- `releases/v1.0.0/schema.json` — machine-readable semantic schema;
- `releases/v1.0.0/uic_v1.h` — released in-process C binding;
- `releases/v1.0.0/fixtures/` — neutral conformance vectors;
- `releases/v1.0.0/compatibility.json` — compatibility rules and proven component pins;
- `releases/v1.0.0/manifest.json` — content-addressed artifact hashes.

The release is proven by:

~~~text
mouse-ui consumer proof:
7d1f0246ca4b70a2a7134de040b02db13e3bd540

mouse-core provider proof:
f5b7384156a42543505b93dd7609e715c4cd3542
~~~

## Directories

- `drafts/` — historical/proposal material; never an integration target after v1.0.0;
- `releases/` — immutable released versions;
- `fixtures/` — historical gate fixtures;
- `traceability/` — accepted/historical product inputs;
- `governance.md` — ownership/promotion/immutability rules;
- `versioning.md` — compatibility/version policy.

## Fundamental rule

Frontend mocks, screen IDs, Help, Lock, SDL, framebuffer, Inspector, scenario controls and
private C structs remain frontend-owned.

BTstack/HCI/GATT handles, TinyUSB report structs, flash layout and hardware timing remain
Core-private.

Only semantic facts/actions/results required across the repository boundary belong in the
released contract.

## Historical candidate

UIC-05 assembled the candidate under `drafts/`; UIC-06 and UIC-07 proved consumer and
provider implementability. UIC-08 promotes the proven semantics into `releases/v1.0.0/`.

The release directory, not the UIC-05 draft, is the canonical integration dependency.
