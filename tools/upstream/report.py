"""Writes `upstream-report.md` / `upstream-report.json` and computes the exit code.

The exit code is 1 whenever something needs a human: unresolved merge conflicts, signature or enum value
conflicts without a decision, upstream removals still used by SylvaGX code, apply errors or a failing build.
"""

import datetime
import json
from pathlib import Path

REPORT_MD = "upstream-report.md"
REPORT_JSON = "upstream-report.json"


def collect(ctx) -> dict:
    """Turns the results of one run into a JSON-serialisable report.

    `ctx` keys (all optional): command, range (from, to), branch, merge, sdks, conflicts (apply.ConflictResult),
    check (compare.CheckResult), apply (apply.ApplyResult), build (build.BuildResult), removals, additions, audit.
    """
    report = {
        "command": ctx.get("command"),
        "date": datetime.datetime.now().isoformat(timespec="seconds"),
        "range": list(ctx.get("range") or (None, None)),
        "branch": ctx.get("branch"),
        "merge": ctx.get("merge"),
        "sdks": ctx.get("sdks", {}),
        "pending": [],
        "notes": list(ctx.get("notes", [])),
    }
    pending = report["pending"]

    conflicts = ctx.get("conflicts")
    if conflicts is not None:
        report["conflicts"] = {
            "resolved": conflicts.resolved, "saved": conflicts.saved, "reinjected": conflicts.reinjected,
            "pending": conflicts.pending,
            "errors": conflicts.apply.errors if conflicts.apply else [],
        }
        for p in conflicts.pending:
            pending.append({"kind": "merge_conflict", "path": p["path"], "detail": p["reason"]})
        for e in report["conflicts"]["errors"]:
            pending.append({"kind": "conflict_reinject_error", "detail": e})

    check = ctx.get("check")
    if check is not None:
        overlaps = []
        for o in check.overlaps:
            overlaps.append({
                "objc_class": o.objc_class, "cpp_class": o.apple.qualified,
                "apple": o.apple.path, "mine": o.mine.path,
                "actions": [a.to_dict() for a in o.actions if a.type != "missing_both"],
                "missing_both": [a.key for a in o.actions if a.type == "missing_both"],
            })
            for a in o.pending:
                pending.append({"kind": "signature_conflict", "class": o.apple.qualified, "key": a.key,
                                "detail": a.detail, "decision_key": f"{o.objc_class} {a.key}"})
        enums = []
        for o in check.enum_overlaps:
            enums.append({"cpp_enum": o.qualified, "apple": o.apple.path, "mine": o.mine.path,
                          "actions": [a.to_dict() for a in o.actions]})
            for a in o.pending:
                pending.append({"kind": "enum_value_conflict", "enum": o.qualified, "key": a.key, "detail": a.detail})
        report["overlaps"] = overlaps
        report["enum_overlaps"] = enums
        report["const_overlaps"] = [{"name": a.qualified, "apple": a.path, "mine": m.path}
                                    for a, m in check.const_overlaps]

    removals = ctx.get("removals")
    if removals is not None:
        report["removals"] = removals
        for r in removals:
            if r.get("usages"):
                pending.append({"kind": "removed_upstream_but_used", "name": r["name"],
                                "detail": f"{r['kind']} removed upstream, used at {', '.join(r['usages'][:5])}"})
    if ctx.get("additions") is not None:
        report["additions"] = ctx["additions"]

    applied = ctx.get("apply")
    if applied is not None:
        report["apply"] = {"applied": applied.applied, "skipped": applied.skipped, "errors": applied.errors,
                           "changed": sorted(p for p in applied.changed if p not in applied.deleted),
                           "deleted": applied.deleted}
        for e in applied.errors:
            pending.append({"kind": "apply_error", "detail": e})

    build = ctx.get("build")
    if build is not None:
        report["build"] = build.to_dict()
        for s in build.steps:
            if s.ok or s.warning:
                continue
            detail = "; ".join(f"{e['file']}:{e['line']}: {e['message']}" for e in s.errors[:5])
            if not detail and s.output.strip():
                detail = s.output.strip().splitlines()[-1]
            pending.append({"kind": "build_failed", "step": s.name, "detail": detail})

    if ctx.get("audit") is not None:
        report["audit"] = ctx["audit"]
    return report


def exit_code(report) -> int:
    return 1 if report["pending"] else 0


# Markdown -----------------------------------------------------------------------------------------------------------


def _code(s):
    return f"`{s}`" if s else ""


