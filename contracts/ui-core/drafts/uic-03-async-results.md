# UIC-03 — Async Results & Ownership

Status: **CANDIDATE — AWAITING HUMAN REVIEW**.

Source baseline:

~~~text
UIC-02 accepted
mouse main: c5a199271eb58c40c527a26926e3eab23e4be5ee
repo-planner main: 2485ae2add2b0425f8157efb1811d540274a70c2
mouse-ui UI Layout 1.0: e8adad7919e931c92515bf655ef4050876a8e7a9
~~~

This document defines the normative language-neutral asynchronous lifecycle,
correlation, cancellation and ordering semantics required by UI Layout 1.0.

It does **not** freeze a thread model, event loop, callback API, queue implementation,
hardware timing mechanism or C ABI.

## 1. Identity and correlation

UIC-02 already established caller-owned `intent_id`.

UIC-03 adds Core-owned `activity_id`.

### 1.1 intent_id

- allocated by the UI/caller;
- unique within one UI producer lifetime;
- reused only when retrying the same logical intent;
- identifies the user/Core request, not an implementation task.

### 1.2 activity_id

- allocated by Core;
- unique and never reused within one Core producer lifetime;
- identifies one asynchronous activity;
- every activity records exactly one `origin_intent_id`;
- one accepted intent may create more than one activity when the product flow has phases.

Pair New is the canonical multi-activity example:

~~~text
START_PAIR_NEW intent P
  -> search activity A, origin_intent_id = P
  -> if unsaved candidate qualifies:
       handoff activity B, origin_intent_id = P
~~~

The exact binary type/width of both identities remains deferred to UIC-05.

## 2. Activity classes

The contract recognizes two asynchronous classes.

### Search activity

Kinds:

- FIRST
- SAVED
- PAIR_NEW

Non-terminal state:

- RUNNING

Terminal states:

- SUCCEEDED
- TIMED_OUT
- FAILED
- CANCELLED

Snapshot projection:

- SUCCEEDED maps to search status `FOUND`;
- TIMED_OUT maps to `TIMED_OUT`;
- FAILED maps to `FAILED`;
- CANCELLED maps to `CANCELLED`.

A successful search may carry one candidate.

### Product operation

Kinds:

- PROFILE_APPLY
- CUSTOM_APPLY
- REMOVE
- HANDOFF

Non-terminal state:

- PENDING

Terminal states:

- SUCCEEDED
- FAILED
- CANCELLED

An activity has exactly one terminal transition.

Terminal classification is exclusive:

- CANCELLED means an accepted logical cancellation invalidated the activity before success;
- TIMED_OUT is search-only and means the product deadline expired before another terminal state;
- FAILED means the activity could not complete because of backend/environment/product failure;
- SUCCEEDED means its normative commit point was reached.

A terminal transition may be delivered more than once by a binding, but repeat delivery
does not create another terminal transition or side effect.

## 3. Logical notification stream

Core exposes one ordered **logical notification stream** per Core producer lifetime.

This is a semantic ordering abstraction, not a required queue/callback mechanism.

Every notification has a Core-allocated monotonically increasing `notification_seq`.

Normative notification kinds:

- `ACTIVITY_RESULT`
- `CONNECTION_CHANGED`

A language binding may expose notifications through polling, callback, queue or another
mechanism, but it must preserve the observable semantic ordering.

### 3.1 ACTIVITY_RESULT

Fields:

~~~text
notification_seq
kind = ACTIVITY_RESULT
activity_id
origin_intent_id
activity_kind
terminal_status
commit_revision
optional candidate
optional cancel_intent_id
~~~

Rules:

- `activity_id` uniquely identifies the completed activity;
- `origin_intent_id` identifies the originating UI request;
- `terminal_status` is unambiguous;
- `commit_revision` identifies the first Snapshot revision whose confirmed product
  truth includes the terminal commit, or the revision that records cancellation/failure
  without changing confirmed truth;
- exact public failure code belongs to UIC-04.

### 3.2 CONNECTION_CHANGED

Fields:

~~~text
notification_seq
kind = CONNECTION_CHANGED
previous_current_mouse_id
current_mouse_id
commit_revision
reason
~~~

`reason` is semantically one of:

- PHYSICAL_DISCONNECT
- PHYSICAL_CONNECT
- SEARCH_RECONNECT
- FIRST_ACCEPT
- HANDOFF
- REMOVE_RELEASE

The exact public encoding of reasons may be refined by UIC-04/UIC-05.

A connection change is product truth and is emitted independently of which UI screen is
currently visible.

## 4. Snapshot and notification ordering

For every semantic commit:

1. Core determines one atomic product-state transition.
2. A new Snapshot revision represents that transition.
3. Any notification describing that transition carries that same `commit_revision`.
4. The notification must never claim a state transition not represented by that
   revision or a later equivalent revision.

