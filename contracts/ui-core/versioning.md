# UI ↔ Core contract versioning

## Drafts

Drafts have no compatibility guarantee. They may be rewritten, split or discarded while
contract work is still exploratory.

Released components MUST depend on a release directory, not a draft.

## Released version

The first released boundary is:

~~~text
UI↔Core Contract v1.0.0
descriptor major = 1
descriptor minor = 0
path = contracts/ui-core/releases/v1.0.0/
~~~

## Semantic version policy

- patch — clarification or compatible correction that does not break the published API;
- minor — backward-compatible optional additions;
- major — incompatible semantic, structural or required-capability change.

The v1.0.0 directory itself is immutable. Even a clarification is published as a new
version rather than edited in place.

## Runtime compatibility

Compatibility requires:

- equal contract major;
- provider minor >= consumer minimum minor;
- all consumer-required semantic capabilities present;
- public major-1 limits compatible with the consumer.

For contract major 1 the public limits are fixed:

- maximum 16 saved Mouse records;
- maximum 63 UTF-8 bytes for a semantic Mouse name.

A compatible minor may add optional fields/capabilities that older consumers can ignore.
It MUST NOT remove required v1 capabilities, repurpose existing enum/error meanings, or
increase fixed major-1 capacities in a way older bounded consumers cannot represent.

## C binding extension policy

The v1 C binding uses:

- explicit fixed-width integers;
- bounded inline arrays;
- top-level `struct_size`;
- zeroed reserved fields.

Compatible minors may append top-level trailing fields or consume explicitly reserved
embedded storage without changing existing offsets/semantics. Incompatible representation
changes require a new major.

## Component declarations

Both `mouse-ui` and `mouse-core` carry a machine-readable `ui-core-contract.json`.

A consumer declaration records:

- release version;
- required major/minimum minor;
- required capabilities/limits;
- canonical release path/source.

A provider declaration records:

- release version;
- provided major/minor;
- provided capabilities/limits;
- canonical release path/source.

CI validates each declaration against the released header/package.

## Independent version spaces

`UI Layout 1.0` is a frontend product/layout version and is not the same version space
as `UI↔Core Contract v1.0.0`.

A future layout change does not automatically imply a contract change, and a contract
release does not automatically imply a layout change.
