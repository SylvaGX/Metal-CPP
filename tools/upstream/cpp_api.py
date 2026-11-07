"""Extracts the API surface of the metal-cpp style C++ binding headers.

The headers follow a very regular layout (see Foundation/NSBundle.hpp): a class body with declarations, then
`_XX_INLINE` definitions that forward to `Object::sendMessage<>(target, _XX_PRIVATE_SEL(accessor), ...)`, with the
accessor -> selector mapping registered in the framework's private header via `_XX_PRIVATE_DEF_SEL`. Parsing is
regex/brace based and keeps exact source spans so members can later be copied or removed.
"""

import re
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class Framework:
    folder: str
    owner: str                  # "apple" or "mine"
    macro: str                  # NS, MTL, AK, ...
    namespaces: tuple
    selector_file: str          # file that holds the _XX_PRIVATE_DEF_SEL list
    class_file: str             # file that holds the _XX_PRIVATE_DEF_CLS list


FRAMEWORKS = {
    "Foundation": Framework("Foundation", "apple", "NS", ("NS",), "Foundation/NSPrivate.hpp", "Foundation/NSPrivate.hpp"),
    "Metal": Framework("Metal", "apple", "MTL", ("MTL", "MTL4"), "Metal/MTLHeaderBridge.hpp", "Metal/MTLHeaderBridge.hpp"),
    "MetalFX": Framework("MetalFX", "apple", "MTLFX", ("MTLFX", "MTL4FX"), "MetalFX/MTLFXPrivate.hpp", "MetalFX/MTLFXPrivate.hpp"),
    "QuartzCore": Framework("QuartzCore", "apple", "CA", ("CA",), "QuartzCore/CAPrivate.hpp", "QuartzCore/CAPrivate.hpp"),
    "AppKit": Framework("AppKit", "mine", "AK", ("NS",), "AppKit/AKPrivate.hpp", "AppKit/AKPrivate.hpp"),
    "MetalKit": Framework("MetalKit", "mine", "MTK", ("MTK",), "MetalKit/MTKPrivate.hpp", "MetalKit/MTKPrivate.hpp"),
    "ModelIO": Framework("ModelIO", "mine", "MDL", ("MDL",), "ModelIO/MDLPrivate.hpp", "ModelIO/MDLPrivate.hpp"),
}

APPLE_FOLDERS = tuple(f for f, fw in FRAMEWORKS.items() if fw.owner == "apple")
MINE_FOLDERS = tuple(f for f, fw in FRAMEWORKS.items() if fw.owner == "mine")
MACRO_TO_FRAMEWORK = {fw.macro: fw for fw in FRAMEWORKS.values()}

TEMPLATE_BASES = {"Referencing", "Copying", "SecureCoding", "Object"}


def framework_of(path: str):
    return FRAMEWORKS.get(Path(path).parts[0]) if path else None


def _top(path: str) -> str:
    return path.split("/", 1)[0] if path else ""


def _owner(path: str) -> str:
    fw = framework_of(path)
    return fw.owner if fw else ""


# Data model ---------------------------------------------------------------------------------------------------------


@dataclass
class Span:
    path: str
    start: int
    end: int

    def text(self, files):
        return files[self.path][self.start:self.end]


@dataclass
class Member:
    cls: str                    # qualified C++ class, e.g. NS::Bundle
    name: str
    params: str = ""
    returns: str = ""
    const: bool = False
    static: bool = False
    decl: Span = None
    decl_text: str = ""
    inline: Span = None
    inline_text: str = ""
    macro: str = ""             # macro prefix used by the inline body (NS, AK, ...)
    accessors: list = field(default_factory=list)
    selector: str = None        # primary ObjC selector, resolved through the private header
    objc_classes: list = field(default_factory=list)
    target: str = None          # "class", "instance" or None when unknown
    deprecated: bool = False    # declared with [[deprecated]]

    @property
    def key(self):
        if self.selector is None:
            return None
        return f"{'+' if self.static else '-'}{self.selector}"

    @property
    def signature(self):
        return f"{self.name}({normalize_params(self.params)}){' const' if self.const else ''}"


