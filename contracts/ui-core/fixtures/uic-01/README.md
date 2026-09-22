# UIC-01 Snapshot fixtures

These fixtures exercise the language-neutral Snapshot semantics defined in
`contracts/ui-core/drafts/uic-01-snapshot-model.md`.

JSON is only portable test notation. It is not a released wire format or C ABI.

## Required scenarios

| Fixture | Required behavior |
|---|---|
| `01-first-use.json` | no saved Mouse, no current Mouse, FIRST search running |
| `02-connected.json` | saved/current Mouse with confirmed profile |
| `03-offline.json` | saved Mouse exists, no current Mouse, SAVED reconnect search running |
| `04-pending-profile-apply.json` | confirmed profile remains old value while a new profile is pending |
| `05-dirty-custom-ui-local.json` | dirty Custom draft exists only in test UI context and does not leak into Snapshot |
| `06-removal-pending.json` | identity-based REMOVE pending while target is still saved/current |
| `07-handoff-pending.json` | old Mouse remains authoritative while unsaved Pair New candidate is handed off |

## Harness rules

A fixture validator should inspect only `snapshot` for contract fields.
`ui_local_context` is optional fixture metadata and must never be interpreted as Core product truth.

Expected generic assertions:

1. all JSON parses;
2. every Snapshot has a non-negative `revision`;
3. saved IDs are unique;
4. current is null or references one saved record;
5. Custom has exactly five sources and valid targets;
6. no Snapshot contains screen/navigation/pixel/SDL/Inspector fields;
7. pending apply keeps confirmed profile distinct from requested profile;
8. dirty Custom UI metadata does not create `custom_draft` or `custom_dirty` Snapshot fields;
9. pending handoff keeps the old current Mouse authoritative and does not add the candidate to saved records before success.
