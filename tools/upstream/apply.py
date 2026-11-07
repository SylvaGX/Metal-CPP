"""Applies the actions computed by compare.check() to the header files.

- `copy` / `replace`: the member declaration goes into Apple's class body, the `_XX_INLINE` definition after the
  class's last inline (macros converted to the destination framework), and the selector / class lookups into the
  destination framework's private header.
- `enum_copy`: the constant is added to Apple's enum.
- Once a class (or enum / constant) has nothing pending, the SylvaGX duplicate is removed, together with private
  selectors and class lookups that are no longer referenced. Files left without any definition are deleted and
  includes pointing at them are redirected to Apple's header.
"""

import difflib
import os
import re
from dataclasses import dataclass, field
from pathlib import Path

import compare
import cpp_api


@dataclass
class Edit:
    start: int
    end: int
    text: str


@dataclass
class ApplyResult:
    changed: dict = field(default_factory=dict)     # path -> new text
    deleted: list = field(default_factory=list)
    applied: list = field(default_factory=list)     # human readable lines
    skipped: list = field(default_factory=list)
    errors: list = field(default_factory=list)

    @property
    def touched(self):
        return sorted(set(self.changed) | set(self.deleted))


class Editor:
    def __init__(self, files):
        self.original = dict(files)
        self.files = dict(files)
        self.edits = {}

    def add(self, path, start, end, text=""):
        self.edits.setdefault(path, []).append(Edit(start, end, text))

    def flush(self):
        """Applies the queued edits (offsets refer to the current text of each file).

        Overlapping deletions are merged; insertions at the same offset keep their queue order and land before
        whatever follows the offset.
        """
        for path, edits in self.edits.items():
            text = self.files[path]
            deletions = sorted((e for e in edits if e.end > e.start), key=lambda e: e.start)
            merged = []
            for e in deletions:
                if merged and e.start <= merged[-1].end:
                    merged[-1].end = max(merged[-1].end, e.end)
                    merged[-1].text += e.text
                else:
                    merged.append(Edit(e.start, e.end, e.text))
            inserts = [(i, e) for i, e in enumerate(edits) if e.end == e.start]
            ops = [(e.start, 1, 0, e) for e in merged] + [(e.start, 0, i, e) for i, e in inserts]
            for start, _, _, e in sorted(ops, key=lambda o: (o[0], o[1], o[2]), reverse=True):
                inside = next((d for d in merged if d.start < start < d.end), None)
                if inside is not None:
                    continue
                text = text[:e.start] + e.text + text[e.end:]
            self.files[path] = text
        self.edits = {}


def _indent_of(text):
    return re.match(r"[ \t]*", text).group(0)


def _reindent(block, indent):
    lines = block.splitlines(keepends=True)
    common = min((len(_indent_of(l)) for l in lines if l.strip()), default=0)
    return "".join(indent + l[common:] if l.strip() else l for l in lines)


def _convert_macros(text, src_macro, dst_macro, src_qualified=None, dst_qualified=None):
    if src_macro != dst_macro:
        text = re.sub(rf"\b_{src_macro}_(INLINE|PRIVATE_SEL|PRIVATE_CLS|PRIVATE_PRO)\b", rf"_{dst_macro}_\1", text)
    if src_qualified and dst_qualified and src_qualified != dst_qualified:
        text = text.replace(f"{src_qualified}::", f"{dst_qualified}::")
    return text


def _consume_blank_lines(text, pos):
    while pos < len(text):
        end = text.find("\n", pos)
        end = len(text) if end < 0 else end + 1
        if text[pos:end].strip():
            break
        pos = end
    return pos


def _removal_range(text, start, end):
    """Extends [start, end) over the separator comment line and blank lines that belong to the block."""
    end = _consume_blank_lines(text, end)
    above = _consume_blank_lines_back(text, start)
    if above > 0:
        prev = text.rfind("\n", 0, above - 1) + 1
        if re.fullmatch(r"\s*//-{10,}\s*", text[prev:above]):
            start = _consume_blank_lines_back(text, prev)
    return start, end


def _consume_blank_lines_back(text, pos):
    while pos > 0:
        prev = text.rfind("\n", 0, pos - 1) + 1
        if prev >= pos or text[prev:pos].strip():
            break
        pos = prev
    return pos


