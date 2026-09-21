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

## No release yet

MBR-08 migration material remains under `drafts/` and `traceability/`. It is not `v1`.