def _section(out, title):
    if out[-1] != "":
        out.append("")
    out.append(f"## {title}")
    out.append("")


def _render_pending(out, report):
    _section(out, "Pending")
    if not report["pending"]:
        out.append("Nothing pending.")
        return
    decisions = [p for p in report["pending"] if p.get("decision_key")]
    for p in report["pending"]:
        what = p.get("path") or p.get("class") or p.get("enum") or p.get("name") or p.get("step") or ""
        key = f" {_code(p['key'])}" if p.get("key") else ""
        out.append(f"- **{p['kind']}** {_code(what)}{key}: {p.get('detail', '')}")
    if decisions:
        out.append("")
        out.append("Decide each signature conflict in `tools/upstream/decisions.json` (`apple`, `mine` or `skip`):")
        out.append("")
        out.append("```json")
        out.append('"members": {')
        out.append(",\n".join(f'    "{p["decision_key"]}": "apple"' for p in decisions))
        out.append("}")
        out.append("```")


def _render_conflicts(out, report):
    c = report.get("conflicts")
    if not c or not (c["resolved"] or c["pending"]):
        return
    _section(out, "Merge conflicts")
    for path in c["resolved"]:
        saved = c["saved"].get(path)
        out.append(f"- {_code(path)}: taken from upstream" + (f" (our side saved in {_code(saved)})" if saved else ""))
    for r in c["reinjected"]:
        out.append(f"  - re-injected {_code(r)}")
    for p in c["pending"]:
        out.append(f"- {_code(p['path'])}: **{p['reason']}**")


def _render_overlaps(out, report):
    overlaps = report.get("overlaps") or []
    enums = report.get("enum_overlaps") or []
    consts = report.get("const_overlaps") or []
    if not (overlaps or enums or consts):
        return
    _section(out, "Classes that exist in Apple's folders and in SylvaGX folders")
    labels = {
        "copy": "copied into Apple's class",
        "replace": "replaced Apple's member (decision: mine)",
        "mine_not_in_sdk": "dropped, not in the SDK",
        "mine_deprecated": "dropped, deprecated",
        "signature_conflict": "**conflict**",
    }
    for o in overlaps:
        out.append(f"### {_code(o['cpp_class'])} ({o['objc_class']})")
        out.append("")
        out.append(f"Apple: {_code(o['apple'])}, SylvaGX: {_code(o['mine'])}")
        out.append("")
        for a in o["actions"]:
            out.append(f"- {_code(a['key'])}: {labels.get(a['type'], a['type'])}"
                       + (f" ({a['detail']})" if a["type"] not in ("copy",) and a.get("detail") else ""))
        if not o["actions"]:
            out.append("- SylvaGX class adds nothing over Apple's: removed")
        if o["missing_both"]:
            out.append(f"- not bound by either side ({len(o['missing_both'])}): "
                       + ", ".join(_code(k) for k in o["missing_both"][:15])
                       + (" ..." if len(o["missing_both"]) > 15 else ""))
        out.append("")
    for e in enums:
        out.append(f"### enum {_code(e['cpp_enum'])}")
        out.append("")
        out.append(f"Apple: {_code(e['apple'])}, SylvaGX: {_code(e['mine'])}")
        out.append("")
        for a in e["actions"]:
            out.append(f"- {_code(a['key'])}: {a['type']} {a.get('detail', '')}")
        out.append("")
    for c in consts:
        out.append(f"- constant {_code(c['name'])}: kept Apple's ({_code(c['apple'])}), removed {_code(c['mine'])}")


def _render_apply(out, report):
    a = report.get("apply")
    if not a:
        return
    _section(out, "Changes written")
    if not (a["applied"] or a["skipped"] or a["errors"]):
        out.append("No changes.")
        return
    for line in a["applied"]:
        out.append(f"- {line}")
    for line in a["skipped"]:
        out.append(f"- skipped: {line}")
    for line in a["errors"]:
        out.append(f"- **error**: {line}")


def _render_upstream(out, report):
    removals = report.get("removals")
    additions = report.get("additions")
    if removals is None and additions is None:
        return
    _section(out, "Upstream changes in Apple's folders")
    if additions:
        out.append(f"- added classes: {len(additions['classes'])}, added members: {additions['members']}, "
                   f"added enums: {len(additions['enums'])}, added constants: {len(additions['constants'])}")
        for q in additions["classes"][:40]:
            out.append(f"  - {_code(q)}")
    if removals:
        out.append(f"- removed / changed: {len(removals)}")
        for r in removals:
            used = f", **used at** {', '.join(_code(u) for u in r['usages'])}" if r.get("usages") else ""
            sig = f" ({_code(r['signature'])})" if r.get("signature") else ""
            out.append(f"  - {r['kind']} {_code(r['name'])}{sig}{used}")
    elif removals is not None:
        out.append("- nothing removed")


