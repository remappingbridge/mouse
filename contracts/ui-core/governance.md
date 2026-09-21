# UI ↔ Core contract governance

## Roles

- `mouse-ui` discovers and validates UX needs;
- `mouse-core` discovers backend constraints and implements product behavior;
- `mouse` owns the neutral public contract and final integration.

## Promotion flow

1. UX may experiment with private mock/state structures in `mouse-ui`.
2. Once a UX behavior is accepted, identify only the information/actions that must cross the UI↔Core boundary.
3. Propose a contract draft here.
4. Validate that the draft does not leak UI layout internals or backend transport internals.
5. Implement/verify both sides against the draft.
6. Promote to an immutable released version only when integration is ready to depend on it.

## What belongs in the contract

- semantic state needed by presentation;
- stable identifiers/value domains needed by both sides;
- semantic actions/commands and their observable completion/failure;
- ordering/lifetime rules that affect user-visible behavior;
- explicit compatibility/version information.

## What does not belong

- pixel coordinates, fonts, colors, screen IDs used only inside UI;
- BTstack events/handles, HCI/GATT structs, HID raw reports;
- TinyUSB report structs/descriptors;
- flash addresses/sector layout;
- GPIO/SPI details;
- private mock controls;
- internal Core states that have no user-visible contractual effect.

## Conflict rule

If UX desire and backend feasibility conflict, neither side silently changes the other. The contract proposal records the smallest shared semantic model that supports an accepted product decision.