# Private header insertion -------------------------------------------------------------------------------------------

_RE_DEF_SEL_ENTRY = re.compile(r"^[ \t]*_([A-Z0-9]+)_PRIVATE_DEF_SEL\(\s*(\w+)\s*,\s*\"[^\"]*\"\s*\)\s*;[ \t]*\n", re.M)
_RE_DEF_CLS_ENTRY = re.compile(r"^[ \t]*_([A-Z0-9]+)_PRIVATE_DEF_CLS\(\s*(\w+)\s*\)\s*;[ \t]*\n", re.M)


def _sorted_insert(editor, path, regex, macro, name, render):
    text = editor.original[path]
    entries = [m for m in regex.finditer(text) if m.group(1) == macro]
    if not entries:
        return False
    after = None
    for m in entries:
        if m.group(2).lower() > name.lower():
            editor.add(path, m.start(), m.start(), render(m.group(0), name))
            return True
        after = m
    editor.add(path, after.end(), after.end(), render(after.group(0), name))
    return True


def _render_sel(template, accessor, selector):
    out = re.sub(r"(DEF_SEL\(\s*)\w+", rf"\g<1>{accessor}", template, count=1)
    return re.sub(r"\"[^\"]*\"", f"\"{selector}\"", out, count=1)


def _render_cls(template, name):
    return re.sub(r"(DEF_CLS\(\s*)\w+", rf"\g<1>{name}", template, count=1)


# Applier ------------------------------------------------------------------------------------------------------------


