#!/usr/bin/env python3
from __future__ import annotations

import json
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent
errors: list[str] = []

def fail(message: str) -> None:
    errors.append(message)

def load_json(path: pathlib.Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path.name}: JSON parse failed: {exc}")
        return {}

schema = load_json(ROOT / "schema.json")
compat = load_json(ROOT / "compatibility.json")
coverage = load_json(ROOT / "coverage.json")
fixture_manifest = load_json(ROOT / "fixtures" / "manifest.json")
release_manifest = load_json(ROOT / "manifest.json")
header = (ROOT / "uic_v1.h").read_text(encoding="utf-8")
contract = (ROOT / "contract.md").read_text(encoding="utf-8")
equivalence = (ROOT / "schema-c-equivalence.md").read_text(encoding="utf-8")

if (ROOT / "VERSION").read_text(encoding="utf-8").strip() != "v1.0.0":
    fail("VERSION must be v1.0.0")

if schema.get("$id") != "urn:remappingbridge:ui-core:v1.0.0":
    fail("schema release id mismatch")
if compat.get("release") != "v1.0.0":
    fail("compatibility release mismatch")
if compat.get("contract") != {"major": 1, "minor": 0}:
    fail("compatibility contract major/minor mismatch")

required_header_tokens = [
    "#define MOUSE_UIC_V1_CONTRACT_MAJOR UINT32_C(1)",
    "#define MOUSE_UIC_V1_CONTRACT_MINOR UINT32_C(0)",
    "#define MOUSE_UIC_V1_MAX_SAVED_MICE UINT32_C(16)",
    "#define MOUSE_UIC_V1_MAX_MOUSE_NAME_UTF8_BYTES UINT32_C(63)",
    "typedef uint64_t mouse_uic_v1_mouse_id_t;",
    "typedef uint64_t mouse_uic_v1_intent_id_t;",
    "typedef uint64_t mouse_uic_v1_activity_id_t;",
    "typedef uint64_t mouse_uic_v1_notification_seq_t;",
    "mouse_uic_v1_get_descriptor(",
    "mouse_uic_v1_read_snapshot(",
    "mouse_uic_v1_submit_intent(",
    "mouse_uic_v1_poll_notification(",
]
for token in required_header_tokens:
    if token not in header:
        fail(f"header missing {token}")

for pattern in [r"\bmui_", r"\bbtstack", r"\btinyusb", r"\bhidpp", r"\bhci_", r"\bgatt_"]:
    if re.search(pattern, header, flags=re.IGNORECASE):
        fail(f"released C binding leaks private implementation token {pattern}")

for path in [ROOT / "contract.md", ROOT / "uic_v1.h", ROOT / "schema.json", ROOT / "schema-c-equivalence.md"]:
    text = path.read_text(encoding="utf-8")
    if re.search(r"\b(TODO|TBD|FIXME)\b|NOT RELEASED", text, flags=re.IGNORECASE):
        fail(f"{path.name}: unresolved normative marker")

defs = schema.get("$defs", {})
domains = {
    "intent": (
        set(defs.get("intent", {}).get("properties", {}).get("kind", {}).get("enum", [])),
        {"START_FIRST_SEARCH","START_SAVED_SEARCH","START_PAIR_NEW","CANCEL_ACTIVITY","APPLY_PROFILE","APPLY_CUSTOM","REMOVE_MOUSE"}
    ),
    "profile": (
        set(defs.get("profile", {}).get("enum", [])),
        {"PASSTHROUGH","STANDARD","ESCAPE","CUSTOM"}
    ),
    "error": (
        set(defs.get("errorCategory", {}).get("enum", [])),
        {"INVALID_REQUEST","CONFLICT","STALE_STATE","LIMIT_REACHED","UNSUPPORTED","INCOMPATIBLE_CONTRACT","AUTHORITY_LOST","TEMPORARY_UNAVAILABLE","PERSISTENCE_FAILURE","INTERNAL_FAILURE"}
    ),
    "capability": (
        set(defs.get("capability", {}).get("enum", [])),
        set(compat.get("required_capabilities", []))
    ),
}
for name,(actual,expected) in domains.items():
    if actual != expected:
        fail(f"{name} domain mismatch")

