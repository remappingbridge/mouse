# MBR-08 baseline boundary draft

Status: **NON-NORMATIVE EXPLORATORY DRAFT**.

Source baseline: `remappingbridge/mouse-bridge-remapper@93f4fa3a7eb792c3f32ba431252485b142eee063`, experimental MBR-08 branch.

This document captures the boundary concepts implied by the MBR-08 product so the new frontend can mock them without declaring a stable ABI.

## Candidate state exposed to UI

Conceptually, the UI may need:

- product has zero or more saved Mouse records;
- zero or one current authoritative connected Mouse;
- saved Mouse identity and display name;
- confirmed per-Mouse profile kind: Passthrough, Standard, Escape, Custom;
- global Custom template plus draft/dirty projection where UX requires it;
- search purpose/outcome: first, saved reconnect, Pair New, timeout/cancel;
- enough operation state to distinguish intent from confirmed success for profile apply/removal/handoff;
- connected/disconnected truth independent of which page is currently displayed;
- errors/failures that are intentionally made user-visible.

Exact C structs, enum names, callback mechanisms, ownership model, and async API shape are deliberately unspecified.

## Candidate semantic intentions emitted by UI

Conceptually:

- resolve/go Home;
- start/cancel/retry Pair New or saved search where product UX exposes it;
- choose/apply a profile;
- edit/apply Custom mappings;
- inspect saved devices;
- request removal of a saved Mouse;
- acknowledge/return from UI-owned presentation states.

Navigation-only actions that never need Core should remain private to `mouse-ui`.

## Timing

Real search deadlines and operation completion are Core/product facts. The exploratory frontend may use a deterministic fake clock, but the eventual contract should expose semantics rather than require the UI to reproduce BT timing internals.

## Lock

UI Lock is presentation ownership. MBR-08 explicitly allowed the Mouse data path to remain functional while the display/backlight is locked. The UI↔Core contract should therefore avoid treating Lock as transport teardown unless a later product decision changes that rule.

## Next step

Do not promote this draft merely because frontend implementation begins. Promotion waits for accepted UX plus an integration-ready Core boundary.