Consumers may receive the Snapshot before or after the notification at the transport
level, but correlation by `commit_revision` must make the semantic order unambiguous.

When one success commit itself changes current authority (FIRST acceptance, SAVED reconnect
or HANDOFF), the logical notification order for that commit is always:

1. CONNECTION_CHANGED
2. ACTIVITY_RESULT(SUCCEEDED)

A consumer must never interpret an older result as confirmation of a newer intent merely
because delivery was delayed.

## 5. Cancellation semantics

UIC-03 chooses **logical invalidation plus best-effort physical cancellation**.

### 5.1 Cancellation acceptance

An accepted `CANCEL_ACTIVITY` applies only to a currently non-terminal activity.

At one atomic cancellation point:

- the target activity becomes logically invalidated;
- its terminal state becomes CANCELLED;
- an `ACTIVITY_RESULT(... CANCELLED ...)` is created;
- confirmed product state that had not already committed remains unchanged.

The cancellation intent's `intent_id` is recorded as `cancel_intent_id` on the
target's terminal result.

### 5.2 Physical work

After logical cancellation Core should attempt to stop unnecessary physical/backend work,
but physical stop is best effort.

A physical completion that arrives after logical cancellation is a **late physical
completion** and cannot mutate confirmed product truth.

### 5.3 Cancellation race rule

Cancellation and successful commit are totally ordered by the Core semantic commit order.

- if success commits first, the activity is terminal SUCCEEDED and later cancellation is
  rejected because the target is no longer active;
- if cancellation commits first, the activity is terminal CANCELLED and later physical
  success is ignored for product truth.

There is no state in which one activity is both SUCCEEDED and CANCELLED.

## 6. Stale and late results

A result is stale/late for a UI owner when its `activity_id` is no longer the activity
owned by that UI flow, or when that activity was already cancelled/terminal.

Normative rules:

- stale/late delivery cannot change confirmed profile;
- stale/late delivery cannot remove a Mouse;
- stale/late delivery cannot promote a Pair New candidate;
- stale/late delivery cannot replace current authoritative Mouse;
- stale/late delivery cannot complete a newer intent;
- matching only by operation kind, Mouse ID or screen is insufficient;
- matching must use activity correlation.

Core must protect product truth from late backend completions even if the UI also rejects
the stale notification.

## 7. Search lifecycle

### 7.1 FIRST search

~~~text
START_FIRST_SEARCH accepted
 -> FIRST activity RUNNING
 -> exactly one terminal:
      SUCCEEDED(candidate)
      TIMED_OUT
      FAILED
      CANCELLED
~~~

On successful first acceptance, the semantic commit atomically:

- creates the saved Mouse record;
- establishes it as the single current authoritative Mouse;
- gives it one valid confirmed profile;
- terminates the FIRST activity as SUCCEEDED.

No intermediate Snapshot may expose a current identity absent from `saved_mice`.

### 7.2 SAVED search

~~~text
START_SAVED_SEARCH accepted
 -> SAVED activity RUNNING
 -> SUCCEEDED(saved mouse) | TIMED_OUT | FAILED | CANCELLED
~~~

On success, the selected saved identity becomes current in the success commit revision.
No saved identity is created by SAVED search.

### 7.3 PAIR_NEW search

~~~text
START_PAIR_NEW accepted
 -> PAIR_NEW search RUNNING
 -> candidate qualifies as unsaved
 -> search terminal SUCCEEDED/FOUND
 -> Core creates HANDOFF operation under same origin_intent_id
~~~

The PAIR_NEW search success itself does **not** change current authority.

## 8. Pair New handoff ordering

Let M1 be the old current Mouse and M2 the unsaved candidate.

Normative sequence:

~~~text
R0: current = M1; M2 not saved
A:  PAIR_NEW search RUNNING
R1: search SUCCEEDED with candidate M2; current still M1
B:  HANDOFF PENDING targeting M2; current still M1
R2: HANDOFF success commit:
      M2 is a valid saved record
      current changes atomically from M1 to M2
      at no revision are M1 and M2 both authoritative
~~~

If HANDOFF fails or is cancelled:

- M1 remains current unless an independent physical connection event changed truth;
- M2 is not promoted by that abandoned handoff;
- late backend success for B is ignored.

The successful handoff commit emits:

- `CONNECTION_CHANGED(previous=M1,current=M2,reason=HANDOFF)`;
- `ACTIVITY_RESULT(B, SUCCEEDED)`;

both correlated to the same commit revision.

For the same handoff commit revision, the logical notification order is fixed:

1. CONNECTION_CHANGED
2. ACTIVITY_RESULT(HANDOFF SUCCEEDED)

## 9. Profile apply commit point

For `APPLY_PROFILE`:

### Pending

- target remains current;
- saved record retains its old `confirmed_profile`;
- operation is PENDING.

### Success commit

At one atomic success commit revision:

