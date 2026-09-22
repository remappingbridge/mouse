# UIC-02 — Intent Model

Status: **ACCEPTED UIC-02 BASELINE**.

Source baseline:

~~~text
UIC-01 accepted
UIC-02 human review accepted 2026-09-22
accepted integration baseline before UIC-03: c5a199271eb58c40c527a26926e3eab23e4be5ee
mouse-ui UI Layout 1.0: e8adad7919e931c92515bf655ef4050876a8e7a9
~~~

This document defines the normative language-neutral **UI → Core command vocabulary** for the frozen UI Layout 1.0. It does not define result transport, callbacks, BLE/USB driver commands, C ABI or screen navigation APIs.

## 1. Intent envelope

Every intent has:

- `intent_id` — opaque caller-generated identity, unique within one UI producer lifetime;
- `kind` — one normative intent kind;
- the fields required by that kind.

Exact binary representation/width of `intent_id` is deferred to UIC-05.

### 1.1 Submission disposition

Intent submission has one semantic disposition:

- `ACCEPTED` — preconditions were valid at one atomic acceptance point and the Core owns the requested work;
- `REPLAY` — the same `intent_id`, kind and payload were already accepted; no duplicate side effect may be created;
- `REJECTED` — the intent is malformed or a precondition is false; no product side effect may be created.

`ACCEPTED` does **not** mean the requested asynchronous work succeeded. Completion/failure/cancellation and result delivery are UIC-03 concerns.

A Core implementation may internally queue accepted work, but the semantic acceptance/rejection decision is atomic with respect to one coherent product state.

### 1.2 Replay and collision rules

- retrying the **same logical request** reuses the same `intent_id`;
- same `intent_id` + same kind + same payload is legal and returns `REPLAY`;
- same `intent_id` + different kind or payload is an invalid identity collision and is `REJECTED`;
- submitting equivalent payload under a **new** `intent_id` is a new logical request and is evaluated normally against current state.

These rules make network/API retry safe without making every business action globally idempotent.

## 2. Intent vocabulary

The complete UIC-02 vocabulary is:

~~~text
START_FIRST_SEARCH
START_SAVED_SEARCH
START_PAIR_NEW
CANCEL_ACTIVITY
APPLY_PROFILE
APPLY_CUSTOM
REMOVE_MOUSE
~~~

There is deliberately no intent for HOME, Back, Help, Lock, menu selection, page navigation, screen transition, Custom row editing, pixels, Inspector or lab controls.

## 3. START_FIRST_SEARCH

Shape:

~~~text
intent_id
kind = START_FIRST_SEARCH
~~~

Purpose: discover/qualify the first Mouse when no saved record exists.

Preconditions:

- `saved_mice` is empty;
- `current_mouse_id` is null;
- no active search already owns the discovery slot;
- no conflicting product operation prevents discovery.

Invalid cases include saved records already existing or another search already active.

A new accepted request creates/owns one FIRST search activity correlated to this intent. Exact activity/result lifecycle is UIC-03.

## 4. START_SAVED_SEARCH

Shape:

~~~text
intent_id
kind = START_SAVED_SEARCH
~~~

Purpose: reconnect to one of the already-saved Mice.

Preconditions:

- at least one saved Mouse exists;
- `current_mouse_id` is null;
- no active search already owns the discovery slot;
- no conflicting operation prevents reconnect search.

The Core searches saved identities; UI does not pass list indexes or presentation order.

## 5. START_PAIR_NEW

Shape:

~~~text
intent_id
kind = START_PAIR_NEW
~~~

Purpose: discover an unsaved Mouse and perform the frozen Pair New qualification/handoff flow.

Preconditions:

- one authoritative `current_mouse_id` exists;
- that current identity is saved;
- no other search is active;
- no conflicting operation is active;
- the product can accept another saved Mouse under the capability/limit rules to be finalized by UIC-04.

The intent has **unsaved-only semantics**. An already-saved candidate cannot satisfy it.

The old current Mouse remains authoritative until successful handoff. The UI does not issue a separate screen-specific “accept candidate” command in v1.

## 6. CANCEL_ACTIVITY

Shape:

~~~text
intent_id
kind = CANCEL_ACTIVITY
target_activity_id
~~~