@dataclass
class CppClass:
    qualified: str
    namespace: str
    name: str
    path: str
    span: Span
    body: Span
    bases: list
    members: list = field(default_factory=list)

    @property
    def framework(self):
        return framework_of(self.path)

    def objc_candidates(self):
        names = [self.namespace + self.name]
        for m in self.members:
            if m.target == "class" or m.name == "alloc":
                names.extend(m.objc_classes)
        seen, out = set(), []
        for n in names:
            if n not in seen:
                seen.add(n)
                out.append(n)
        return out

    def by_key(self):
        result = {}
        for m in self.members:
            if m.key:
                result.setdefault(m.key, []).append(m)
        return result


@dataclass
class SelectorDef:
    macro: str
    accessor: str
    selector: str
    span: Span


@dataclass
class EnumDef:
    qualified: str
    namespace: str
    name: str
    macro: str
    kind: str                   # ENUM or OPTIONS
    type: str
    path: str
    span: Span
    values: dict = field(default_factory=dict)  # constant name -> value text


@dataclass
class ConstDef:
    qualified: str
    namespace: str
    name: str
    macro: str
    type: str
    path: str
    span: Span


# Lexical helpers ----------------------------------------------------------------------------------------------------


def _mask_comments(text: str) -> str:
    """Replaces comments and string literals with spaces (same length) so brace matching ignores them."""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            for k in range(i, j):
                out[k] = " "
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
        elif c == '"':
            j = i + 1
            while j < n and text[j] != '"':
                j += 2 if text[j] == "\\" else 1
            for k in range(i + 1, min(j, n)):
                out[k] = " "
            i = j + 1
        elif c == "'":
            j = i + 1
            while j < n and text[j] != "'":
                j += 2 if text[j] == "\\" else 1
            for k in range(i + 1, min(j, n)):
                out[k] = " "
            i = j + 1
        else:
            i += 1
    return "".join(out)


def _match_brace(masked: str, open_pos: int) -> int:
    """Index of the `}` matching the `{` at open_pos."""
    depth = 0
    for i in range(open_pos, len(masked)):
        ch = masked[i]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return i
    return len(masked) - 1


def _line_start(text: str, pos: int) -> int:
    return text.rfind("\n", 0, pos) + 1


def _line_end(text: str, pos: int) -> int:
    j = text.find("\n", pos)
    return len(text) if j < 0 else j + 1


def normalize_type(t: str) -> str:
    t = re.sub(r"\b(class|struct|enum|typename)\s+", "", t)
    t = re.sub(r"\b\w+::", "", t)
    return re.sub(r"\s+", "", t)


def split_params(params: str) -> list:
    params = params.strip()
    if not params or params == "void":
        return []
    out, depth, cur = [], 0, ""
    for ch in params:
        if ch in "<([":
            depth += 1
        elif ch in ">)]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def param_type(param: str) -> str:
    """`const class String* pName = nullptr` -> `const class String*`."""
    param = param.split("=", 1)[0].strip()
    m = re.match(r"^(.*?[\s\*&>])(\w+)(\s*\[[^\]]*\])?$", param)
    if m and m.group(1).strip() and not m.group(1).strip().endswith("::"):
        base = m.group(1).strip()
        if base not in ("const", "unsigned", "signed", "struct", "class", "enum"):
            return base + (m.group(3) or "")
    return param


def normalize_params(params: str) -> str:
    return ",".join(normalize_type(param_type(p)) for p in split_params(params))


# Parsing ------------------------------------------------------------------------------------------------------------

