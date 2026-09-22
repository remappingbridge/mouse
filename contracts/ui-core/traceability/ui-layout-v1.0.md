# Frozen mouse-ui UI Layout 1.0

Status: **NORMATIVE PRODUCT INPUT FOR CONTRACT ANALYSIS — NOT A CONTRACT RELEASE**.

Frozen on: **2026-09-22**.

## Reference

~~~text
repository: remappingbridge/mouse-ui
application version: 1.0.0
UI Layout: 1.0
commit: e8adad7919e931c92515bf655ef4050876a8e7a9
stable ref: release/ui-layout-v1.0
behavior merge: 65c2040f71dd68bd054fc9c3aa24d47d53e58ead
CI: 35682121924 — 13/13 tests in Debug and ASan/UBSan
~~~

## Product semantics that can create Core obligations

- zero or one authoritative live Mouse;
- multiple stable saved Mouse identities (frontend reference capacity 16);
- saved/current name and confirmed profile;
- profiles Passthrough, Standard, Escape and Custom;
- persistent global Custom template semantics;
- first search and saved search observable windows of 8 seconds;
- Pair New new-only observable window of 15 seconds while current Mouse can remain live;
- atomic/ordered replacement handoff semantics;
- profile Apply not user-confirmed until runtime+persistence success;
- removal outcome and live-session cleanup before successful deletion;
- disconnect truth independent of current UI page;
- cancellation/ownership semantics so late/stale results cannot complete abandoned UI work;
- user-visible operation/search success, failure, timeout and cancellation states.

## Frontend-private semantics that are not contract obligations

- the 30 screen IDs and their exact copy/geometry;
- selection/page state;
- Help ownership;
- Lock presentation/backlight behavior;
- semantic element/Inspector IDs and bounds;
- SDL2 shell, scale and virtual backlight;
- deterministic lab scenarios and fault buttons;
- private `mui_*` C structs and enum layouts;
- framebuffer/renderer internals.

## Core-private implementation choices

- BTstack/HCI/GATT/HOGP object layout and callback APIs;
- raw HID reports;
- HID++ transaction structures;
- TinyUSB descriptors/report structs;
- flash addresses/sectors;
- RP2350/CYW43/GPIO/SPI timing.

## Contract derivation principle

The contract should make every frozen cross-boundary business rule implementable while leaving both repositories free to rewrite their internals.