class Applier:
    def __init__(self, index: cpp_api.CppIndex, source: cpp_api.CppIndex = None):
        self.index = index
        self.source = source or index   # where the copied members (and their selectors) come from
        self.editor = Editor(index.files)
        self.result = ApplyResult()
        self.added_sels = set()
        self.added_cls = set()
        self.removed_mine = []          # (CppClass | EnumDef | ConstDef)

    # Selectors / classes ----------------------------------------------------------------------------------------------

    def _ensure_selector(self, member, dst_fw):
        for accessor in member.accessors:
            src = self.source.selector_def(member.macro, accessor)
            if src is None:
                self.result.errors.append(f"{member.cls}::{member.name}: accessor {accessor} has no selector definition")
                return False
            existing = self.index.selector_def(dst_fw.macro, accessor)
            if existing is not None:
                if existing.selector != src.selector:
                    self.result.errors.append(
                        f"{member.cls}::{member.name}: accessor {accessor} is \"{existing.selector}\" in "
                        f"{dst_fw.selector_file} but \"{src.selector}\" in {src.span.path}")
                    return False
                continue
            if (dst_fw.macro, accessor) in self.added_sels:
                continue
            ok = _sorted_insert(self.editor, dst_fw.selector_file, _RE_DEF_SEL_ENTRY, dst_fw.macro, accessor,
                                lambda tpl, name, sel=src.selector: _render_sel(tpl, name, sel))
            if not ok:
                self.result.errors.append(f"no _{dst_fw.macro}_PRIVATE_DEF_SEL list found in {dst_fw.selector_file}")
                return False
            self.added_sels.add((dst_fw.macro, accessor))
            self.result.applied.append(f"selector {accessor} (\"{src.selector}\") -> {dst_fw.selector_file}")
        return True

    def _ensure_classes(self, member, dst_fw):
        for name in member.objc_classes:
            if name in self.index.class_defs.get(dst_fw.macro, {}) or (dst_fw.macro, name) in self.added_cls:
                continue
            if _sorted_insert(self.editor, dst_fw.class_file, _RE_DEF_CLS_ENTRY, dst_fw.macro, name, _render_cls):
                self.added_cls.add((dst_fw.macro, name))
                self.result.applied.append(f"class lookup {name} -> {dst_fw.class_file}")
            else:
                self.result.errors.append(f"no _{dst_fw.macro}_PRIVATE_DEF_CLS list found in {dst_fw.class_file}")
                return False
        return True

    # Members ----------------------------------------------------------------------------------------------------------

    def _decl_anchor(self, cls):
        """(insert offset, indentation, return-type column width) for a new declaration in `cls`."""
        text = self.editor.original[cls.path]
        decls = [m for m in cls.members if m.decl and m.decl.path == cls.path]
        if decls:
            last = max(decls, key=lambda m: m.decl.end)
            line = text[last.decl.start:last.decl.end]
            width = 0
            am = re.match(rf"^[ \t]*(.*?\S)(\s+)(?:\w+::)*{re.escape(last.name)}\s*\(", line)
            if am and len(am.group(2)) > 1:
                width = len(am.group(1)) + len(am.group(2))
            return last.decl.end, _indent_of(line), width
        close = cls.body.end
        line_start = text.rfind("\n", 0, close) + 1
        return line_start, _indent_of(text[cls.span.start:cls.span.end]) + "    ", 0

    def _inline_anchor(self, cls):
        inlines = sorted((m.inline for m in cls.members if m.inline and m.inline.path == cls.path),
                         key=lambda s: s.start)
        text = self.editor.original[cls.path]
        if not inlines:
            return cls.span.end, "\n"
        last = inlines[-1]
        gap = "\n"
        if len(inlines) > 1:
            between = text[inlines[-2].end:last.start]
            if not between.strip() or re.fullmatch(r"\s*//-+\s*", between):
                gap = between
        return last.end, gap

    def _format_decl(self, member, indent, width):
        decl = _reindent(member.decl_text, indent)
        if width:
            dm = re.match(rf"^([ \t]*)(.*?\S)\s+({re.escape(member.name)}\s*\()", decl)
            if dm and len(dm.group(2)) < width:
                decl = dm.group(1) + dm.group(2).ljust(width) + decl[dm.start(3):]
        return decl

    def copy_member(self, member, dst_cls, replacing=None):
        """Copies `member` into `dst_cls`; with `replacing`, Apple's member is replaced in place."""
        dst_fw = dst_cls.framework
        if not self._ensure_selector(member, dst_fw) or not self._ensure_classes(member, dst_fw):
            self.result.skipped.append(f"{dst_cls.qualified}::{member.name}: not copied (see errors)")
            return False
        inline = _convert_macros(member.inline_text, member.macro, dst_fw.macro, member.cls, dst_cls.qualified)
        if replacing is not None and replacing.decl and replacing.inline:
            text = self.editor.original[dst_cls.path]
            line = text[replacing.decl.start:replacing.decl.end]
            pos, indent, width = self._decl_anchor(dst_cls)
            self.editor.add(dst_cls.path, replacing.decl.start, replacing.decl.end,
                            self._format_decl(member, _indent_of(line), width))
            self.editor.add(replacing.inline.path, replacing.inline.start, replacing.inline.end, inline)
            self.result.applied.append(f"replaced {replacing.signature} with mine in {dst_cls.qualified} ({dst_cls.path})")
            return True
        if member.decl_text:
            pos, indent, width = self._decl_anchor(dst_cls)
            self.editor.add(dst_cls.path, pos, pos, self._format_decl(member, indent, width))
        if member.inline_text:
            pos, gap = self._inline_anchor(dst_cls)
            self.editor.add(dst_cls.path, pos, pos, gap + inline)
        self.result.applied.append(f"copied {member.signature} -> {dst_cls.qualified} ({dst_cls.path})")
        return True

    def remove_class(self, cls):
        text = self.editor.original[cls.path]
        start, end = _removal_range(text, cls.span.start, cls.span.end)
        self.editor.add(cls.path, start, end, "")
        for m in cls.members:
            if m.inline and m.inline.path:
                t = self.editor.original[m.inline.path]
                s, e = _removal_range(t, m.inline.start, m.inline.end)
                self.editor.add(m.inline.path, s, e, "")
        self.removed_mine.append(cls)
        self.result.applied.append(f"removed duplicate {cls.qualified} ({cls.path})")

    def copy_enum_value(self, action):
        apple, name, value = action.apple, action.key, action.extra.get("value", "")
        text = self.editor.original[apple.path]
        body = text[apple.span.start:apple.span.end]
        close = body.rfind("}")
        entries = list(re.finditer(r"^([ \t]*)(\w+)(\s*)=", body, re.M))
        indent = entries[-1].group(1) if entries else "    "
        pad = ""
        if entries:
            pad = " " * max(1, len(entries[-1].group(2)) + len(entries[-1].group(3)) - len(name))
        line = f"{indent}{name}{pad}= {value},\n" if value else f"{indent}{name},\n"
        pos = apple.span.start + text[apple.span.start:apple.span.start + close].rfind("\n") + 1
        self.editor.add(apple.path, pos, pos, line)
        self.result.applied.append(f"enum value {apple.namespace}::{name} -> {apple.qualified} ({apple.path})")

    def remove_span(self, item):
        text = self.editor.original[item.path]
        start, end = _removal_range(text, item.span.start, item.span.end)
        self.editor.add(item.path, start, end, "")
        self.removed_mine.append(item)
        self.result.applied.append(f"removed duplicate {item.qualified} ({item.path})")

    # Cleanup ----------------------------------------------------------------------------------------------------------

    def _cleanup_private(self):
        mine_texts = {p: t for p, t in self.editor.files.items() if p.split("/", 1)[0] in cpp_api.MINE_FOLDERS}
        joined = "\n".join(mine_texts.values())
        removed_accessors = {(m.macro, a) for c in self.removed_mine if isinstance(c, cpp_api.CppClass)
                             for m in c.members for a in m.accessors}
        removed_classes = {(m.macro, n) for c in self.removed_mine if isinstance(c, cpp_api.CppClass)
                           for m in c.members for n in m.objc_classes}
        for macro, accessor in sorted(removed_accessors):
            if re.search(rf"_{macro}_PRIVATE_SEL\(\s*{accessor}\s*\)", joined):
                continue
            sel = self.index.selector_def(macro, accessor)
            if sel and sel.span.path.split("/", 1)[0] in cpp_api.MINE_FOLDERS:
                text = self.editor.files[sel.span.path]
                m = re.search(rf"^[ \t]*_{macro}_PRIVATE_DEF_SEL\(\s*{accessor}\s*,[^;]*;[ \t]*\n", text, re.M)
                if m:
                    self.editor.add(sel.span.path, m.start(), m.end(), "")
                    self.result.applied.append(f"removed unused selector {accessor} from {sel.span.path}")
        for macro, name in sorted(removed_classes):
            if re.search(rf"_{macro}_PRIVATE_CLS\(\s*{name}\s*\)", joined):
                continue
            span = self.index.class_defs.get(macro, {}).get(name)
            if span and span.path.split("/", 1)[0] in cpp_api.MINE_FOLDERS:
                text = self.editor.files[span.path]
                m = re.search(rf"^[ \t]*_{macro}_PRIVATE_DEF_CLS\(\s*{name}\s*\)\s*;[ \t]*\n", text, re.M)
                if m:
                    self.editor.add(span.path, m.start(), m.end(), "")
                    self.result.applied.append(f"removed unused class lookup {name} from {span.path}")
        self.editor.flush()

    def _cleanup_files(self, replacements):
        """Deletes mine files left without definitions and redirects includes to Apple's header."""
        for path in sorted({c.path for c in self.removed_mine}):
            text = self.editor.files[path]
            if not _is_empty_header(text):
                continue
            targets = list(dict.fromkeys(replacements.get(path, [])))
            self.result.deleted.append(path)
            del self.editor.files[path]
            for other, otext in list(self.editor.files.items()):
                if other.split("/", 1)[0] not in cpp_api.MINE_FOLDERS:
                    continue
                new = _redirect_include(other, otext, path, targets)
                if new != otext:
                    self.editor.files[other] = new
                    self.result.applied.append(
                        f"include of {path} in {other} -> {', '.join(targets) or 'removed'}")
            self.result.applied.append(f"deleted {path}")

    # Driver -----------------------------------------------------------------------------------------------------------

    def run(self, check_result):
        replacements = {}
        for overlap in check_result.overlaps:
            if overlap.pending:
                self.result.skipped.append(
                    f"{overlap.mine.qualified}: {len(overlap.pending)} signature conflict(s) need a decision")
                continue
            for action in overlap.actions:
                if action.type == "copy":
                    self.copy_member(action.mine, overlap.apple)
                elif action.type == "replace":
                    self.copy_member(action.mine, overlap.apple, replacing=action.apple)
            self.remove_class(overlap.mine)
            replacements.setdefault(overlap.mine.path, []).append(overlap.apple.path)
        for overlap in check_result.enum_overlaps:
            if overlap.pending:
                self.result.skipped.append(f"{overlap.mine.qualified}: enum value conflict(s) need a decision")
                continue
            for action in overlap.actions:
                if action.type == "enum_copy":
                    self.copy_enum_value(action)
            self.remove_span(overlap.mine)
            replacements.setdefault(overlap.mine.path, []).append(overlap.apple.path)
        for apple_const, mine_const in check_result.const_overlaps:
            self.remove_span(mine_const)
            replacements.setdefault(mine_const.path, []).append(apple_const.path)
        self.editor.flush()
        self._cleanup_private()
        self._cleanup_files(replacements)
        for path, text in self.editor.files.items():
            if self.editor.original.get(path) != text:
                self.result.changed[path] = text
        return self.result