_RE_NAMESPACE = re.compile(r"\bnamespace\s+(\w+)\s*\{")
_RE_CLASS = re.compile(r"^[ \t]*class[ \t]+(\w+)[ \t]*(?:final[ \t]*)?:[ \t]*([^{;]+?)\s*\{", re.M)
_RE_INLINE = re.compile(r"^_([A-Z0-9]+)_INLINE\s+([^{;]*?)\s*\{", re.M)
_RE_INLINE_SIG = re.compile(r"^(.*?)\s*((?:\w+::)+)(~?\w+)\s*\((.*)\)\s*(const)?\s*$", re.S)
_RE_DECL = re.compile(
    r"^(?P<pre>(?:(?:static|virtual|inline|explicit|constexpr)\s+)*)(?P<ret>.*?)\s*\b(?P<name>~?\w+)\s*"
    r"\((?P<params>.*)\)\s*(?P<const>const)?\s*(?:=\s*\w+\s*)?$",
    re.S,
)
_RE_PRIVATE_SEL = re.compile(r"_([A-Z0-9]+)_PRIVATE_SEL\(\s*(\w+)\s*\)")
_RE_PRIVATE_CLS = re.compile(r"_([A-Z0-9]+)_PRIVATE_CLS\(\s*(\w+)\s*\)")
_RE_TARGET = re.compile(r"sendMessage(?:Safe)?\s*<.*?>\s*\(\s*(.+?)\s*,\s*_[A-Z0-9]+_PRIVATE_SEL", re.S)
_RE_ALLOC = re.compile(r"\balloc\s*<[^>]*>\s*\(\s*_([A-Z0-9]+)_PRIVATE_CLS\(\s*(\w+)\s*\)")
_RE_INIT = re.compile(r"\bObject::init\s*<")
_RE_DEF_SEL = re.compile(r"_([A-Z0-9]+)_PRIVATE_DEF_SEL\(\s*(\w+)\s*,\s*\"([^\"]*)\"\s*\)\s*;")
_RE_DEF_CLS = re.compile(r"_([A-Z0-9]+)_PRIVATE_DEF_CLS\(\s*(\w+)\s*\)\s*;")
_RE_ENUM = re.compile(r"^_([A-Z0-9]+)_(ENUM|OPTIONS)\(\s*([^,]+?)\s*,\s*(\w+)\s*\)\s*\{", re.M)
_RE_CONST = re.compile(r"^_([A-Z0-9]+)_(?:CONST|PRIVATE_DEF_STR|PRIVATE_DEF_CONST)\(\s*([^,]+?)\s*,\s*(\w+)\s*\)\s*;", re.M)


def _namespace_at(spans, pos):
    best = ""
    for start, end, name in spans:
        if start <= pos <= end:
            best = name
    return best


def _namespace_spans(masked):
    spans = []
    for m in _RE_NAMESPACE.finditer(masked):
        open_pos = m.end() - 1
        spans.append((open_pos, _match_brace(masked, open_pos), m.group(1)))
    return spans