vectors = fixture_manifest.get("vectors", [])
actual_vectors = sorted(p.name for p in (ROOT / "fixtures").glob("*.json") if p.name != "manifest.json")
if sorted(vectors) != actual_vectors or len(actual_vectors) != 11:
    fail("fixture manifest must name exactly the 11 released vectors")

allowed_types={"descriptor","snapshot_read","intent","submission","notification","sequence"}
def walk(doc, where):
    typ=doc.get("object_type")
    if typ not in allowed_types:
        fail(f"{where}: bad object_type")
        return
    if typ=="sequence":
        records=doc.get("records",[])
        if [r.get("index") for r in records] != list(range(len(records))):
            fail(f"{where}: non-contiguous sequence")
        for r in records:
            walk(r.get("record",{}), f"{where}[{r.get('index')}]")
    elif typ=="snapshot_read":
        value=doc.get("value",{})
        if value.get("kind")=="SNAPSHOT":
            snap=value.get("snapshot",{})
            saved=snap.get("saved_mice",[])
            if len(saved)>16:
                fail(f"{where}: saved limit exceeded")
            ids=[m.get("mouse_id") for m in saved]
            if len(ids)!=len(set(ids)):
                fail(f"{where}: duplicate mouse ids")
            current=snap.get("current_mouse_id")
            if current is not None and current not in ids:
                fail(f"{where}: current mouse not saved")
            for mouse in saved:
                name=mouse.get("name","")
                if "\x00" in name or len(name.encode("utf-8"))>63:
                    fail(f"{where}: invalid mouse name")

for name in actual_vectors:
    walk(load_json(ROOT / "fixtures" / name), name)

rules=coverage.get("rules",[])
ids=[r.get("id") for r in rules]
if len(rules)!=88 or sorted(ids) != [f"BR-{i:03d}" for i in range(1,89)]:
    fail("coverage must contain BR-001..BR-088 exactly")
for row in rules:
    if row.get("disposition") not in {
        "EXPRESSED_IN_V1_RELEASE",
        "INTENTIONALLY_EXCLUDED_UI_LOCAL",
        "INTENTIONALLY_EXCLUDED_CORE_LOCAL",
    }:
        fail(f"{row.get('id')}: invalid release disposition")
    if not row.get("release_reference"):
        fail(f"{row.get('id')}: missing release reference")

# Content stability: every listed normative/support artifact must have the Git blob hash
# captured in manifest.json. manifest.json itself is intentionally excluded to avoid
# self-reference.
artifacts=release_manifest.get("artifacts",[])
if not artifacts:
    fail("release manifest has no artifacts")
for item in artifacts:
    rel=item.get("path")
    expected=item.get("git_blob_sha")
    path=ROOT / rel
    if not path.is_file():
        fail(f"manifest artifact missing: {rel}")
        continue
    try:
        actual=subprocess.check_output(["git","hash-object",str(path)],text=True).strip()
    except Exception as exc:
        fail(f"git hash-object failed for {rel}: {exc}")
        continue
    if actual != expected:
        fail(f"content drift: {rel}")

if release_manifest.get("release") != "v1.0.0":
    fail("release manifest version mismatch")
if release_manifest.get("consumer_proof_commit") != "7d1f0246ca4b70a2a7134de040b02db13e3bd540":
    fail("consumer proof pin mismatch")
if release_manifest.get("provider_proof_commit") != "f5b7384156a42543505b93dd7609e715c4cd3542":
    fail("provider proof pin mismatch")

if errors:
    print("UI-Core v1.0.0 release validation FAILED")
    for error in errors:
        print(" -", error)
    sys.exit(1)

print("UI-Core v1.0.0 release validation PASS")
print("vectors=11")
print("traceability_rules=88")
print(f"content_addressed_artifacts={len(artifacts)}")
print("consumer_proof=mouse-ui@7d1f0246")
print("provider_proof=mouse-core@f5b73841")