- target saved record's `confirmed_profile` becomes the requested profile;
- operation becomes terminal SUCCEEDED;
- result references that commit revision.

Only after this commit may UI project the requested profile as ACTIVE.

### Failure/cancellation

- confirmed profile remains unchanged;
- terminal status is FAILED or CANCELLED.

If the target ceases to be current before commit, the operation cannot silently retarget.
It terminates without applying to another Mouse.

## 10. Custom apply commit point

For `APPLY_CUSTOM`, success is one atomic semantic commit:

- `custom_confirmed` becomes the complete requested five-source mapping;
- target saved Mouse `confirmed_profile` becomes CUSTOM;
- operation becomes SUCCEEDED.

Failure/cancellation leaves both confirmed values unchanged.

A late Custom backend completion after cancellation cannot update either value.

## 11. Remove commit points

REMOVE has two cases.

### 11.1 Offline target

For an offline saved target:

- target remains saved while REMOVE is PENDING;
- success commit atomically removes the saved record;
- operation becomes SUCCEEDED in that commit revision.

### 11.2 Live target

For a live target, frozen UI 1.0 requires the live session to end **before** logical
saved-record removal.

Normative order:

~~~text
R0: target is saved and current; REMOVE PENDING
R1: authority-release commit
    current_mouse_id = null
    target still saved
    REMOVE still PENDING
    CONNECTION_CHANGED(... current=null, reason=REMOVE_RELEASE)

R2: removal commit
    target no longer in saved_mice
    REMOVE = SUCCEEDED
    ACTIVITY_RESULT(REMOVE SUCCEEDED)
~~~

Cancellation after R1 but before R2 leaves the record saved but offline. A late physical
removal completion after cancellation cannot delete the saved record.

No successful removal revision may keep the removed target as current.

## 12. Physical connection change semantics

Physical connection truth is independent of UI screen state.

When the current Mouse physically disconnects:

- Core commits `current_mouse_id = null` immediately as product truth;
- saved record and confirmed profile remain unchanged;
- `CONNECTION_CHANGED(... reason=PHYSICAL_DISCONNECT)` is produced;
- UI decides which screen to project from the new Snapshot.

A disconnect is not delayed merely because the UI is showing an ACTIVE profile screen,
Help, Lock, Saved Devices or Remove This.

### Pending operations on disconnect

- PROFILE_APPLY and CUSTOM_APPLY targeting the disconnected current Mouse can no longer
  commit as if it remained authoritative; they terminate FAILED, without changing confirmed state;
- REMOVE may continue because removing an offline saved Mouse is legal;
- PAIR_NEW/HANDOFF whose ordering requires the old authoritative Mouse cannot promote the
  candidate from a stale handoff; it terminates FAILED without authority promotion.

CANCELLED is not used for an environmental disconnect; it is reserved for accepted logical
cancellation.

UIC-04 defines public error categories for environment-caused failures.

## 13. Terminal-state retention

Search and operation terminal state may remain visible in Snapshot until superseded by
the next activity of the same slot or until a later contract-defined clearing transition.

No hidden timer-based eviction is required by UIC-03.

The logical notification result remains identified by `activity_id` even if a later
Snapshot has already moved on.

## 14. Race fixtures required by this gate

Language-neutral race fixtures live under:

`contracts/ui-core/fixtures/uic-03/`

They cover at minimum:

- cancel + late success;
- stale old operation after newer work;
- physical disconnect during active profile;
- Pair New handoff ordering;
- live-Mouse removal release-before-delete;
- profile apply commit;
- Custom apply commit.

## 15. Compatibility analysis

UIC-03 creates semantic ordering commitments but no threading or ABI commitment.

Compatible implementation changes include:

- replacing threads with an event loop or vice versa;
- changing callback/poll/queue transport;
- changing Bluetooth/USB libraries;
- changing internal task IDs.

Incompatible semantic changes include:

- reusing an `activity_id` within one Core lifetime;
- allowing cancelled work to mutate confirmed truth;
- using screen state as connection truth;
- confirming a profile before the success commit;
- deleting a live Mouse record before releasing current authority;
- exposing a Pair New revision with two authoritative Mice;
- correlating terminal results only by operation kind instead of activity identity.

## 16. Decisions resolved by UIC-03

UIC-03 resolves:

- **OD-006** — result delivery is an ordered logical notification stream independent of
  the concrete binding mechanism;
- **OD-007** — cancellation is logical invalidation plus best-effort physical cancellation;
- **OD-008** — late/stale backend completion cannot mutate confirmed product truth; result
  correlation uses activity identity and terminal transitions are unique;
- the activity side of **OD-003** — Core allocates non-reused `activity_id` values and
  every activity records `origin_intent_id`.

Still deferred:

- concrete identity widths/encodings;
- final public error taxonomy;
- capabilities and limits;
- exact timeout implementation/clock representation;
- C ABI, memory ownership and thread-safety.
