#!/usr/bin/env python3
"""Keeps this repo in sync with apple/metal-cpp without losing the SylvaGX bindings.

    update              merge the next apple/metal-cpp release, reconcile, build and report
    check               compare SylvaGX classes with Apple's (and upstream removals with --from)
    resolve-conflicts   resolve the conflicts of a merge in progress
    audit               compare bound classes, enums and constants with the SDK headers
    report              re-render upstream-report.md from upstream-report.json

See tools/upstream/README.md.
"""

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import apply as apply_mod  # noqa: E402
import build as build_mod  # noqa: E402
import compare  # noqa: E402
import cpp_api  # noqa: E402
import gitops  # noqa: E402
import report as report_mod  # noqa: E402
import sdk_api  # noqa: E402

TOOL_DIR = Path(__file__).resolve().parent
DEFAULT_SDKS = ["macosx", "iphoneos"]


class Abort(Exception):
    pass


# Shared helpers -----------------------------------------------------------------------------------------------------


def _say(msg=""):
    print(msg, flush=True)


def _context(args):
    repo = Path(args.repo).resolve() if args.repo else None
    git = gitops.Git(repo or Path.cwd())
    if repo is None:
        try:
            repo = git.root()
        except gitops.GitError:
            raise Abort("not inside a git repository (use --repo)")
        git = gitops.Git(repo)
    cache = Path(args.cache).resolve() if args.cache else repo / ".upstream-cache"
    decisions_path = Path(args.decisions) if args.decisions else TOOL_DIR / "decisions.json"
    return repo, git, cache, compare.Decisions(decisions_path)


def _load_sdk(args, cache, min_major=None):
    sdks = args.sdk or DEFAULT_SDKS
    for sdk in sdks:
        if min_major:
            sdk_api.require_sdk(sdk, min_major)
    _say(f"Reading SDK headers ({', '.join(sdks)})...")
    return sdk_api.SdkApi.load(sdks, cache, refresh=args.refresh_sdk)


def _finish(args, repo, ctx):
    rep = report_mod.collect(ctx)
    md, js = report_mod.write(repo, rep, args.report, None)
    _say("")
    if rep["pending"]:
        _say(f"{len(rep['pending'])} item(s) need you:")
        for p in rep["pending"][:15]:
            what = p.get("path") or p.get("class") or p.get("enum") or p.get("name") or p.get("step") or ""
            _say(f"  - {p['kind']} {what} {p.get('key') or ''}".rstrip())
        if len(rep["pending"]) > 15:
            _say(f"  ... and {len(rep['pending']) - 15} more")
    else:
        _say("Nothing pending.")
    _say(f"Report: {md}")
    return report_mod.exit_code(rep)


def _print_apply(result, dry_run):
    verb = "Would apply" if dry_run else "Applied"
    if not (result.applied or result.skipped or result.errors):
        _say("No reconciliation needed.")
        return
    _say(f"{verb} {len(result.applied)} change(s):")
    for line in result.applied:
        _say(f"  {line}")
    for line in result.skipped:
        _say(f"  skipped: {line}")
    for line in result.errors:
        _say(f"  error: {line}")


def _upstream_changes(git, repo, old_rev, new_rev):
    folders = list(cpp_api.APPLE_FOLDERS)
    old = cpp_api.CppIndex.from_git(git, old_rev, folders)
    new = cpp_api.CppIndex.from_git(git, new_rev, folders)
    removed, added = compare.diff_upstream(old, new)
    compare.find_usages(removed, repo, list(cpp_api.MINE_FOLDERS) + ["test"])
    return removed, added


def _reconcile(args, repo, api, decisions, dry_run, ctx):
    index = cpp_api.CppIndex.from_disk(repo, cpp_api.FRAMEWORKS)
    result = compare.check(api, index, decisions)
    applied = apply_mod.apply(index, result, repo, dry_run=dry_run)
    ctx["check"], ctx["apply"] = result, applied
    _say(f"Overlapping classes: {len(result.overlaps)}, enums: {len(result.enum_overlaps)}, "
         f"constants: {len(result.const_overlaps)}")
    _print_apply(applied, dry_run)
    if dry_run and applied.touched:
        diff = apply_mod.unified_diff(index.files, applied)
        diff_path = Path(args.cache_dir) / "apply.diff"
        diff_path.parent.mkdir(parents=True, exist_ok=True)
        diff_path.write_text(diff)
        _say(f"Diff: {diff_path}")
    return result, applied


def _build(args, repo, ctx):
    if args.no_build:
        return None
    _say("Building (test/ with CMake, plain C++ translation unit, single header)...")
    result = build_mod.run(repo)
    for s in result.steps:
        state = f"skipped ({s.skipped})" if s.skipped else ("ok" if s.ok else ("warning" if s.warning else "FAILED"))
        _say(f"  {s.name}: {state}")
    ctx["build"] = result
    return result


# Commands -----------------------------------------------------------------------------------------------------------


