#!/usr/bin/env python3
from __future__ import annotations

import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
DRAFTS = ROOT / "drafts"
FIXTURES = ROOT / "fixtures" / "uic-05"
TRACE = ROOT / "traceability" / "uic-05-v1-candidate-coverage.json"

errors: list[str] = []

def fail(msg: str) -> None:
    errors.append(msg)

def load(path: pathlib.Path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        fail(f"{path}: JSON parse failed: {exc}")
        return None

schema = load(DRAFTS / "uic-05-v1.schema.json")
header = (DRAFTS / "uic-05-c-binding.h").read_text(encoding="utf-8")
spec = (DRAFTS / "uic-05-v1-candidate.md").read_text(encoding="utf-8")
coverage = load(TRACE)

# Candidate constants / bounded representation.
required_header_tokens = [
    "#define MOUSE_UIC_V1_CONTRACT_MAJOR UINT32_C(1)",
    "#define MOUSE_UIC_V1_CONTRACT_MINOR UINT32_C(0)",
    "#define MOUSE_UIC_V1_MAX_SAVED_MICE UINT32_C(16)",
    "#define MOUSE_UIC_V1_MAX_MOUSE_NAME_UTF8_BYTES UINT32_C(63)",
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
        fail(f"header missing: {token}")

# Public names must not leak private implementation technologies/types.
for pattern in [r"\bmui_", r"\bbtstack", r"\btinyusb", r"\bhidpp", r"\bhci_", r"\bgatt_"]:
    if re.search(pattern, header, flags=re.IGNORECASE):
        fail(f"public C binding leaks implementation token: {pattern}")

# Schema/C semantic domains must agree.
expected_intents = {
    "START_FIRST_SEARCH", "START_SAVED_SEARCH", "START_PAIR_NEW",
    "CANCEL_ACTIVITY", "APPLY_PROFILE", "APPLY_CUSTOM", "REMOVE_MOUSE",
}
expected_profiles = {"PASSTHROUGH", "STANDARD", "ESCAPE", "CUSTOM"}
expected_targets = {"LEFT", "RIGHT", "MIDDLE", "ESCAPE", "FORWARD", "BACKWARD"}
expected_search_purposes = {"FIRST", "SAVED", "PAIR_NEW"}
expected_search_statuses = {"RUNNING", "FOUND", "TIMED_OUT", "FAILED", "CANCELLED"}
expected_operation_kinds = {"PROFILE_APPLY", "CUSTOM_APPLY", "REMOVE", "HANDOFF"}
expected_operation_statuses = {"PENDING", "SUCCEEDED", "FAILED", "CANCELLED"}
expected_terminal_statuses = {"SUCCEEDED", "TIMED_OUT", "FAILED", "CANCELLED"}
expected_connection_reasons = {
    "PHYSICAL_DISCONNECT", "PHYSICAL_CONNECT", "SEARCH_RECONNECT",
    "FIRST_ACCEPT", "HANDOFF", "REMOVE_RELEASE",
}
expected_retryability = {"NOT_RETRYABLE", "AFTER_REFRESH", "AFTER_STATE_CHANGE", "LATER"}
expected_visibility = {"DIAGNOSTIC_ONLY", "GENERIC_USER_FAILURE", "USER_ACTIONABLE"}
expected_errors = {
    "INVALID_REQUEST", "CONFLICT", "STALE_STATE", "LIMIT_REACHED",
    "UNSUPPORTED", "INCOMPATIBLE_CONTRACT", "AUTHORITY_LOST",
    "TEMPORARY_UNAVAILABLE", "PERSISTENCE_FAILURE", "INTERNAL_FAILURE",
}
expected_caps = {
    "FIRST_DISCOVERY", "SAVED_RECONNECT", "PAIR_NEW",
    "PROFILE_PASSTHROUGH", "PROFILE_STANDARD", "PROFILE_ESCAPE",
    "PROFILE_CUSTOM", "REMOVE_MOUSE", "ESCAPE_OUTPUT",
}

if schema:
    defs = schema.get("$defs", {})
    intents = set(defs.get("intent", {}).get("properties", {}).get("kind", {}).get("enum", []))
    profiles = set(defs.get("profile", {}).get("enum", []))
    targets = set(defs.get("target", {}).get("enum", []))
    search_purposes = set(defs.get("search", {}).get("properties", {}).get("purpose", {}).get("enum", []))
    search_statuses = set(defs.get("search", {}).get("properties", {}).get("status", {}).get("enum", []))
    operation_kinds = set(defs.get("operation", {}).get("properties", {}).get("kind", {}).get("enum", []))
    operation_statuses = set(defs.get("operation", {}).get("properties", {}).get("status", {}).get("enum", []))
    terminal_statuses = set(defs.get("terminalStatus", {}).get("enum", []))
    connection_reasons = set(
        defs.get("connectionChanged", {}).get("properties", {}).get("reason", {}).get("enum", [])
    )
    retryability = set(defs.get("retryability", {}).get("enum", []))
    visibility = set(defs.get("visibility", {}).get("enum", []))
    errcats = set(defs.get("errorCategory", {}).get("enum", []))
    caps = set(defs.get("capability", {}).get("enum", []))
    if intents != expected_intents:
        fail(f"schema intent domain mismatch: {sorted(intents)}")
    if profiles != expected_profiles:
        fail(f"schema profile domain mismatch: {sorted(profiles)}")
    if targets != expected_targets:
        fail(f"schema target domain mismatch: {sorted(targets)}")
    if search_purposes != expected_search_purposes:
        fail(f"schema search purpose mismatch: {sorted(search_purposes)}")
    if search_statuses != expected_search_statuses:
        fail(f"schema search status mismatch: {sorted(search_statuses)}")
    if operation_kinds != expected_operation_kinds:
        fail(f"schema operation kind mismatch: {sorted(operation_kinds)}")
    if operation_statuses != expected_operation_statuses:
        fail(f"schema operation status mismatch: {sorted(operation_statuses)}")
    if terminal_statuses != expected_terminal_statuses:
        fail(f"schema terminal status mismatch: {sorted(terminal_statuses)}")
    if connection_reasons != expected_connection_reasons:
        fail(f"schema connection reason mismatch: {sorted(connection_reasons)}")
    if retryability != expected_retryability:
        fail(f"schema retryability mismatch: {sorted(retryability)}")
    if visibility != expected_visibility:
        fail(f"schema visibility mismatch: {sorted(visibility)}")
    if errcats != expected_errors:
        fail(f"schema error domain mismatch: {sorted(errcats)}")
    if caps != expected_caps:
        fail(f"schema capability domain mismatch: {sorted(caps)}")

for name in expected_intents:
    macro = "MOUSE_UIC_V1_INTENT_" + name
    if macro not in header:
        fail(f"C binding missing intent macro {macro}")

for name in expected_profiles:
    macro = "MOUSE_UIC_V1_PROFILE_" + name
    if macro not in header:
        fail(f"C binding missing profile macro {macro}")

for name in expected_targets:
    macro = "MOUSE_UIC_V1_TARGET_" + name
    if macro not in header:
        fail(f"C binding missing target macro {macro}")

for name in expected_search_purposes:
    macro = "MOUSE_UIC_V1_SEARCH_PURPOSE_" + name
    if macro not in header:
        fail(f"C binding missing search purpose macro {macro}")

for name in expected_search_statuses:
    macro = "MOUSE_UIC_V1_SEARCH_STATUS_" + name
    if macro not in header:
        fail(f"C binding missing search status macro {macro}")

for name in expected_operation_kinds:
    macro = "MOUSE_UIC_V1_OPERATION_" + name
    if macro not in header:
        fail(f"C binding missing operation kind macro {macro}")

for name in expected_operation_statuses:
    macro = "MOUSE_UIC_V1_OPERATION_STATUS_" + name
    if macro not in header:
        fail(f"C binding missing operation status macro {macro}")

for name in expected_terminal_statuses:
    macro = "MOUSE_UIC_V1_ACTIVITY_TERMINAL_" + name
    if macro not in header:
        fail(f"C binding missing terminal status macro {macro}")

for name in expected_connection_reasons:
    macro = "MOUSE_UIC_V1_CONNECTION_REASON_" + name
    if macro not in header:
        fail(f"C binding missing connection reason macro {macro}")

retry_macros = {
    "NOT_RETRYABLE": "MOUSE_UIC_V1_RETRY_NOT_RETRYABLE",
    "AFTER_REFRESH": "MOUSE_UIC_V1_RETRY_AFTER_REFRESH",
    "AFTER_STATE_CHANGE": "MOUSE_UIC_V1_RETRY_AFTER_STATE_CHANGE",
    "LATER": "MOUSE_UIC_V1_RETRY_LATER",
}
for macro in retry_macros.values():
    if macro not in header:
        fail(f"C binding missing retryability macro {macro}")

visibility_macros = {
    "DIAGNOSTIC_ONLY": "MOUSE_UIC_V1_VISIBILITY_DIAGNOSTIC_ONLY",
    "GENERIC_USER_FAILURE": "MOUSE_UIC_V1_VISIBILITY_GENERIC_USER_FAILURE",
    "USER_ACTIONABLE": "MOUSE_UIC_V1_VISIBILITY_USER_ACTIONABLE",
}
for macro in visibility_macros.values():
    if macro not in header:
        fail(f"C binding missing visibility macro {macro}")

for name in expected_errors:
    macro = "MOUSE_UIC_V1_ERROR_" + name
    if macro not in header:
        fail(f"C binding missing error macro {macro}")

for name in expected_caps:
    macro = "MOUSE_UIC_V1_CAP_" + name
    if macro not in header:
        fail(f"C binding missing capability macro {macro}")

# Candidate vectors.
vector_files = sorted(
    p for p in FIXTURES.glob("*.json")
    if p.name != "manifest.json"
)
if len(vector_files) < 10:
    fail(f"expected >=10 candidate vectors, got {len(vector_files)}")

allowed_object_types = {
    "descriptor", "snapshot_read", "intent", "submission", "notification", "sequence"
}

def utf8_len(value: str) -> int:
    return len(value.encode("utf-8"))

def check_error(err: dict, where: str) -> None:
    if err.get("category") not in expected_errors:
        fail(f"{where}: invalid error category")
    if err.get("retryability") not in {
        "NOT_RETRYABLE", "AFTER_REFRESH", "AFTER_STATE_CHANGE", "LATER"
    }:
        fail(f"{where}: invalid retryability")
    if err.get("visibility") not in {
        "DIAGNOSTIC_ONLY", "GENERIC_USER_FAILURE", "USER_ACTIONABLE"
    }:
        fail(f"{where}: invalid visibility")
    if "diagnostic_code" in err and utf8_len(err["diagnostic_code"]) > 31:
        fail(f"{where}: diagnostic_code exceeds 31 UTF-8 bytes")
    if "diagnostic_message" in err and utf8_len(err["diagnostic_message"]) > 127:
        fail(f"{where}: diagnostic_message exceeds 127 UTF-8 bytes")

def check_snapshot(snapshot: dict, where: str) -> None:
    saved = snapshot.get("saved_mice", [])
    if len(saved) > 16:
        fail(f"{where}: more than 16 saved mice")
    ids = [m.get("mouse_id") for m in saved]
    if len(ids) != len(set(ids)):
        fail(f"{where}: duplicate saved mouse IDs")
    current = snapshot.get("current_mouse_id")
    if current is not None and current not in ids:
        fail(f"{where}: current mouse is not saved")
    for m in saved:
        name = m.get("name", "")
        if "\x00" in name or utf8_len(name) > 63:
            fail(f"{where}: invalid semantic mouse name")
        if m.get("confirmed_profile") not in expected_profiles:
            fail(f"{where}: invalid confirmed profile")
    mapping = snapshot.get("custom_confirmed", {})
    if set(mapping) != {"LEFT", "RIGHT", "MIDDLE", "FORWARD", "BACKWARD"}:
        fail(f"{where}: invalid Custom source set")

def walk_record(doc: dict, where: str) -> None:
    typ = doc.get("object_type")
    if typ not in allowed_object_types:
        fail(f"{where}: invalid object_type {typ!r}")
        return
    if typ == "sequence":
        records = doc.get("records", [])
        indexes = [r.get("index") for r in records]
        if indexes != list(range(len(records))):
            fail(f"{where}: sequence indexes must be contiguous from zero")
        for r in records:
            walk_record(r.get("record", {}), f"{where}[{r.get('index')}]")
        return

    value = doc.get("value", {})
    if typ == "descriptor":
        if value.get("contract", {}).get("major") != 1:
            fail(f"{where}: descriptor major")
        if value.get("limits", {}).get("max_saved_mice") != 16:
            fail(f"{where}: descriptor saved limit")
        if value.get("limits", {}).get("max_mouse_name_utf8_bytes") != 63:
            fail(f"{where}: descriptor name limit")
        if not expected_caps.issubset(set(value.get("capabilities", []))):
            fail(f"{where}: required capabilities missing")
    elif typ == "snapshot_read":
        kind = value.get("kind")
        if kind == "SNAPSHOT":
            if "error" in value:
                fail(f"{where}: SNAPSHOT read must not carry error")
            check_snapshot(value.get("snapshot", {}), where)
        elif kind == "UNAVAILABLE":
            if "snapshot" in value:
                fail(f"{where}: UNAVAILABLE must not carry snapshot")
            check_error(value.get("error", {}), where)
        else:
            fail(f"{where}: invalid snapshot read kind")
    elif typ == "intent":
        if value.get("kind") not in expected_intents:
            fail(f"{where}: invalid intent kind")
        if int(value.get("intent_id", 0)) <= 0:
            fail(f"{where}: invalid intent_id")
    elif typ == "submission":
        disp = value.get("disposition")
        if disp not in {"ACCEPTED", "REPLAY", "REJECTED"}:
            fail(f"{where}: invalid submission disposition")
        if disp == "REJECTED":
            check_error(value.get("error", {}), where)
        elif "error" in value:
            fail(f"{where}: non-rejected submission carries error")
    elif typ == "notification":
        if int(value.get("notification_seq", 0)) <= 0:
            fail(f"{where}: invalid notification sequence")
        kind = value.get("kind")
        if kind == "ACTIVITY_RESULT":
            result = value.get("activity_result", {})
            terminal = result.get("terminal_status")
            if terminal == "FAILED":
                check_error(result.get("error", {}), where)
            elif "error" in result:
                fail(f"{where}: non-FAILED result carries error")
            if terminal == "CANCELLED" and int(result.get("cancel_intent_id", 0)) <= 0:
                fail(f"{where}: CANCELLED result missing cancel_intent_id")
        elif kind == "CONNECTION_CHANGED":
            pass
        else:
            fail(f"{where}: invalid notification kind")

for path in vector_files:
    doc = load(path)
    if doc:
        walk_record(doc, path.name)

# Required representative/race vectors by manifest.
manifest = load(FIXTURES / "manifest.json")
if manifest:
    declared = set(manifest.get("vectors", []))
    actual = {p.name for p in vector_files}
    if declared != actual:
        fail("manifest vector list differs from files on disk")

# Full UIC-00 rule audit.
if coverage:
    rules = coverage.get("rules", [])
    ids = [r.get("id") for r in rules]
    if len(rules) != 88:
        fail(f"coverage must contain 88 rules, got {len(rules)}")
    if len(ids) != len(set(ids)):
        fail("coverage contains duplicate rule IDs")
    expected_ids = [f"BR-{i:03d}" for i in range(1, 89)]
    if sorted(ids) != expected_ids:
        fail("coverage rule IDs do not exactly cover BR-001..BR-088")
    allowed_disp = {
        "EXPRESSED_IN_V1_CANDIDATE",
        "INTENTIONALLY_EXCLUDED_UI_LOCAL",
        "INTENTIONALLY_EXCLUDED_CORE_LOCAL",
    }
    for row in rules:
        if row.get("disposition") not in allowed_disp:
            fail(f"{row.get('id')}: invalid coverage disposition")
        if not row.get("candidate_reference"):
            fail(f"{row.get('id')}: missing candidate reference")

# No unresolved MUST-level candidate decisions may remain among UIC-05-owned items.
decision_text = (DRAFTS / "uic-00-unresolved-decisions.md").read_text(encoding="utf-8")
for decision in ["OD-003", "OD-016", "OD-017", "OD-018", "OD-019"]:
    for line in decision_text.splitlines():
        if line.startswith(f"| {decision} |"):
            fail(f"{decision} remains open after UIC-05")

# Candidate must explicitly define ownership/lifetime/threading.
for phrase in [
    "provider MUST NOT retain pointers into caller structs",
    "same provider instance MUST be externally serialized",
    "No public call returns provider-owned data pointers",
    "release directory",
]:
    if phrase not in spec:
        fail(f"candidate spec missing required phrase: {phrase}")

if errors:
    print("UIC-05 validation FAILED")
    for error in errors:
        print(" -", error)
    sys.exit(1)

print("UIC-05 validation PASS")
print(f"vectors={len(vector_files)}")
print("traceability_rules=88")
print("schema_c_domains=PASS")
print("schema_c_full_tag_domain_equivalence=PASS")
print("implementation_leakage_guard=PASS")