_RE_DEFINITION = re.compile(r"_[A-Z0-9]+_(INLINE|ENUM|OPTIONS|CONST|PRIVATE_DEF_\w+)\b|^\s*class\s+\w+\s*:", re.M)


def _is_empty_header(text):
    masked = cpp_api._mask_comments(text)
    masked = re.sub(r"#define[^\n]*", "", masked)
    return not _RE_DEFINITION.search(masked) and not re.search(r"\b(struct|using|typedef)\b", masked)


def _redirect_include(including, text, removed, targets):
    rel_removed = os.path.relpath(removed, os.path.dirname(including))
    candidates = {rel_removed, Path(removed).name, removed, "/".join(Path(removed).parts[1:])}
    pattern = re.compile(r'^([ \t]*#include[ \t]*")([^"]+)(".*\n)', re.M)

    def sub(m):
        if m.group(2) not in candidates:
            return m.group(0)
        return "".join(f"{m.group(1)}{os.path.relpath(t, os.path.dirname(including))}{m.group(3)}" for t in targets)

    return pattern.sub(sub, text)


# Output -------------------------------------------------------------------------------------------------------------


def unified_diff(original: dict, result: ApplyResult) -> str:
    out = []
    for path in result.touched:
        old = original.get(path, "")
        new = "" if path in result.deleted else result.changed.get(path, old)
        out.extend(difflib.unified_diff(old.splitlines(keepends=True), new.splitlines(keepends=True),
                                        f"a/{path}", f"b/{path}" if path not in result.deleted else "/dev/null"))
    return "".join(out)


