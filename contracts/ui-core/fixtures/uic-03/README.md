# UIC-03 async race fixtures

These JSON fixtures are language-neutral sequence specifications for
`contracts/ui-core/drafts/uic-03-async-results.md`.

They are not a wire format, event-loop API or C ABI.

Each fixture records a sequence of semantic commits and/or backend arrivals.

Rules used by the fixtures:

- `intent_id` is caller-owned.
- `activity_id` and `notification_seq` are Core-owned.
- every activity has exactly one terminal state.
- every product commit has a Snapshot `revision`.
- terminal notifications carry the revision that semantically committed their result.
- backend completions after cancellation may be recorded as late arrivals, but they must
  not create another commit or terminal result.

Required race coverage:

1. cancel + late success;
2. stale old operation after newer work;
3. disconnect during active profile;
4. Pair New handoff ordering;
5. live-Mouse removal release-before-delete;
6. profile apply commit;
7. Custom apply commit;
8. correlated search timeout.
