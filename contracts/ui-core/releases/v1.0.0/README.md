# UI ↔ Core Contract v1.0.0

This directory is the normative release package for UI↔Core Contract **v1.0.0**.

## Normative artifacts

- `contract.md` — language-neutral semantics;
- `schema.json` — machine-readable conformance schema;
- `uic_v1.h` — in-process C binding;
- `schema-c-equivalence.md` — mapping between language-neutral and C forms;
- `fixtures/` — neutral conformance vectors;
- `coverage.json` — disposition of BR-001..BR-088;
- `compatibility.json` — component proof pins and compatibility declaration;
- `manifest.json` — content-addressed Git blob hashes;
- `validate_release.py` — self-contained release validator.

`binding-smoke.c` is a compile-only binding check shipped with the release.

## Proven component revisions

Consumer/UI adapter proof:

~~~text
remappingbridge/mouse-ui
7d1f0246ca4b70a2a7134de040b02db13e3bd540
~~~

Provider/Core conformance proof:

~~~text
remappingbridge/mouse-core
f5b7384156a42543505b93dd7609e715c4cd3542
~~~

These commits prove the boundary semantics. Component branches may contain later
non-semantic declaration/documentation changes while still declaring compatibility with
v1.0.0.

## Immutability

After UIC-08 acceptance and promotion, this directory is immutable.

Do not edit v1.0.0 in place. Clarifications or semantic/API changes are published as a
new patch/minor/major release according to `../../versioning.md`.

The Git blob hashes in `manifest.json` make content stability testable.
