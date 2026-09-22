# UIC-01 — Snapshot Model

Status: **ACCEPTED UIC-01 BASELINE**.

Source baseline:

~~~text
UIC-00 accepted
UIC-01 human review accepted 2026-09-22
accepted integration baseline before UIC-02: b324fc7733036958005a29b23686c304ea182a07
mouse-ui UI Layout 1.0: e8adad7919e931c92515bf655ef4050876a8e7a9
~~~

This document defines the normative **Core → UI product snapshot semantics** for UIC-01. It is language-neutral and does not freeze a C ABI, callback API, event transport, queue, thread model or memory layout.

## 1. Snapshot purpose

A Snapshot is a complete atomic point-in-time view of the Core-owned product truth required by the frozen UI Layout 1.0.

The UI may derive screens, menu selection, Help, Lock and presentation from the Snapshot, but none of those UI-private concepts belong inside it.

Conceptually:

~~~text
Snapshot
  revision
  saved_mice
  current_mouse_id
  custom_confirmed
  search
  operation
~~~

## 2. Normative semantic shape

### 2.1 revision

`revision` is a non-negative monotonically increasing logical revision within one Core producer lifetime.

Rules:

1. whenever any normative Snapshot value changes, a later published Snapshot has a greater revision;
2. the producer may skip revision numbers;
3. re-delivery of an unchanged Snapshot may reuse the same revision;
4. two Snapshots from the same producer lifetime with the same revision are semantically identical;
5. revisions are not compared across producer restarts/lifetimes; the first Snapshot of a new producer lifetime becomes a new baseline;
6. exact integer width and C representation are deferred to UIC-05.

### 2.2 saved_mice

`saved_mice` is a semantic set of zero or more saved Mouse records.

Each record contains:

- `mouse_id` — stable product identity;
- `name` — valid UTF-8 semantic device name, 0..63 encoded bytes, before UI-local formatting;
- `confirmed_profile` — exactly one of `PASSTHROUGH | STANDARD | ESCAPE | CUSTOM`.

The order used by a serialization or fixture is not normative. Saved Devices ordering is UI-local.

A stable `mouse_id` does not change merely because presentation order changes.

### 2.3 current_mouse_id

`current_mouse_id` is either null or one `mouse_id`.

Normative invariant:

- when non-null, it identifies the **single authoritative live Mouse**;
- it must identify a record currently present in `saved_mice`;
- there is no per-record `connected` flag in the shared model, preventing contradictory multiple-current representations.

This field alone expresses zero-or-one current authority.

### 2.4 custom_confirmed

`custom_confirmed` is the confirmed global Custom mapping visible to product semantics.

It maps exactly these five physical sources:

- `LEFT`
- `RIGHT`
- `MIDDLE`
- `FORWARD`
- `BACKWARD`

Each maps to exactly one target:

`LEFT | RIGHT | MIDDLE | ESCAPE | FORWARD | BACKWARD`.

The UI-local editable Custom draft and its dirty flag are **not** Snapshot fields.

Only a confirmed/persisted semantic Custom mapping belongs here.

### 2.5 search

`search` is either null or one observable search activity.

When present it contains:

- `activity_id` — Core-owned unique non-reused identity for this search activity;
- `origin_intent_id` — caller-owned intent identity that created this activity;
- `purpose` — `FIRST | SAVED | PAIR_NEW`;
- `status` — `RUNNING | FOUND | TIMED_OUT | FAILED | CANCELLED`;
- optional `candidate` when a candidate is semantically known;
- `error` is present exactly when `status = FAILED`, using the UIC-04 stable public error object.

Candidate shape:

- `mouse_id`
- `name`
- `already_saved` boolean

Normative rules:

- at most one search activity is represented at a time;
- `FOUND` requires a candidate;
- non-`FOUND` states do not require a candidate;
- Core owns the semantic timeout windows: 8 s FIRST/SAVED and 15 s PAIR_NEW from accepted search activity start; UI may display progress but does not decide terminal timeout;
- screen IDs, retry screen identity and navigation destination are not Snapshot data.