def write(repo: Path, result: ApplyResult, git=None):
    for path, text in result.changed.items():
        if path in result.deleted:
            continue
        (Path(repo) / path).write_text(text)
    for path in result.deleted:
        p = Path(repo) / path
        if git is not None:
            try:
                git.rm(path)
                continue
            except Exception:
                pass
        if p.exists():
            p.unlink()
        parent = p.parent
        while parent != Path(repo) and parent.exists() and not any(parent.iterdir()):
            parent.rmdir()
            parent = parent.parent


def apply(index, check_result, repo, dry_run=False, git=None):
    applier = Applier(index)
    result = applier.run(check_result)
    if not dry_run:
        write(repo, result, git)
    return result


# Merge conflicts ----------------------------------------------------------------------------------------------------


@dataclass
class ConflictResult:
    resolved: list = field(default_factory=list)        # Apple files taken from upstream
    saved: dict = field(default_factory=dict)           # path -> saved copy of our side
    reinjected: list = field(default_factory=list)      # human readable lines
    pending: list = field(default_factory=list)         # {"path", "reason"}
    apply: ApplyResult = None


def resolve_conflicts(git, api, repo, cache_dir, decisions, dry_run=False) -> ConflictResult:
    """Resolves merge conflicts in Apple folders by taking upstream's file and re-injecting our additions.

    Our side (stage 2) is saved under `<cache_dir>/conflicts/`. Members (and enum values) that exist on our side but
    neither in the merge base nor upstream are local additions: they are copied back when the SDK has them and they
    are not deprecated. Local signature changes that upstream also touched, delete/modify conflicts and conflicts in
    SylvaGX folders are left pending.
    """
    out = ConflictResult()
    apple_paths = []
    for path in git.unmerged_files():
        stages = git.unmerged_stages(path)
        if cpp_api._top(path) not in cpp_api.APPLE_FOLDERS:
            out.pending.append({"path": path, "reason": "conflict in a SylvaGX file: resolve by hand"})
        elif 2 not in stages or 3 not in stages:
            side = "deleted upstream, modified here" if 2 in stages else "modified upstream, deleted here"
            out.pending.append({"path": path, "reason": f"delete/modify conflict ({side})"})
        else:
            apple_paths.append(path)
    if not apple_paths:
        return out
    if dry_run:
        out.resolved = apple_paths
        return out

    folders = list(cpp_api.FRAMEWORKS)
    base_rev = git.merge_base()
    ours_index = cpp_api.CppIndex.from_git(git, "HEAD", folders)
    base_index = cpp_api.CppIndex.from_git(git, base_rev, folders)
    for path in apple_paths:
        saved = Path(cache_dir) / "conflicts" / path
        saved.parent.mkdir(parents=True, exist_ok=True)
        saved.write_text(git.show(f":2:{path}") or "")
        out.saved[path] = str(saved)
        git.checkout_theirs(path)
        out.resolved.append(path)

    new_index = cpp_api.CppIndex.from_disk(repo, folders)
    applier = Applier(new_index, source=ours_index)
    conflicted = set(apple_paths)

    for variants in ours_index.variants.values():
        for ours in variants:
            if ours.path not in conflicted:
                continue
            theirs = new_index.find_class(ours.qualified, ours.path)
            base = base_index.find_class(ours.qualified, ours.path)
            if theirs is None:
                if base is None:
                    out.pending.append({"path": ours.path,
                                        "reason": f"class {ours.qualified} added here was dropped by upstream's file"})
                continue
            objc = compare.objc_name(theirs, api, decisions)
            if objc is None:
                continue
            base_keys = base.by_key() if base else {}
            overlap = compare.compare_class(objc, theirs, ours, api, new_index, decisions)
            for action in overlap.actions:
                if action.type == "copy" and action.key not in base_keys:
                    if applier.copy_member(action.mine, theirs):
                        out.reinjected.append(f"{ours.qualified}::{action.mine.name} ({action.key})")
                elif action.type in ("signature_conflict", "replace") and action.key in base_keys:
                    old = base_keys[action.key][0]
                    if old.signature != action.mine.signature or old.returns != action.mine.returns:
                        out.pending.append({"path": ours.path, "reason":
                                            f"{ours.qualified}::{action.mine.name} was changed here and upstream: "
                                            f"{action.detail}"})

    for macro, accessors in ours_index.selectors.items():
        for accessor, sel in accessors.items():
            if (sel.span.path not in conflicted or (macro, accessor) in applier.added_sels
                    or base_index.selector_def(macro, accessor) is not None
                    or new_index.selector_def(macro, accessor) is not None):
                continue
            if _sorted_insert(applier.editor, sel.span.path, _RE_DEF_SEL_ENTRY, macro, accessor,
                              lambda tpl, name, s=sel.selector: _render_sel(tpl, name, s)):
                out.reinjected.append(f"selector {accessor} (\"{sel.selector}\") in {sel.span.path}")
    for macro, names in ours_index.class_defs.items():
        for name, span in names.items():
            if (span.path not in conflicted or (macro, name) in applier.added_cls
                    or name in base_index.class_defs.get(macro, {})
                    or name in new_index.class_defs.get(macro, {})):
                continue
            if _sorted_insert(applier.editor, span.path, _RE_DEF_CLS_ENTRY, macro, name, _render_cls):
                out.reinjected.append(f"class lookup {name} in {span.path}")

    theirs_enums = {e.qualified: e for e in new_index.enums if e.path in conflicted}
    base_enums = {e.qualified: e for e in base_index.enums if e.path in conflicted}
    for ours in ours_index.enums:
        theirs = theirs_enums.get(ours.qualified)
        if ours.path not in conflicted or theirs is None:
            continue
        base_values = base_enums[ours.qualified].values if ours.qualified in base_enums else {}
        for action in compare.compare_enum(theirs, ours, api).actions:
            if action.type == "enum_copy" and action.key not in base_values:
                applier.copy_enum_value(action)
                out.reinjected.append(f"{ours.qualified}::{action.key}")

    applier.editor.flush()
    for path, text in applier.editor.files.items():
        if applier.editor.original.get(path) != text:
            applier.result.changed[path] = text
    out.apply = applier.result
    write(repo, applier.result)
    git.add(*apple_paths, *[p for p in applier.result.changed if p not in conflicted])
    return out