Purpose: request cancellation/abandonment of a currently observable search or operation.

Preconditions:

- `target_activity_id` identifies the currently non-terminal search or operation the caller intends to cancel.

Command-level semantics:

- acceptance atomically logically invalidates the target activity and makes it terminal CANCELLED;
- physical/backend cancellation is best effort and may complete later without product-state effect;
- UIC-03 defines cancellation-vs-success races by semantic commit order;
- UI may submit one cancellation intent per activity when search and operation coexist, such as Pair New handoff.

An unknown, superseded or already-terminal target activity is rejected at submission level. UIC-03 forbids any late backend completion from changing confirmed truth after cancellation.

## 7. APPLY_PROFILE

Shape:

~~~text
intent_id
kind = APPLY_PROFILE
target_mouse_id
profile = PASSTHROUGH | STANDARD | ESCAPE
~~~

Purpose: request one named non-Custom profile for the authoritative Mouse.

Preconditions:

- `target_mouse_id` identifies a saved Mouse;
- it equals `current_mouse_id`;
- profile belongs to the three non-Custom named profiles;
- no conflicting operation is active for product state mutation.

Stable target identity is mandatory. No list index or “current screen Mouse” shorthand is normative.

If the requested profile is already confirmed, submission is legal; Core may treat it as an already-satisfied no-op while preserving correlation semantics to be specified by UIC-03.

If current authority changed since the UI observed its Snapshot, the stale target request is rejected rather than retargeted to the new current Mouse.

## 8. Custom ownership decision

UIC-02 makes the following normative ownership decision:

- Custom **editing**, row selection and dirty draft state are UI-local;
- Core owns only the **confirmed global Custom mapping**;
- no `EDIT_CUSTOM_ROW`, `SAVE_CUSTOM_DRAFT` or per-row persistence intent exists;
- UI submits the complete five-source mapping only when the user chooses Apply Custom.

Thus a user may edit freely without mutating Core-confirmed state.

## 9. APPLY_CUSTOM

Shape:

~~~text
intent_id
kind = APPLY_CUSTOM
target_mouse_id
mapping:
  LEFT     -> target
  RIGHT    -> target
  MIDDLE   -> target
  FORWARD  -> target
  BACKWARD -> target
~~~

Each target is one of:

`LEFT | RIGHT | MIDDLE | ESCAPE | FORWARD | BACKWARD`.

Preconditions:

- `target_mouse_id` is saved and equals `current_mouse_id`;
- the mapping contains exactly the five required sources;
- every target belongs to the valid target domain;
- no conflicting product-mutation operation is active.

The complete mapping is one atomic semantic request. Partial mapping mutation is invalid.

Until successful completion, `custom_confirmed` and the target Mouse's `confirmed_profile` remain unchanged.

## 10. REMOVE_MOUSE

Shape:

~~~text
intent_id
kind = REMOVE_MOUSE
target_mouse_id
~~~

Purpose: remove one saved Mouse association by stable identity.

Preconditions:

- `target_mouse_id` exists in `saved_mice`;
- no conflicting product-mutation operation is active.

The target may be current or offline.

The intent must never contain page index, list index or display position. If presentation order changes after the user opened Remove This, stable identity preserves the target.

Successful live-Mouse removal semantics remain those defined by the Snapshot model: the removed identity cannot remain authoritative.

## 11. Conflict model

UIC-02 defines a conservative single-mutation model:

- at most one search activity is active;
- at most one product-mutation operation is active;
- a new APPLY_PROFILE, APPLY_CUSTOM or REMOVE_MOUSE is rejected while another mutation operation is pending;
- START_FIRST_SEARCH and START_SAVED_SEARCH require no active search;
- START_PAIR_NEW requires no active search and no mutation operation;
- CANCEL_ACTIVITY may target whichever observable activity currently exists.

Pair New may evolve from search qualification into HANDOFF operation under Core ownership without a second UI intent.

Exact ordering between cancellation, terminal results and Snapshot publication is UIC-03.

## 12. Invalid request behavior

A rejected intent creates no new search, operation, saved record, profile change, Custom change, current-authority change or removal.

Normative invalid examples:

