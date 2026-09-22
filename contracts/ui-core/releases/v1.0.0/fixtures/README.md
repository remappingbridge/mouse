# UI↔Core v1.0.0 conformance vectors

These vectors are implementation-independent examples of the released v1.0.0 contract.

They validate against `../schema.json`.

JSON remains conformance notation, not a runtime wire format.

Coverage includes descriptor/versioning, Snapshot, all major command/result shapes,
rejection, cancellation/late protection, Pair New handoff ordering, live removal ordering,
Custom atomic commit, timeout and initialization/persistence failure.