### 2.6 operation

`operation` is either null or one observable product operation.

When present it contains:

- `activity_id` — Core-owned unique non-reused identity for this operation activity;
- `origin_intent_id` — caller-owned intent identity that created this activity;
- `kind` — `PROFILE_APPLY | CUSTOM_APPLY | REMOVE | HANDOFF`;
- `status` — `PENDING | SUCCEEDED | FAILED | CANCELLED`;
- `target_mouse_id` — semantic operation subject;
- optional `requested_profile`;
- optional `requested_custom` mapping;
- `error` is present exactly when `status = FAILED`, using the UIC-04 stable public error object.

Rules by kind:

- `PROFILE_APPLY`: `requested_profile` is required and is not `CUSTOM`;
- `CUSTOM_APPLY`: `requested_profile = CUSTOM` and `requested_custom` is required;
- `REMOVE`: requested profile/custom data are absent;
- `HANDOFF`: target may identify an unsaved Pair New candidate while the old current Mouse remains authoritative until handoff success.

UIC-03 defines each activity as having exactly one terminal transition. Terminal operation state may remain visible until superseded by later same-slot work; no hidden timer-based eviction is required.

## 3. Atomic consistency

A published Snapshot is one atomic semantic value.

The UI must never observe combinations assembled from different revisions, for example:

- a new `current_mouse_id` with an old `saved_mice` set that does not contain it;
- an updated `confirmed_profile` while an old pending operation is accidentally interpreted as the cause;
- a removed saved record while the same revision still declares it current.

A producer may build the Snapshot internally however it wants, but publication is semantically atomic.

## 4. Confirmed versus requested state

Confirmed state and requested/pending state are deliberately separate.

Example:

~~~text
saved_mice["mouse-1"].confirmed_profile = PASSTHROUGH
operation.kind = PROFILE_APPLY
operation.status = PENDING
operation.requested_profile = STANDARD
~~~

This means **Passthrough is still confirmed** while Standard is only requested.

The UI must not render Standard as ACTIVE until a later Snapshot reflects successful completion and the confirmed profile is updated.

Likewise, `custom_confirmed` does not change merely because a local draft is dirty or because a Custom apply is pending.

## 5. Handoff semantics in Snapshot

During Pair New qualification/handoff:

- the old current Mouse may remain `current_mouse_id`;
- the new candidate may appear in `search.candidate` and/or as `operation.target_mouse_id`;
- the candidate is not required to be in `saved_mice` before successful handoff;
- a successful handoff revision atomically updates saved membership/current authority as required by product semantics.

No Snapshot may claim two current authoritative Mice.

## 6. Removal semantics in Snapshot

While REMOVE is pending:

- target remains present in `saved_mice`;
- if it is current, it may remain `current_mouse_id` until success.

After successful removal of the live target, a later atomic Snapshot must not keep that identity as current and saved simultaneously.

The exact credential deletion/storage mechanism is Core-local.

## 7. Snapshot lifetime semantics

Snapshot semantics are value semantics:

- once a revision has been observed, its semantic contents do not mutate retroactively;
- a later change is represented by a later revision;
- consumers may cache older Snapshots for comparison/testing;
- pointer ownership, allocation, copying and buffer lifetime are language-binding concerns deferred to UIC-05;
- UIC-03 adds a transport-independent ordered logical notification stream correlated by activity and Snapshot revision; callback/poll/queue mechanics remain binding-specific.

## 8. Fields explicitly forbidden from Snapshot

The Snapshot must not contain:

- screen ID or screen family;
- selection, page index or menu cursor;
- Help owner/state;
- Lock state or backlight;
- KEY/joystick press state;
- pixel coordinates, RGB565, colors, fonts or text bounds;
- SDL/window/zoom state;
- Inspector IDs;
- scenario/fault-injection controls;
- private `mui_*` types;
- BTstack/HCI/GATT/HOGP/HID++ objects;
- TinyUSB report structs;
- flash addresses/sectors;
- GPIO/SPI/ST7789 state.

## 9. Language-neutral fixture representation

UIC-01 fixtures use JSON only as a portable test notation.

The JSON shape is **not** the final wire format or C ABI.

Each fixture is a wrapper:

~~~text
{
  "fixture_id": "...",
  "ui_local_context": { ... optional test-only data ... },
  "snapshot": { ... normative Snapshot semantics ... }
}
~~~

Only the object under `snapshot` represents shared Snapshot semantics.

`ui_local_context` may be used only to prove that UI-private state such as a dirty Custom draft does not leak into Snapshot.

## 10. Invariants

A valid UIC-01 Snapshot satisfies all of the following:

1. saved Mouse IDs are unique;
2. `current_mouse_id` is null or identifies exactly one saved record;
3. every saved record has exactly one confirmed profile;
4. `confirmed_profile` never means requested/pending profile;
5. Custom has exactly five source mappings;
6. each Custom target belongs to the six-target domain;
7. every semantic Mouse/candidate name is valid UTF-8 and at most 63 encoded bytes;
8. at most one search is represented;
9. at most one operation is represented;
10. `FOUND` search contains a candidate;
11. FAILED search/operation contains exactly one stable public error object;
12. non-FAILED terminal state does not require/synthesize an error object;
13. pending profile/custom apply keeps confirmed state unchanged until success;
14. UI-local draft/dirty/navigation/pixel state is absent;
15. Snapshot publication is atomic per revision.

## 11. Traceability to accepted UIC-00 semantics

| Snapshot semantic | UIC-00 rules |
|---|---|
| saved Mouse identity/name/confirmed profile | BR-004, BR-011, BR-012, BR-013, BR-065, BR-066 |
| zero/one current authority | BR-009, BR-032, BR-056, BR-068 |
| confirmed global Custom mapping | BR-015, BR-053, BR-060, BR-061 |
| FIRST/SAVED/PAIR_NEW search state | BR-020, BR-021, BR-026, BR-027, BR-031, BR-033, BR-035 |
| profile/Custom operation state | BR-050, BR-051, BR-052, BR-054, BR-061 |
| REMOVE state and target identity | BR-066, BR-067, BR-068, BR-071 |
| HANDOFF state | BR-032, BR-034, BR-035 |
| revision/atomicity | derived from BR-016, BR-030, BR-054, BR-060 and the requirement that screen state never be product truth |

## 12. Compatibility analysis

UIC-01 freezes semantic meaning only inside the working contract program; it still creates no released ABI.

Compatibility conclusions for later gates:

- changing serialization array order for `saved_mice` is non-semantic because the collection is a set;
- changing a private Core identity representation is allowed if stable `mouse_id` semantics are preserved at the boundary;
- adding UI-private state to Snapshot is not compatible because it violates the ownership split;
- collapsing confirmed profile into requested/pending profile is not compatible;
- allowing more than one current authority is not compatible;
- changing profile, Custom source or Custom target value domains requires explicit contract review;
- exact integer widths, string encodings, memory layout and transport framing remain unfrozen until later gates;
- a Core implementation may be completely rewritten without contract impact if it produces semantically equivalent atomic Snapshots.

## 13. Decisions resolved by UIC-01

UIC-01 resolves:

- **OD-001** — the language-neutral Snapshot semantic schema is the model in this document;
- **OD-002** — Snapshot publication is atomic and revisioned with immutable value semantics within a producer lifetime.

Still deferred:

- concrete request/activity ID representation widths/encodings;
- binding-specific notification transport;
- error taxonomy;
- capabilities/limits encoding;
- C ABI and memory ownership.
