# UIC-05 — Language-neutral schema ↔ C binding equivalence

Status: **CANDIDATE**.

Normative semantic source: `uic-05-v1-candidate.md`.

Machine-readable schema: `uic-05-v1.schema.json`.

Proposed C binding: `uic-05-c-binding.h`.

## Object mapping

| Language-neutral object | C representation | Notes |
|---|---|---|
| ContractDescriptor | `mouse_uic_v1_contract_descriptor_t` | major/minor, fixed limits, 64-bit capability mask |
| SnapshotReadResult | `mouse_uic_v1_snapshot_read_result_t` | discriminated by `kind`; SNAPSHOT vs UNAVAILABLE |
| Snapshot | `mouse_uic_v1_snapshot_t` | atomic copy, revisioned, max 16 saved Mice |
| SavedMouse | `mouse_uic_v1_saved_mouse_t` | stable 64-bit ID, bounded UTF-8 name, confirmed profile |
| Custom mapping | `mouse_uic_v1_custom_mapping_t` | five fixed source indexes |
| Search state | `mouse_uic_v1_search_state_t` | optional slot using `present` |
| Operation state | `mouse_uic_v1_operation_state_t` | optional slot using `present` |
| Error | `mouse_uic_v1_error_t` | stable category/retryability/visibility + opaque diagnostics |
| Intent | `mouse_uic_v1_intent_t` | tagged union; caller-owned 64-bit `intent_id` |
| Submission result | `mouse_uic_v1_submission_result_t` | ACCEPTED/REPLAY/REJECTED + optional activity/error |
| Notification | `mouse_uic_v1_notification_t` | tagged union + monotonic 64-bit sequence |
| Activity result | `mouse_uic_v1_activity_result_t` | search or operation class, one terminal result |
| Connection changed | `mouse_uic_v1_connection_changed_t` | optional previous/current IDs + commit revision |

## Scalar/domain mapping

All C tagged domains are `uint32_t`, not implementation-defined C enums.

All public identities/revisions/sequences are `uint64_t`.

Zero is invalid/none for IDs; schema uses positive integers and null for absence.

Language-neutral strings are represented as explicit length + fixed byte array. They are
not required to be NUL-terminated.

## Custom array mapping

Language-neutral keys map to C indexes exactly:

| Semantic source | C index |
|---|---:|
| LEFT | 0 |
| RIGHT | 1 |
| MIDDLE | 2 |
| FORWARD | 3 |
| BACKWARD | 4 |

Values use `MOUSE_UIC_V1_TARGET_*`.

## Optional/null mapping

Language-neutral null/absence maps to explicit presence flags in C.

Examples:

- `current_mouse_id = null` → `has_current_mouse = 0`;
- absent search → `search.present = 0`;
- absent candidate/error/requested value → corresponding `has_*` flag is 0;
- connection previous/current null → corresponding `has_*_current_mouse = 0`.

When a presence flag is zero, the associated storage is semantically ignored and MUST be
zero-initialized/reserved-safe by producers.

## Activity result class mapping

Language-neutral `activity_kind` is one domain combining search purposes and operations.

The C binding avoids a second public enum:

- search result: `search_purpose != INVALID` and `operation_kind == INVALID`;
- operation result: `operation_kind != INVALID` and `search_purpose == INVALID`.

Exactly one class tag is active.

For search SUCCEEDED, candidate is present.

For FAILED, error is present.

For CANCELLED, `cancel_intent_id` is present.

TIMED_OUT is legal only for search results.

## Submission mapping

START_FIRST_SEARCH, START_SAVED_SEARCH, START_PAIR_NEW, APPLY_PROFILE, APPLY_CUSTOM and
REMOVE_MOUSE:

- ACCEPTED/REPLAY → `has_activity_id = 1`;
- REJECTED → `has_error = 1`.

CANCEL_ACTIVITY:

- ACCEPTED/REPLAY → `has_activity_id = 0`, because it does not create a second activity;
- the target activity's terminal notification carries `cancel_intent_id`.

## Snapshot read mapping

`SNAPSHOT`:

- C kind = `MOUSE_UIC_V1_SNAPSHOT_READ_SNAPSHOT`;
- nested Snapshot is valid and has provider-set `struct_size`;
- Error storage is ignored/zero.

`UNAVAILABLE`:

- C kind = `MOUSE_UIC_V1_SNAPSHOT_READ_UNAVAILABLE`;
- Error is valid;
- nested Snapshot storage is ignored/zero.

## Error mapping

Schema categories, retryability and visibility have a one-to-one C macro value.

Diagnostic code/message are bounded binding conveniences, not semantic branching keys.

## Extension mapping

Top-level C call structs use `struct_size` and trailing reserved storage.

Language-neutral later-minor optional additions correspond to trailing C fields or
capability-gated consumption of reserved storage without changing v1.0 offsets.

No later compatible minor may change an existing field/tag meaning.

## Equivalence acceptance

The UIC-05 validation workflow checks:

- schema/C intent, profile, error and capability domains;
- required limits/version constants;
- public identifier technology-leakage guard;
- C11 compile;
- C++17 include;
- candidate JSON parsing;
- all 88 UIC-00 rules have an explicit candidate disposition.

Any future change to the schema or C binding MUST update both sides and the conformance
vectors in the same candidate change.