- START_FIRST_SEARCH when saved records exist;
- START_SAVED_SEARCH when a current Mouse is already authoritative;
- START_PAIR_NEW with no current Mouse;
- APPLY_PROFILE/Custom to an offline or stale non-current identity;
- APPLY_PROFILE with CUSTOM as its profile;
- APPLY_CUSTOM with missing/extra Custom source or invalid target;
- REMOVE_MOUSE for an unknown identity;
- CANCEL_ACTIVITY for an unknown/non-current activity;
- reusing an `intent_id` with a different payload.

UIC-04 will define stable public error categories. UIC-02 defines the invalid conditions, not the final error-code taxonomy.

## 13. Retry/idempotency expectations

| Situation | Required behavior |
|---|---|
| transport/client retries same accepted request | reuse same `intent_id`; Core returns REPLAY and creates no duplicate side effect |
| user deliberately tries action again after terminal failure | use a new `intent_id` |
| user deliberately reapplies already-confirmed named profile | legal new request; may complete as already-satisfied no-op |
| user retries REMOVE after it already succeeded using new ID | rejected because target no longer exists |
| user retries old request ID with changed target/payload | rejected as ID collision |
| stale UI applies profile to Mouse that is no longer current | rejected; never retarget to current Mouse implicitly |

## 14. Frozen UI action mapping

| Frozen UI/product action | Normative intent / ownership |
|---|---|
| initial discovery cycle begins | START_FIRST_SEARCH |
| retry saved-Mouse search | START_SAVED_SEARCH |
| Pair New starts | START_PAIR_NEW |
| Back/Help/Lock/leave abandons owned async activity | CANCEL_ACTIVITY for each owned shared activity; navigation itself is UI-local |
| select Passthrough and apply | APPLY_PROFILE(PASSTHROUGH) |
| select Standard and apply | APPLY_PROFILE(STANDARD) |
| select Escape and apply | APPLY_PROFILE(ESCAPE) |
| edit any Custom row | UI-local; no intent |
| APPLY CUSTOM | APPLY_CUSTOM with complete mapping |
| KEY A confirms Remove This | REMOVE_MOUSE(target identity) |
| HOME resolver | UI-local |
| Saved Devices pagination | UI-local |
| Back parent navigation | UI-local |
| Help modal | UI-local except cancellation of owned activity |
| Lock presentation/unlock routing | UI-local except cancellation of owned activity |
| Learn the Keys / didactic controls | UI-local |

Every shared UI action therefore maps to exactly one normative intent kind, while presentation/navigation actions remain UI-local.

## 15. Intent-to-Snapshot relationship

An accepted intent does not directly mutate UI-confirmed truth.

Examples:

- APPLY_PROFILE accepted → Snapshot may show PROFILE_APPLY PENDING while saved record keeps old `confirmed_profile`;
- APPLY_CUSTOM accepted → Snapshot may show CUSTOM_APPLY PENDING while `custom_confirmed` stays old;
- REMOVE_MOUSE accepted → target remains saved until removal success;
- START_PAIR_NEW accepted → old current remains authoritative during qualification/handoff.

This preserves the UIC-01 confirmed/requested separation.

## 16. Compatibility analysis

UIC-02 still freezes no C ABI.

Semantic compatibility requirements:

- intent kind names/meanings cannot be silently repurposed;
- stable target identity semantics must remain index-independent;
- duplicate replay must not create a second side effect;
- a future binding may encode intent IDs differently while preserving opaque identity/replay semantics;
- adding screen/navigation intent kinds would violate the ownership boundary;
- per-row Custom persistence would change the accepted Custom ownership model and require explicit contract review.

## 17. Decisions resolved by UIC-02

UIC-02 resolves:

- **OD-004** — the UI/caller allocates opaque `intent_id` values;
- **OD-005** — every submission has atomic semantic disposition ACCEPTED/REPLAY/REJECTED; accepted work may be queued internally;
- **OD-015** — Custom editing/draft remains UI-local and only the complete mapping crosses through APPLY_CUSTOM.

Partially resolved:

- **OD-003** — identity semantics and allocation owner are fixed, but concrete type/width remain deferred to UIC-05.

Still deferred to UIC-03 or later:

- concrete notification transport binding;
- exact search timer ownership;
- public error taxonomy;
- capabilities/limits representation;
- C ABI and memory/threading rules.
