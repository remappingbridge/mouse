# UI ↔ Core contract versioning

## Drafts

Drafts have no compatibility guarantee. They may be rewritten, split, or discarded while UX/Core exploration continues.

## Releases

Released contracts use explicit versions. A release directory is immutable after publication except for clearly non-semantic typo fixes that do not change interpretation.

Recommended scheme:

- `v1.0.0` — first integrated stable boundary;
- patch — clarification with no semantic/API change;
- minor — backward-compatible additions;
- major — incompatible semantic or structural changes.

Both `mouse-ui` and `mouse-core` should declare which released contract version they implement. The final `mouse` integration should pin compatible component revisions rather than track moving branches implicitly.

## Independent version spaces

`UI Layout 1.0` is a frontend product/layout version. It is **not** `UI↔Core Contract v1.0.0`.

The first contract release may eventually be named `v1.0.0`, but only after the UIC program validates both UI and Core sides. Until then, the semantic boundary remains a draft.

## No contract release yet

MBR-08/MUI material remains historical traceability. UI Layout 1.0 is the current accepted product input, but no released UI↔Core ABI/API exists yet.
