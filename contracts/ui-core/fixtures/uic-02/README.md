# UIC-02 intent fixtures

These JSON files are language-neutral examples for the normative intent model in
`contracts/ui-core/drafts/uic-02-intent-model.md`.

They are test notation only, not a wire format or C ABI.

Each fixture contains:

- `snapshot_before` — the relevant UIC-01 product truth before submission;
- `submissions` — one or more UI→Core intents;
- `expected.disposition` — `ACCEPTED | REPLAY | REJECTED`;
- optional explanatory `expected.reason`.

The fixtures cover every normative intent kind plus stale-target rejection,
safe duplicate replay, intent-ID collision rejection and incomplete Custom rejection.
