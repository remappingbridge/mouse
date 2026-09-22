# Accepted MUI-08 frontend baseline

Status: **TRACEABILITY INPUT — NOT A PUBLIC CONTRACT RELEASE**.

Accepted on: **2026-09-21**.

## Reference

~~~text
repository: remappingbridge/mouse-ui
program:    MUI-00 through MUI-08
status:     ACCEPTED
commit:     537b0f6fdd188b283cf10648b1cc6dbdacbfe20d
baseline:   baseline/mui-08-accepted
~~~

## What this establishes

The accepted frontend provides a tested reference for user-observable semantics such as:

- zero/one current Mouse and saved Mouse presentation;
- first-use, saved-reconnect, and Pair New flows;
- confirmed profile state versus pending/failed apply;
- Custom draft/applied distinction;
- saved-device removal outcomes;
- user-visible timeout, stale, late, success, and failure handling;
- Help and Lock presentation ownership;
- deterministic frontend navigation and state projection.

## What this does not establish

The following remain frontend-private unless deliberately promoted:

- exact C structs and enum layouts;
- mock-world implementation;
- navigation screen enum;
- scenario catalog and fault-lab controls;
- SDL desktop shell;
- Inspector and bug-evidence metadata;
- framebuffer/rendering internals;
- virtual clock implementation;
- callback/threading/ownership ABI.

## Contract-analysis rule

The first released UI↔Core contract should express only semantics that must cross the repository boundary. It should be possible to rewrite `mouse-ui` internals or `mouse-core` internals without changing the contract as long as those public semantics remain compatible.

## Recommended next phase

Review the accepted frontend together with Core requirements and promote a minimal semantic boundary, conceptually separating:

1. **Snapshot/state from Core to UI**;
2. **Intent/commands from UI to Core**;
3. **asynchronous result/event semantics**;
4. **identity/version/capability information needed for compatibility**.

No ABI/version is released by this traceability record.

## Superseded by product freeze

MUI-08 remains reconstruction traceability. Contract work after 2026-09-22 uses the later frozen UI Layout 1.0 reference in `ui-layout-v1.0.md`.