def _render_build(out, report):
    b = report.get("build")
    if not b:
        return
    _section(out, "Build")
    for s in b["steps"]:
        state = "skipped (" + s["skipped"] + ")" if s["skipped"] else ("ok" if s["ok"] else
                                                                        ("warning" if s["warning"] else "**failed**"))
        out.append(f"- {s['name']}: {state}")
        if not s["ok"] and not s["skipped"]:
            for e in s["errors"][:10]:
                out.append(f"  - {_code(e['file'] + ':' + str(e['line']))}: {e['message']}")
            if not s["errors"]:
                tail = s["output"].strip().splitlines()[-1:] if s["output"].strip() else []
                for line in tail:
                    out.append(f"  - {line}")
    if b["redefinitions"]:
        out.append("")
        out.append("Redefinitions usually mean Apple now ships a class that also exists in a SylvaGX folder:")
        for e in b["redefinitions"]:
            out.append(f"- {_code(e['file'] + ':' + str(e['line']))}: {e['message']}")


_AUDIT_ORDER = ["value_mismatch", "signature_mismatch", "static_mismatch", "deprecated", "unavailable",
                "not_in_sdk", "missing"]


def _render_audit(out, report):
    audit = report.get("audit")
    if audit is None:
        return
    _section(out, "Audit against the SDK")
    totals = {}
    for e in audit:
        for i in e["items"]:
            totals[i["type"]] = totals.get(i["type"], 0) + 1
    out.append(", ".join(f"{k}: {totals[k]}" for k in _AUDIT_ORDER if k in totals) or "No findings.")
    unmapped = [e for e in audit if e.get("status") in ("unmapped", "not_in_sdk")]
    if unmapped:
        out.append("")
        out.append("Not found in the SDK: " + ", ".join(
            _code(e.get("cpp_class") or e.get("cpp_enum") or e.get("cpp_constant")) for e in unmapped))
    for e in audit:
        items = [i for i in e["items"] if i["type"] != "missing"]
        missing = [i for i in e["items"] if i["type"] == "missing"]
        name = e.get("cpp_class") or e.get("cpp_enum") or e.get("cpp_constant")
        if not items and not missing:
            continue
        out.append("")
        out.append(f"### {_code(name)} ({_code(e['path'])})")
        out.append("")
        for i in sorted(items, key=lambda i: _AUDIT_ORDER.index(i["type"]) if i["type"] in _AUDIT_ORDER else 99):
            detail = f": {i['detail']}" if i.get("detail") else ""
            out.append(f"- {i['type']} {_code(i.get('signature') or i['key'])}{detail}")
        if missing:
            out.append(f"- missing ({len(missing)}): " + ", ".join(_code(i["key"]) for i in missing[:20])
                       + (" ..." if len(missing) > 20 else ""))


def render_markdown(report) -> str:
    out = ["# Upstream sync report", ""]
    frm, to = report["range"]
    if report.get("command"):
        out.append(f"- command: {_code(report['command'])}")
    if frm or to:
        out.append(f"- range: {_code(frm)} -> {_code(to)}")
    if report.get("branch"):
        out.append(f"- branch: {_code(report['branch'])}")
    if report.get("merge"):
        out.append(f"- merge: {report['merge']}")
    if report.get("sdks"):
        out.append("- SDKs: " + ", ".join(f"{k} {v}" for k, v in report["sdks"].items()))
    out.append(f"- date: {report['date']}")
    for n in report["notes"]:
        out.append(f"- {n}")
    _render_pending(out, report)
    _render_conflicts(out, report)
    _render_overlaps(out, report)
    _render_apply(out, report)
    _render_upstream(out, report)
    _render_build(out, report)
    _render_audit(out, report)
    return "\n".join(out).rstrip() + "\n"


def write(repo, report, md_path=None, json_path=None):
    md = Path(md_path) if md_path else Path(repo) / REPORT_MD
    js = Path(json_path) if json_path else Path(repo) / REPORT_JSON
    md.write_text(render_markdown(report))
    js.write_text(json.dumps(report, indent=2, default=str) + "\n")
    return md, js
