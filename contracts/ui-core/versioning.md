# UI ↔ Core contract versioning

## Drafts

Drafts have no compatibility guarantee. They may be rewritten, split, or discarded while UX/Core exploration continues.

## Releases

Released contracts use explicit versions. A release directory is immutable after publication except for clearly non-semantic typo fixes that do not change interpretation.

Recommended scheme:

- `v1.0.0` — first integrated stable boundary;
- patch — clarification with no semantic/API change;
- minor — backward-compatible optional additions;
- major — incompatible semantic or structural changes.

## UIC-04 semantic discovery rules

Before a concrete release binding exists, the candidate semantic descriptor uses
`major = 1, minor = 0`.

Compatibility requires:

- equal major;
- provider minor >= consumer minimum minor;
- all consumer-required semantic capability tokens present;
- public major-1 limits compatible with the consumer.

For the frozen UI Layout 1.0 / contract-major-1 line, the public limits are fixed:

- maximum 16 saved Mouse records;
- maximum 63 UTF-8 bytes for a semantic Mouse name.

A minor release may add optional fields/capabilities that older consumers can ignore, but
must not increase these fixed capacities, remove required v1 capabilities, or repurpose
existing enum/error meanings. Such changes require a new major.

Concrete C constants, ABI representation and the final released version number remain
UIC-05/UIC-08 work.

Both `mouse-ui` and `mouse-core` should declare which released contract version they implement. The final `mouse` integration should pin compatible component revisions rather than track moving branches implicitly.

## Independent version spaces

`UI Layout 1.0` is a frontend product/layout version. It is **not** `UI↔Core Contract v1.0.0`.

The first contract release may eventually be named `v1.0.0`, but only after the UIC program validates both UI and Core sides. Until then, the semantic boundary remains a draft.

## No contract release yet

MBR-08/MUI material remains historical traceability. UI Layout 1.0 is the current accepted product input, but no released UI↔Core ABI/API exists yet.
