# UI Layout 1.0 — UI ↔ Core semantic boundary draft

Status: **WORKING DRAFT — UIC PROGRAM INPUT — NOT RELEASED**.

Source: frozen `mouse-ui` UI Layout 1.0, `e8adad7919e931c92515bf655ef4050876a8e7a9`.

## Goal

Define the smallest semantic boundary required to replace the frontend mock with a real Core without coupling either repository to the other's private state machine.

## 1. Candidate Core → UI Snapshot

A snapshot must be able to express, at minimum:

- persistent saved Mouse records with stable product identity and display name;
- confirmed profile for each saved Mouse;
- zero or one current authoritative Mouse identity;
- the persistent global Custom mapping/template needed by the accepted UX;
- current search purpose/status plus a correlation identity;
- current product operation kind/status/target plus a correlation identity;
- user-visible failure information once error taxonomy is defined;
- capability/limit information needed to render or validate safely.

Candidate value domains derived from UI 1.0:

~~~text
profile: PASSTHROUGH | STANDARD | ESCAPE | CUSTOM
custom target: LEFT | RIGHT | MIDDLE | ESCAPE | FORWARD | BACKWARD
search purpose: FIRST | SAVED | PAIR_NEW
search status: IDLE | RUNNING | FOUND | TIMED_OUT | FAILED | CANCELLED
operation: PROFILE_APPLY | CUSTOM_APPLY | REMOVE | HANDOFF
operation status: IDLE | PENDING | SUCCEEDED | FAILED
~~~

Exact C layout is deliberately unspecified at this stage.

## 2. Candidate UI → Core Intents

Cross-boundary intents likely required:

- start first-Mouse search;
- start saved-Mouse reconnect search;
- start Pair New (unsaved-only);
- cancel the currently owned search;
- apply Passthrough/Standard/Escape profile to the current Mouse;
- edit/persist one Custom source→target choice or submit the whole draft, depending on final ownership decision;
- apply the Custom profile/template;
- request removal of a saved Mouse;
- cancel an owned operation if the final Core API supports explicit cancellation.

HOME navigation, Help, Lock, list selection, screen Back and Inspector operations are UI-local and are not Core intents.

## 3. Candidate Events / Results

The asynchronous model must support:

- connection/current-Mouse changed;
- search found / timeout / failure / cancelled;
- profile/custom apply success or failure;
- remove success or failure;
- handoff completion/failure;
- correlation identifiers that let UI reject stale or late outcomes.

Results must identify the request/lifetime they complete. A result for abandoned ownership must not be interpreted as confirmation of a newer UI action.

## 4. Ordering and transaction semantics

### Single live authority

At most one Mouse is authoritative for input at any instant.

### Pair New

The old current Mouse may remain live while an unsaved candidate is qualified. Promotion of the candidate must be an ordered handoff; cancellation/timeout before commit must leave the old current Mouse unchanged if it is still physically present.

### Profile apply

Success means the runtime mapping is effective **and** required persistence is committed/verified. UI must not receive success earlier.

### Removal

If removing the live Mouse, stop/release its output and terminate its live authority before reporting successful deletion of product association/credential state.

### Cancellation

Leaving the owning UX state can cancel interest in the result. UIC-03 must decide whether cancellation is best-effort transport cancellation, logical request invalidation, or both. In all cases the UI must be able to reject old correlation IDs.

## 5. Limits/capabilities to resolve

UIC-04 must decide which limits are public product limits versus frontend implementation limits:

- maximum saved Mouse count (UI 1.0 reference uses 16);
- maximum/encoding of display name (frontend private storage is 64 bytes);
- support/capability flags for Escape keyboard output and optional Logitech HID++ behavior;
- persistence/version compatibility;
- stable error taxonomy and retryability.

## 6. UI adapter boundary

`mouse-ui` should eventually contain a thin adapter:

~~~text
public contract Snapshot/Event -> adapter -> private Product View
private navigation Intent -> adapter -> public contract command
~~~

The existing mock stays useful for desktop scenarios/tests, but production integration does not expose mock headers to Core.

## 7. Core adapter boundary

`mouse-core` implements the contract through its own state machine. It must not include `mouse-ui` headers or know screen IDs, selection, Help, Lock, SDL, colors, pixels or Inspector metadata.

## 8. Open decisions before v1 release

- exact language-neutral schema;
- exact C binding and ownership/lifetime rules;
- synchronous submission vs queued commands;
- request/correlation ID type and allocation owner;
- event delivery/poll/callback mechanism;
- cancellation guarantee;
- error categories;
- capabilities/limits representation;
- snapshot atomicity/thread-safety;
- version negotiation and compatibility policy.

These are resolved by UIC-00 through UIC-08; this file must not be promoted verbatim without that evidence.