class CppIndex:
    """Parsed view of a set of header files (path relative to the repo root -> text)."""

    def __init__(self, files: dict):
        self.files = dict(files)
        self.variants = {}          # qualified -> [CppClass] (one per defining file)
        self.selectors = {}         # macro -> {accessor: SelectorDef}
        self.class_defs = {}        # macro -> {objc class: Span}
        self.enums = []             # [EnumDef]
        self.constants = []         # [ConstDef]
        self.free_inlines = []      # inline functions that are not class members
        for path in sorted(self.files):
            if path.endswith(".hpp"):
                self._parse_private(path)
        for path in sorted(self.files):
            if path.endswith(".hpp"):
                self._parse_file(path)
        self._resolve_selectors()

    # Loading ----------------------------------------------------------------------------------------------------------

    @classmethod
    def from_disk(cls, repo: Path, folders):
        files = {}
        for folder in folders:
            for p in sorted((Path(repo) / folder).rglob("*.hpp")):
                files[p.relative_to(repo).as_posix()] = p.read_text(errors="replace")
        return cls(files)

    @classmethod
    def from_git(cls, git, rev, folders):
        files = {p: t for p, t in git.read_tree(rev, *folders).items() if p.endswith(".hpp")}
        return cls(files)

    # Private headers --------------------------------------------------------------------------------------------------

    def _parse_private(self, path):
        text = self.files[path]
        for m in _RE_DEF_SEL.finditer(text):
            macro, accessor, selector = m.groups()
            start = _line_start(text, m.start())
            self.selectors.setdefault(macro, {}).setdefault(
                accessor, SelectorDef(macro, accessor, selector, Span(path, start, _line_end(text, m.end() - 1)))
            )
        for m in _RE_DEF_CLS.finditer(text):
            macro, name = m.groups()
            start = _line_start(text, m.start())
            self.class_defs.setdefault(macro, {}).setdefault(name, Span(path, start, _line_end(text, m.end() - 1)))

    # Headers ----------------------------------------------------------------------------------------------------------

    def _parse_file(self, path):
        text = self.files[path]
        masked = _mask_comments(text)
        ns_spans = _namespace_spans(masked)

        for m in _RE_CLASS.finditer(masked):
            name = m.group(1)
            prev_line = masked[_line_start(masked, _line_start(masked, m.start()) - 1):_line_start(masked, m.start())]
            if name in TEMPLATE_BASES or "template" in prev_line:
                continue
            open_pos = m.end() - 1
            close = _match_brace(masked, open_pos)
            end = close + 1
            if masked[end:end + 1] == ";":
                end += 1
            ns = _namespace_at(ns_spans, m.start())
            qualified = f"{ns}::{name}" if ns else name
            if any(v.path == path for v in self.variants.get(qualified, [])):
                continue
            bases = [re.sub(r"\s*([<>,])\s*", r"\1", b).replace(",", ", ")
                     for b in split_params(re.sub(r"\b(public|protected|private|virtual)\b", "", m.group(2)))]
            start = _line_start(text, m.start())
            cls = CppClass(qualified, ns, name, path, Span(path, start, _line_end(text, end)),
                           Span(path, open_pos + 1, close), bases)
            self._parse_class_body(cls, text, masked, open_pos + 1, close)
            self.variants.setdefault(qualified, []).append(cls)

        for m in _RE_ENUM.finditer(masked):
            macro, kind, etype, name = m.groups()
            open_pos = m.end() - 1
            close = _match_brace(masked, open_pos)
            ns = _namespace_at(ns_spans, m.start())
            qualified = f"{ns}::{name}" if ns else name
            end = _line_end(text, close)
            values = {}
            for entry in masked[open_pos + 1:close].split(","):
                em = re.match(r"\s*(\w+)\s*(?:=\s*(.+?))?\s*$", entry, re.S)
                if em:
                    values[em.group(1)] = re.sub(r"\s+", " ", em.group(2) or "").strip()
            self.enums.append(EnumDef(
                qualified, ns, name, macro, kind, etype, path,
                Span(path, _line_start(text, m.start()), end), values))

        for m in _RE_CONST.finditer(masked):
            macro, ctype, name = m.groups()
            ns = _namespace_at(ns_spans, m.start())
            if not ns:
                ns = MACRO_TO_FRAMEWORK[macro].namespaces[0] if macro in MACRO_TO_FRAMEWORK else ""
            qualified = f"{ns}::{name}" if ns else name
            self.constants.append(ConstDef(
                qualified, ns, name, macro, ctype, path,
                Span(path, _line_start(text, m.start()), _line_end(text, m.end() - 1))))

        for m in _RE_INLINE.finditer(masked):
            macro, sig = m.group(1), m.group(2)
            sm = _RE_INLINE_SIG.match(sig.strip())
            open_pos = m.end() - 1
            close = _match_brace(masked, open_pos)
            span = Span(path, m.start(), _line_end(text, close))
            if not sm:
                continue
            ret, qual, name, params, const = sm.groups()
            qualifier = qual[:-2]
            cls = self.find_class(qualifier, path)
            if cls is None:
                if "::" not in qualifier:
                    self.free_inlines.append((qualifier + "::" + name, span))
                    continue
                cls = self._pending_class(qualifier, path)
            body = text[open_pos:close + 1]
            self._attach_inline(cls, macro, ret.strip(), name, params, bool(const), span, body)

    def _pending_class(self, qualifier, path):
        """Inline definitions whose class body was not found (templates, structs, ...)."""
        ns, _, name = qualifier.rpartition("::")
        for v in self.variants.get(qualifier, []):
            if not v.path and v.body is None and getattr(v, "_near", None) == _top(path):
                return v
        cls = CppClass(qualifier, ns, name, "", None, None, [])
        cls._near = _top(path)
        self.variants.setdefault(qualifier, []).append(cls)
        return cls

    def _parse_class_body(self, cls, text, masked, start, end):
        pos = start
        depth = 0
        stmt_start = start
        i = start
        while i < end:
            ch = masked[i]
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
            elif ch == ";" and depth == 0:
                self._add_decl(cls, text, masked, stmt_start, i + 1)
                stmt_start = i + 1
            elif ch == ":" and depth == 0:
                label = masked[stmt_start:i].strip()
                if label in ("public", "private", "protected"):
                    stmt_start = i + 1
            i += 1
        del pos

    def _add_decl(self, cls, text, masked, start, end):
        stmt = masked[start:end - 1].strip()
        deprecated = False
        while stmt.startswith("[["):
            close = stmt.find("]]")
            if close < 0:
                break
            deprecated = deprecated or "deprecated" in text[start:end]
            stmt = stmt[close + 2:].strip()
        if not stmt or "(" not in stmt:
            return
        if re.match(r"^(using|friend|typedef|enum|struct|class\s+\w+\s*;)", stmt):
            return
        m = _RE_DECL.match(stmt)
        if not m or m.group("name").startswith("operator") or "operator" in m.group("ret"):
            return
        real_start = start + (len(masked[start:end]) - len(masked[start:end].lstrip()))
        line_start = _line_start(text, real_start)
        if text[line_start:real_start].strip():
            line_start = real_start
        decl_end = _line_end(text, end - 1)
        cls.members.append(Member(
            cls=cls.qualified,
            name=m.group("name"),
            params=m.group("params").strip(),
            returns=m.group("ret").strip(),
            const=bool(m.group("const")),
            static="static" in m.group("pre"),
            decl=Span(cls.path, line_start, decl_end),
            decl_text=text[line_start:decl_end],
            deprecated=deprecated,
        ))

    def _attach_inline(self, cls, macro, ret, name, params, const, span, body):
        norm = normalize_params(params)
        candidates = [m for m in cls.members if m.name == name and m.inline is None]
        match = next((m for m in candidates if normalize_params(m.params) == norm and m.const == const), None)
        if match is None:
            match = next((m for m in candidates if normalize_params(m.params) == norm), None)
        if match is None and len(candidates) == 1 and not candidates[0].params and not norm:
            match = candidates[0]
        if match is None:
            match = Member(cls=cls.qualified, name=name, params=params.strip(), returns=ret, const=const)
            cls.members.append(match)
        match.inline = span
        match.inline_text = self.files[span.path][span.start:span.end]
        match.macro = macro
        match.accessors = [a for _, a in _RE_PRIVATE_SEL.findall(body)]
        match.objc_classes = [c for _, c in _RE_PRIVATE_CLS.findall(body)]
        alloc = _RE_ALLOC.search(body)
        target = _RE_TARGET.search(body)
        if alloc and not match.accessors:
            match.selector, match.target, match.static = "alloc", "class", True
            match.objc_classes = [alloc.group(2)]
        elif _RE_INIT.search(body) and not match.accessors:
            match.selector, match.target = "init", "instance"
        if target:
            arg = target.group(1).strip()
            if arg == "this":
                match.target = "instance"
            elif "_PRIVATE_CLS(" in arg:
                match.target = "class"
                match.static = True

    def _resolve_selectors(self):
        for variants in self.variants.values():
            for cls in variants:
                for m in cls.members:
                    if m.selector or not m.accessors:
                        continue
                    sel = self.selector_def(m.macro, m.accessors[0])
                    m.selector = sel.selector if sel else None

    # Queries ----------------------------------------------------------------------------------------------------------

    @property
    def classes(self):
        """qualified -> class, preferring Apple's definition when both sides define it."""
        out = {}
        for q, variants in self.variants.items():
            defined = [v for v in variants if v.path]
            pick = sorted(defined or variants, key=lambda v: (framework_of(v.path) is None or
                                                              framework_of(v.path).owner != "apple"))
            out[q] = pick[0]
        return out

    def find_class(self, qualified, near_path=None):
        """The variant of `qualified` closest to `near_path`: same file, same folder, same owner, then any."""
        variants = [v for v in self.variants.get(qualified, []) if v.path]
        if not variants:
            return None
        if near_path:
            for test in (lambda v: v.path == near_path,
                         lambda v: _top(v.path) == _top(near_path),
                         lambda v: _owner(v.path) == _owner(near_path)):
                hit = next((v for v in variants if test(v)), None)
                if hit is not None:
                    return hit
        return self.classes.get(qualified)

    def selector_def(self, macro, accessor):
        return self.selectors.get(macro, {}).get(accessor)

    def classes_in(self, folders):
        folders = tuple(folders)
        out = {}
        for q, variants in self.variants.items():
            for v in variants:
                if v.path and _top(v.path) in folders:
                    out.setdefault(q, v)
        return out

    def enums_in(self, folders):
        out = {}
        for e in self.enums:
            if _top(e.path) in tuple(folders):
                out.setdefault(e.qualified, e)
        return out

    def constants_in(self, folders):
        out = {}
        for c in self.constants:
            if _top(c.path) in tuple(folders):
                out.setdefault(c.qualified, c)
        return out

    def text(self, span: Span) -> str:
        return self.files[span.path][span.start:span.end]