def cmd_update(args):
    repo, git, cache, decisions = _context(args)
    args.cache_dir = cache
    if git.merge_in_progress():
        raise Abort("a merge is already in progress: finish it (or `git merge --abort`) first; "
                    "`resolve-conflicts` handles conflicts of a merge in progress")
    if not git.is_clean():
        raise Abort("the work tree has uncommitted changes to tracked files: commit or stash them first")

    if git.ensure_upstream():
        _say(f"Added remote upstream -> {gitops.UPSTREAM_URL}")
    if not args.no_fetch:
        _say("Fetching upstream tags...")
        git.fetch_upstream()

    merged = git.merged_tags()
    if args.tag:
        target = args.tag
        if git.is_ancestor(target):
            _say(f"{target} is already merged.")
            return 0
    else:
        pending = git.pending_tags()
        pending = [t for t in pending if not merged or gitops.tag_version(t) > gitops.tag_version(merged[-1])]
        if not pending:
            _say(f"Up to date ({merged[-1] if merged else 'no release merged'}).")
            return 0
        target = pending[-1]
    source = args.from_rev or (merged[-1] if merged else None)
    if source is None:
        raise Abort("no apple/metal-cpp release is an ancestor of HEAD: link the histories first "
                    "(git merge -s ours --allow-unrelated-histories <tag>, see README)")
    _say(f"Updating {source} -> {target}")

    major = gitops.tag_version(target)[0]
    api = _load_sdk(args, cache, min_major=major)
    ctx = {"command": "update" + (" --dry-run" if args.dry_run else ""), "range": (source, target),
           "sdks": api.sdks}

    removed, added = _upstream_changes(git, repo, source, target)
    ctx["removals"], ctx["additions"] = removed, added
    _say(f"Upstream: {len(added['classes'])} new class(es), {added['members']} new member(s), "
         f"{len(removed)} removal(s) / signature change(s)")

    if args.dry_run:
        conflicts = git.predict_conflicts(target)
        ctx["merge"] = f"dry run: {len(conflicts)} file(s) would conflict" + (
            f" ({', '.join(conflicts)})" if conflicts else "")
        _say(ctx["merge"])
        _reconcile(args, repo, api, decisions, True, ctx)
        ctx["notes"] = ["dry run: overlaps were computed on the current tree, before the merge"]
        return _finish(args, repo, ctx)

    branch = args.branch or f"update/{gitops.tag_short_name(target)}"
    if git.branch_exists(branch):
        raise Abort(f"branch {branch} already exists (delete it or pass --branch)")
    git.create_branch(branch)
    ctx["branch"] = branch
    _say(f"Created branch {branch}")

    clean = git.merge(target)
    ctx["merge"] = "clean" if clean else "conflicts"
    _say(f"Merge: {ctx['merge']}")
    if not clean:
        conflicts = apply_mod.resolve_conflicts(git, api, repo, cache, decisions)
        ctx["conflicts"] = conflicts
        for path in conflicts.resolved:
            _say(f"  resolved {path} (upstream's version)")
        for line in conflicts.reinjected:
            _say(f"  re-injected {line}")
        if git.unmerged_files():
            ctx["notes"] = ["merge left in progress: resolve the files listed under Pending, `git add` them, then run "
                            "`check_upstream.py check --apply` and commit"]
            return _finish(args, repo, ctx)

    _reconcile(args, repo, api, decisions, False, ctx)
    _build(args, repo, ctx)
    code = _finish(args, repo, ctx)

    if args.commit:
        if code != 0:
            _say("Not committing: something is pending.")
        else:
            short = gitops.tag_short_name(target)
            git.commit()
            _say(f"Committed the merge of {target}")
            touched = ctx["apply"].touched if ctx.get("apply") else []
            if touched:
                git.add(*touched)
                git.commit(f"Reconcile SylvaGX bindings with apple/metal-cpp {short}")
                _say("Committed the reconciliation")
    else:
        _say("Nothing was committed: review with `git status` / `git diff`, then commit "
             "(`git commit` finishes the merge).")
    return code


def cmd_check(args):
    repo, git, cache, decisions = _context(args)
    args.cache_dir = cache
    api = _load_sdk(args, cache)
    ctx = {"command": "check" + (" --apply" if args.apply else ""), "sdks": api.sdks}
    if args.from_rev:
        to = args.to or "HEAD"
        removed, added = _upstream_changes(git, repo, args.from_rev, to)
        ctx["range"], ctx["removals"], ctx["additions"] = (args.from_rev, to), removed, added
        _say(f"Upstream {args.from_rev} -> {to}: {len(added['classes'])} new class(es), "
             f"{len(removed)} removal(s) / signature change(s)")
    _reconcile(args, repo, api, decisions, not args.apply, ctx)
    if args.apply:
        _build(args, repo, ctx)
    return _finish(args, repo, ctx)


def cmd_resolve_conflicts(args):
    repo, git, cache, decisions = _context(args)
    if not git.merge_in_progress():
        raise Abort("no merge in progress")
    api = _load_sdk(args, cache)
    result = apply_mod.resolve_conflicts(git, api, repo, cache, decisions, dry_run=args.dry_run)
    for path in result.resolved:
        _say(f"{'would resolve' if args.dry_run else 'resolved'} {path} (upstream's version)")
    for line in result.reinjected:
        _say(f"  re-injected {line}")
    ctx = {"command": "resolve-conflicts", "conflicts": result, "sdks": api.sdks}
    return _finish(args, repo, ctx)


_SCOPES = {"apple": cpp_api.APPLE_FOLDERS, "mine": cpp_api.MINE_FOLDERS, "all": tuple(cpp_api.FRAMEWORKS)}


def cmd_audit(args):
    repo, git, cache, decisions = _context(args)
    folders = []
    for scope in args.scope or ["all"]:
        if scope in _SCOPES:
            folders += list(_SCOPES[scope])
        elif scope in cpp_api.FRAMEWORKS:
            folders.append(scope)
        else:
            raise Abort(f"unknown scope {scope!r} (apple, mine, all or a folder: {', '.join(cpp_api.FRAMEWORKS)})")
    folders = list(dict.fromkeys(folders))
    api = _load_sdk(args, cache)
    index = cpp_api.CppIndex.from_disk(repo, cpp_api.FRAMEWORKS)
    entries = compare.audit(api, index, decisions, folders)
    totals = {}
    for e in entries:
        for i in e["items"]:
            totals[i["type"]] = totals.get(i["type"], 0) + 1
    _say(f"Audited {', '.join(folders)}: " + (", ".join(f"{k} {v}" for k, v in sorted(totals.items())) or "clean"))
    ctx = {"command": "audit " + " ".join(args.scope or ["all"]), "audit": entries, "sdks": api.sdks}
    _finish(args, repo, ctx)
    return 0


def cmd_report(args):
    import json
    repo, _, _, _ = _context(args)
    js = repo / report_mod.REPORT_JSON
    if not js.exists():
        raise Abort(f"{js} not found: run update, check, resolve-conflicts or audit first")
    rep = json.loads(js.read_text())
    md, _ = report_mod.write(repo, rep, args.report, js)
    _say(f"Report: {md}")
    for p in rep["pending"]:
        what = p.get("path") or p.get("class") or p.get("enum") or p.get("name") or p.get("step") or ""
        _say(f"  - {p['kind']} {what} {p.get('key') or ''}".rstrip())
    return report_mod.exit_code(rep)


# CLI ----------------------------------------------------------------------------------------------------------------


def parser():
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument("--repo", help="repository root (default: the git root of the current directory)")
    common.add_argument("--sdk", action="append", choices=sorted(sdk_api.SDK_FRAMEWORKS),
                        help="SDK to read (repeatable, default: macosx and iphoneos)")
    common.add_argument("--refresh-sdk", action="store_true", help="re-dump the SDK headers, ignoring the cache")
    common.add_argument("--decisions", help="decisions file (default: tools/upstream/decisions.json)")
    common.add_argument("--cache", help="cache directory (default: <repo>/.upstream-cache)")
    common.add_argument("--report", help="markdown report path (default: <repo>/upstream-report.md)")
    common.add_argument("--no-build", action="store_true", help="skip the compile checks")

    p = argparse.ArgumentParser(prog="check_upstream.py", description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="command", required=True)

    u = sub.add_parser("update", parents=[common], help="merge the next apple/metal-cpp release")
    u.add_argument("--tag", help="release tag to merge (default: the newest one not merged yet)")
    u.add_argument("--from", dest="from_rev", help="previous release (default: the newest merged release tag)")
    u.add_argument("--branch", help="branch to create (default: update/<version>)")
    u.add_argument("--no-fetch", action="store_true", help="do not fetch upstream")
    u.add_argument("--commit", action="store_true", help="commit the merge and the reconciliation when nothing is pending")
    u.add_argument("--dry-run", action="store_true", help="report what would happen without touching the tree")
    u.set_defaults(func=cmd_update)

    c = sub.add_parser("check", parents=[common], help="compare SylvaGX classes with Apple's")
    c.add_argument("--from", dest="from_rev", help="also list upstream removals since this revision")
    c.add_argument("--to", help="end of the upstream range (default: HEAD)")
    c.add_argument("--apply", action="store_true", help="write the changes (default: dry run)")
    c.set_defaults(func=cmd_check)

    r = sub.add_parser("resolve-conflicts", parents=[common], help="resolve the conflicts of a merge in progress")
    r.add_argument("--dry-run", action="store_true")
    r.set_defaults(func=cmd_resolve_conflicts)

    a = sub.add_parser("audit", parents=[common], help="compare the bindings with the SDK headers")
    a.add_argument("--scope", action="append", help="apple, mine, all (default) or a folder; repeatable")
    a.set_defaults(func=cmd_audit)

    rp = sub.add_parser("report", parents=[common], help="re-render the markdown report from the JSON one")
    rp.set_defaults(func=cmd_report)
    return p


def main(argv=None):
    args = parser().parse_args(argv)
    try:
        return args.func(args)
    except (Abort, gitops.GitError, sdk_api.SdkError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
