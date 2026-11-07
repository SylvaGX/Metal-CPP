"""Reconciliation rules: SDK (reference) vs Apple's metal-cpp classes vs SylvaGX classes.

For every ObjC class bound on both sides, each member (keyed by `+/-selector`) is classified:

    copy                in the SDK, missing in Apple's class, present in mine   -> copy mine into Apple's class
    missing_both        in the SDK, missing in both                             -> report
    mine_not_in_sdk     only in mine and not found in the SDK                   -> do not copy, report
    mine_deprecated     only in mine and deprecated/unavailable in the SDK      -> do not copy, report
    signature_conflict  same selector, Apple's signature disagrees with the SDK -> manual decision
    replace             signature_conflict decided as "mine" in decisions.json  -> replace Apple's with mine
    ok                  nothing to do
"""

import json
import re
from dataclasses import dataclass, field
from pathlib import Path

import cpp_api
from sdk_api import member_key

OBJC_PREFIXES = ("MTL4FX", "MTLFX", "MTL4", "MTL", "MTK", "MDL", "CA", "NS", "CG", "UI")

SCALARS = {
    "BOOL": "bool", "bool": "bool", "_Bool": "bool",
    "NSUInteger": "uint", "UInteger": "uint", "unsigned long": "uint", "size_t": "uint", "unsigned long long": "u64",
    "NSInteger": "int", "Integer": "int", "long": "int", "long long": "i64",
    "CGFloat": "double", "Float": "double", "double": "double", "NSTimeInterval": "double", "TimeInterval": "double",
    "CFTimeInterval": "double", "float": "float", "int": "i32", "unsigned int": "u32",
    "uint64_t": "u64", "int64_t": "i64", "uint32_t": "u32", "int32_t": "i32", "uint16_t": "u16", "int16_t": "i16",
    "uint8_t": "u8", "int8_t": "i8", "char": "char", "unichar": "u16",
    "SEL": "sel", "Class": "class", "void": "void",
}

DEFAULT_TYPE_ALIASES = {
    "NotificationName": "obj:String",
    "ErrorDomain": "obj:String",
    "ErrorUserInfoKey": "obj:String",
    "RunLoopMode": "obj:String",
    "Rect": "Rect", "Size": "Size", "Point": "Point",
}

DECISIONS_TEMPLATE = {
    "members": {},
    "class_map": {},
    "type_aliases": {},
    "ignore_classes": [],
}


# Decisions ----------------------------------------------------------------------------------------------------------


class Decisions:
    def __init__(self, path: Path):
        self.path = Path(path)
        data = json.loads(self.path.read_text()) if self.path.exists() else {}
        self.data = {**DECISIONS_TEMPLATE, **data}

    def member(self, objc_class, key):
        return self.data["members"].get(f"{objc_class} {key}")

    def class_map(self):
        return self.data["class_map"]

    def type_aliases(self):
        return {**DEFAULT_TYPE_ALIASES, **self.data["type_aliases"]}

    def ignored(self, qualified):
        return qualified in self.data["ignore_classes"]


# Type canonicalisation ----------------------------------------------------------------------------------------------


def strip_prefix(name: str) -> str:
    for p in OBJC_PREFIXES:
        if name.startswith(p) and len(name) > len(p) and (name[len(p)].isupper() or name[len(p)].isdigit()):
            return name[len(p):]
    return name


VALUE_TYPES = ("Rect", "Size", "Point", "Range", "Origin", "Region")
OBJC_GENERICS = ("ObjectType", "KeyType", "ValueType", "ElementType", "UnitType")


def _strip_generics(t: str) -> str:
    prev = None
    while prev != t:
        prev, t = t, re.sub(r"<[^<>]*>", "", t)
    return t


def _canon_base(base: str, aliases) -> str:
    base = base.strip()
    if base.startswith("simd_"):
        base = base[5:]
    if base in SCALARS:
        return SCALARS[base]
    short = strip_prefix(base)
    if short in SCALARS:
        return SCALARS[short]
    if short in aliases:
        return aliases[short]
    if re.search(r"(Handler|Block)$", short):
        return "block"
    return short


def _canon_pointer(base: str, canon: str, stars: int) -> str:
    if canon.startswith("obj"):
        result = "ptr:" * stars + canon
    elif stars == 0:
        result = canon
    elif canon == "void":
        result = "ptr:" * (stars - 1) + "any"
    elif canon in SCALARS.values() or canon == "block" or canon in VALUE_TYPES:
        result = "ptr:" * stars + canon
    elif base[:1].isupper():
        result = "ptr:" * (stars - 1) + "obj:" + canon
    else:
        result = "ptr:" * stars + canon
    return result.replace("obj:Object", "obj")


def canon_objc(t: str, aliases) -> str:
    t = re.sub(r"\b[A-Z]+_[A-Z0-9_]+\b(\([^)]*\))?", " ", t)
    t = re.sub(r"\b(const|__strong|__weak|__autoreleasing|__unsafe_unretained|_Nonnull|_Nullable|__kindof|struct|enum)\b", " ", t)
    if "^" in t:
        return "block"
    m = re.match(r"^\s*id\s*<\s*(\w+)\s*>\s*(\**)\s*(\[\])?\s*$", t)
    if m:
        depth = len(m.group(2)) + (1 if m.group(3) else 0)
        return ("ptr:" * depth + "obj:" + strip_prefix(m.group(1))).replace("obj:Object", "obj")
    t = _strip_generics(t)
    arrays = t.count("[]")
    t = t.replace("[]", "")
    stars = t.count("*") + arrays
    base = re.sub(r"\s+", " ", t.replace("*", " ")).strip()
    if base in ("id", "instancetype") or base in OBJC_GENERICS:
        return "ptr:" * stars + "obj"
    return _canon_pointer(base, _canon_base(base, aliases), stars)


def canon_cpp(t: str, aliases) -> str:
    t = re.sub(r"^\s*template\s*<[^>]*>\s*", "", t)
    if "^" in t or "std::function" in t or re.search(r"\bfunction\s*<", t):
        return "block"
    t = re.sub(r"\b(const|class|struct|enum|typename|volatile)\b", " ", t)
    t = re.sub(r"\b\w+::", "", t)
    t = _strip_generics(t)
    arrays = t.count("[]")
    t = t.replace("[]", "").replace("&", "")
    stars = t.count("*") + arrays
    base = re.sub(r"\s+", " ", t.replace("*", " ")).strip()
    if base.startswith("_"):
        return "ptr:" * max(stars - 1, 0) + "any"
    return _canon_pointer(base, _canon_base(base, aliases), stars)


def types_match(objc: str, cpp: str) -> bool:
    if objc == cpp or "any" in (objc, cpp):
        return True
    # `id` / generics match any object type; `Object*` in C++ matches any ObjC object.
    if objc.endswith("obj") and cpp.startswith(objc) or cpp.endswith("obj") and objc.startswith(cpp):
        return True
    if cpp.endswith("any") and objc.startswith(cpp[:-3]):
        return True
    if objc.endswith("block") and cpp.endswith("block"):
        return True
    # Opaque CoreFoundation/IOSurface references bound as void* in C++.
    if objc.endswith("Ref") and cpp.endswith("any"):
        return True
    return False


def signature_issues(sdk_member, cpp_member, aliases) -> list:
    """Differences between an SDK member and a C++ member. Empty list when they agree."""
    issues = []
    params = [cpp_api.param_type(p) for p in cpp_api.split_params(cpp_member.params)]
    sdk_params = [p[1] for p in sdk_member["params"]]
    if len(params) != len(sdk_params) and not sdk_member.get("variadic"):
        issues.append(f"{len(params)} parameter(s), SDK has {len(sdk_params)}")
        return issues
    ret_objc = canon_objc(sdk_member["returns"], aliases)
    ret_cpp = canon_cpp(cpp_member.returns, aliases)
    if cpp_member.name not in ("alloc", "init") and not cpp_member.name.startswith("init"):
        if not types_match(ret_objc, ret_cpp):
            issues.append(f"returns {cpp_member.returns.strip()} ({ret_cpp}), SDK returns {sdk_member['returns']} ({ret_objc})")
    for i, (sp, cp) in enumerate(zip(sdk_params, params)):
        a, b = canon_objc(sp, aliases), canon_cpp(cp, aliases)
        if not types_match(a, b):
            issues.append(f"parameter {i + 1} is {cp} ({b}), SDK has {sp} ({a})")
    return issues


# Results ------------------------------------------------------------------------------------------------------------


@dataclass
class Action:
    type: str
    objc_class: str
    cpp_class: str
    key: str = None
    detail: str = ""
    mine: object = None             # cpp_api.Member / EnumDef / ConstDef
    apple: object = None
    extra: dict = field(default_factory=dict)

    def to_dict(self):
        def describe(m):
            if m is None:
                return None
            if isinstance(m, cpp_api.Member):
                path = (m.decl or m.inline).path if (m.decl or m.inline) else ""
                return {"signature": m.signature, "path": path}
            return {"name": getattr(m, "qualified", str(m)), "path": getattr(m, "path", "")}
        return {
            "type": self.type, "objc_class": self.objc_class, "cpp_class": self.cpp_class, "key": self.key,
            "detail": self.detail, "mine": describe(self.mine), "apple": describe(self.apple), **self.extra,
        }


@dataclass
class ClassOverlap:
    objc_class: str
    apple: cpp_api.CppClass
    mine: cpp_api.CppClass
    actions: list = field(default_factory=list)

    @property
    def pending(self):
        return [a for a in self.actions if a.type == "signature_conflict"]


@dataclass
class EnumOverlap:
    qualified: str
    apple: cpp_api.EnumDef
    mine: cpp_api.EnumDef
    actions: list = field(default_factory=list)

    @property
    def pending(self):
        return [a for a in self.actions if a.type == "enum_value_conflict"]


@dataclass
class CheckResult:
    overlaps: list = field(default_factory=list)
    enum_overlaps: list = field(default_factory=list)
    const_overlaps: list = field(default_factory=list)   # list[(apple ConstDef, mine ConstDef)]
    removals: list = field(default_factory=list)
    additions: dict = field(default_factory=dict)
    audit: list = field(default_factory=list)
    unmapped: list = field(default_factory=list)
    range: tuple = (None, None)

    def actions(self):
        for o in self.overlaps:
            yield from o.actions
        for o in self.enum_overlaps:
            yield from o.actions

    def pending(self):
        out = [a for o in self.overlaps for a in o.pending]
        out += [a for o in self.enum_overlaps for a in o.pending]
        out += [r for r in self.removals if r.get("usages")]
        return out


# Class mapping ------------------------------------------------------------------------------------------------------


def objc_name(cls: cpp_api.CppClass, api, decisions) -> str:
    mapped = decisions.class_map().get(cls.qualified)
    if mapped:
        return mapped
    candidates = cls.objc_candidates()
    for c in candidates:
        if c in api.classes:
            return c
    return None


def _base_classes(cls, index):
    """C++ base classes, from `Referencing<Self, Base>` or a plain `Base`, resolved next to `cls`."""
    out = []
    for b in cls.bases:
        m = re.match(r"^(?:\w+::)?(?:Referencing|Copying|SecureCoding)\s*<\s*[\w:]+\s*(?:,\s*([\w:]+)\s*)?>$", b.strip())
        name = m.group(1) if m else (None if re.match(r"^(?:\w+::)?(?:Referencing|Copying|SecureCoding)\b", b) else b)
        if not name:
            continue
        qualified = name if "::" in name else f"{cls.namespace}::{name}"
        base = index.find_class(qualified, cls.path)
        if base is not None:
            out.append(base)
    return out


def _inherited_keys(cls, index, seen=None) -> set:
    seen = seen if seen is not None else set()
    keys = set()
    for bcls in _base_classes(cls, index):
        if id(bcls) in seen:
            continue
        seen.add(id(bcls))
        keys |= {m.key for m in bcls.members if m.key}
        keys |= _inherited_keys(bcls, index, seen)
    return keys


# Rules --------------------------------------------------------------------------------------------------------------


def _lookup(api, objc, key):
    owner, sm = api.find_member(objc, key)
    if sm is None:
        flipped = ("+" if key[0] == "-" else "-") + key[1:]
        owner, sm = api.find_member(objc, flipped)
        if sm is not None:
            return owner, sm, flipped
    return owner, sm, key


def compare_class(objc, apple, mine, api, index, decisions) -> ClassOverlap:
    overlap = ClassOverlap(objc, apple, mine)
    aliases = decisions.type_aliases()
    apple_keys = apple.by_key()
    mine_keys = mine.by_key()
    apple_inherited = _inherited_keys(apple, index)
    add = overlap.actions.append

    for key, mine_members in mine_keys.items():
        mm = mine_members[0]
        decision = decisions.member(objc, key)
        if decision == "skip":
            continue
        owner, sm, sdk_key = _lookup(api, objc, key)
        if sm is None:
            add(Action("mine_not_in_sdk", objc, apple.qualified, key, "selector not found in the SDK", mine=mm))
            continue
        status = api.member_status(owner, sdk_key)
        if status["status"] != "ok":
            add(Action("mine_deprecated", objc, apple.qualified, key,
                       f"{status['status']} on {', '.join(status['deprecated_on'] or status['platforms']) or 'all platforms'}",
                       mine=mm))
            continue
        if key not in apple_keys and key in apple_inherited:
            continue
        if key not in apple_keys:
            for m in mine_members:
                add(Action("copy", objc, apple.qualified, key, "missing in Apple's class", mine=m,
                           extra={"sdk_owner": owner}))
            continue
        am = apple_keys[key][0]
        if am.signature == mm.signature and cpp_api.normalize_type(am.returns) == cpp_api.normalize_type(mm.returns):
            continue
        apple_issues = signature_issues(sm, am, aliases)
        mine_issues = signature_issues(sm, mm, aliases)
        if decision == "apple":
            continue
        if decision == "mine":
            add(Action("replace", objc, apple.qualified, key, "decided: keep mine", mine=mm, apple=am))
            continue
        if not apple_issues:
            continue
        add(Action("signature_conflict", objc, apple.qualified, key,
                   "Apple: " + "; ".join(apple_issues) + (" | mine: " + "; ".join(mine_issues) if mine_issues else " | mine matches the SDK"),
                   mine=mm, apple=am))

    inherited = apple_inherited | _inherited_keys(mine, index)
    sdk_cls = api.classes.get(objc)
    if sdk_cls:
        for key, sm in sdk_cls["members"].items():
            if key in apple_keys or key in mine_keys or key in inherited:
                continue
            if decisions.member(objc, key) == "skip":
                continue
            if api.member_status(objc, key)["status"] != "ok":
                continue
            add(Action("missing_both", objc, apple.qualified, key, sm["kind"]))
    return overlap


def compare_enum(apple, mine, api) -> EnumOverlap:
    overlap = EnumOverlap(apple.qualified, apple, mine)
    prefix = apple.namespace
    sdk_enum = api.enums.get(prefix + apple.name)
    for name, value in mine.values.items():
        sdk_const = sdk_enum["constants"].get(prefix + name) if sdk_enum else None
        if name in apple.values:
            mine_v, apple_v = _eval(value), _eval(apple.values[name])
            if mine_v is not None and apple_v is not None and not _same_value(mine_v, apple_v):
                sdk_value = sdk_const["value"] if sdk_const else None
                detail = f"Apple = {apple.values[name]}, mine = {value}, SDK = {sdk_value}"
                if sdk_value is not None and _same_value(apple_v, sdk_value):
                    continue
                overlap.actions.append(Action("enum_value_conflict", prefix + apple.name, apple.qualified, name, detail,
                                              mine=mine, apple=apple))
            continue
        if sdk_enum is not None and sdk_const is None:
            overlap.actions.append(Action("mine_not_in_sdk", prefix + apple.name, apple.qualified, name,
                                          "enum constant not found in the SDK", mine=mine))
            continue
        if sdk_const is not None and api.status(sdk_const["avail"], sdk_const["sdks"], sdk_enum["avail"])["status"] != "ok":
            overlap.actions.append(Action("mine_deprecated", prefix + apple.name, apple.qualified, name,
                                          "enum constant deprecated in the SDK", mine=mine))
            continue
        overlap.actions.append(Action("enum_copy", prefix + apple.name, apple.qualified, name,
                                      f"= {value}" if value else "", mine=mine, apple=apple, extra={"value": value}))
    return overlap


def _same_value(a, b) -> bool:
    return a == b or (a % 2 ** 64) == (b % 2 ** 64)


def _eval(expr):
    if expr is None:
        return None
    expr = expr.strip()
    if not expr:
        return None
    if not re.fullmatch(r"[0-9xXa-fA-FuUlL\s\(\)\|<>&~\-+]+", expr):
        return None
    try:
        return int(eval(re.sub(r"(?<=[0-9a-fA-F])[uUlL]+", "", expr), {"__builtins__": {}}))
    except Exception:
        return None


def check(api, index, decisions) -> CheckResult:
    result = CheckResult()
    apple = index.classes_in(cpp_api.APPLE_FOLDERS)
    mine = index.classes_in(cpp_api.MINE_FOLDERS)

    apple_by_objc = {}
    for q, c in apple.items():
        name = objc_name(c, api, decisions)
        if name:
            apple_by_objc.setdefault(name, c)
    for q, c in mine.items():
        if decisions.ignored(q):
            continue
        name = objc_name(c, api, decisions)
        other = apple.get(q) or (apple_by_objc.get(name) if name else None)
        if other is None:
            continue
        name = name or objc_name(other, api, decisions) or (other.namespace + other.name)
        result.overlaps.append(compare_class(name, other, c, api, index, decisions))

    apple_enums = index.enums_in(cpp_api.APPLE_FOLDERS)
    for q, e in index.enums_in(cpp_api.MINE_FOLDERS).items():
        if q in apple_enums:
            result.enum_overlaps.append(compare_enum(apple_enums[q], e, api))

    apple_consts = index.constants_in(cpp_api.APPLE_FOLDERS)
    for q, c in index.constants_in(cpp_api.MINE_FOLDERS).items():
        if q in apple_consts:
            result.const_overlaps.append((apple_consts[q], c))
    return result


# Audit --------------------------------------------------------------------------------------------------------------


def audit(api, index, decisions, folders) -> list:
    """Compares bound classes, enums and constants in `folders` against the SDK."""
    aliases = decisions.type_aliases()
    out = []
    classes = index.classes_in(folders)
    for q, cls in sorted(classes.items()):
        if decisions.ignored(q):
            continue
        entry = {"cpp_class": q, "path": cls.path, "objc_class": None, "status": "ok", "items": []}
        out.append(entry)
        objc = objc_name(cls, api, decisions)
        if objc is None:
            entry["status"] = "unmapped"
            continue
        entry["objc_class"] = objc
        cstatus = api.class_status(objc)
        if cstatus["status"] != "ok":
            entry["status"] = f"class {cstatus['status']}"
        keys = cls.by_key()
        for key, members in sorted(keys.items()):
            if decisions.member(objc, key) == "skip":
                continue
            owner, sm, sdk_key = _lookup(api, objc, key)
            if sm is None:
                entry["items"].append({"type": "not_in_sdk", "key": key, "signature": members[0].signature})
                continue
            if sdk_key != key:
                entry["items"].append({"type": "static_mismatch", "key": key, "detail": f"SDK declares {sdk_key}"})
            status = api.member_status(owner, sdk_key)
            if status["status"] != "ok":
                entry["items"].append({"type": status["status"], "key": key, "signature": members[0].signature,
                                       "detail": ", ".join(status["deprecated_on"])})
            for m in members:
                issues = signature_issues(sm, m, aliases)
                if issues:
                    entry["items"].append({"type": "signature_mismatch", "key": key, "signature": m.signature,
                                           "detail": "; ".join(issues)})
        sdk_cls = api.classes.get(objc)
        inherited = _inherited_keys(cls, index)
        if sdk_cls:
            for key in sorted(sdk_cls["members"]):
                if key in keys or key in inherited or decisions.member(objc, key) == "skip":
                    continue
                if api.member_status(objc, key)["status"] == "ok":
                    entry["items"].append({"type": "missing", "key": key})

    for q, e in sorted(index.enums_in(folders).items()):
        sdk_enum = api.enums.get(e.namespace + e.name)
        entry = {"cpp_enum": q, "path": e.path, "items": []}
        out.append(entry)
        if sdk_enum is None:
            entry["status"] = "not_in_sdk"
            continue
        entry["status"] = "ok"
        for name, value in e.values.items():
            sc = sdk_enum["constants"].get(e.namespace + name)
            if sc is None:
                entry["items"].append({"type": "not_in_sdk", "key": name})
                continue
            if api.status(sc["avail"], sc["sdks"], sdk_enum["avail"])["status"] != "ok":
                entry["items"].append({"type": "deprecated", "key": name})
            v = _eval(value)
            if v is not None and not _same_value(v, sc["value"]):
                entry["items"].append({"type": "value_mismatch", "key": name, "detail": f"{value} != SDK {sc['value']}"})
        for cname, sc in sdk_enum["constants"].items():
            short = cname[len(e.namespace):] if cname.startswith(e.namespace) else cname
            if short not in e.values and api.status(sc["avail"], sc["sdks"], sdk_enum["avail"])["status"] == "ok":
                entry["items"].append({"type": "missing", "key": short})

    for q, c in sorted(index.constants_in(folders).items()):
        sdk_const = api.constants.get(c.namespace + c.name)
        status = "not_in_sdk" if sdk_const is None else api.status(sdk_const["avail"], sdk_const["sdks"])["status"]
        if status != "ok":
            out.append({"cpp_constant": q, "path": c.path, "status": status, "items": []})
    return out


# Upstream removals --------------------------------------------------------------------------------------------------


def _member_ids(cls):
    return {m.signature: m for m in cls.members}


def _names_with_bases(cls, index, seen=None):
    seen = seen if seen is not None else set()
    names = set()
    for base in _base_classes(cls, index):
        if id(base) not in seen:
            seen.add(id(base))
            names |= {m.name for m in base.members}
            names |= _names_with_bases(base, index, seen)
    return names


def _removed_members(old_cls, new_cls, new_index):
    """Yields (kind, member): member_removed, member_changed (same name, new signature) or member_moved (to a base)."""
    new_sigs = {m.signature for m in new_cls.members}
    new_names = {m.name for m in new_cls.members}
    new_keys = {m.key for m in new_cls.members if m.key}
    base_names = None
    for m in old_cls.members:
        if m.signature in new_sigs or (m.key and m.key in new_keys):
            continue
        if m.name in new_names:
            yield "member_changed", m
            continue
        if base_names is None:
            base_names = _names_with_bases(new_cls, new_index)
        yield ("member_moved" if m.name in base_names else "member"), m


def diff_upstream(old: cpp_api.CppIndex, new: cpp_api.CppIndex):
    """Classes/members/enums/constants removed and added between two Apple trees."""
    old_c, new_c = old.classes_in(cpp_api.APPLE_FOLDERS), new.classes_in(cpp_api.APPLE_FOLDERS)
    removed, added = [], {"classes": [], "members": 0, "enums": [], "constants": []}
    for q, cls in old_c.items():
        if q not in new_c:
            removed.append({"kind": "class", "name": q, "path": cls.path})
            continue
        for kind, m in _removed_members(cls, new_c[q], new):
            if kind == "member_moved":
                continue
            removed.append({"kind": kind, "name": f"{q}::{m.name}", "member": m.name, "class": q,
                            "key": m.key, "signature": m.signature, "path": cls.path})
    for q, cls in new_c.items():
        if q not in old_c:
            added["classes"].append(q)
        else:
            added["members"] += len(set(_member_ids(cls)) - set(_member_ids(old_c[q])))
    old_e, new_e = old.enums_in(cpp_api.APPLE_FOLDERS), new.enums_in(cpp_api.APPLE_FOLDERS)
    for q, e in old_e.items():
        if q not in new_e:
            removed.append({"kind": "enum", "name": q, "path": e.path})
            continue
        for name in e.values:
            if name not in new_e[q].values:
                removed.append({"kind": "enum_value", "name": f"{e.namespace}::{name}", "path": e.path})
    added["enums"] = [q for q in new_e if q not in old_e]
    old_k, new_k = old.constants_in(cpp_api.APPLE_FOLDERS), new.constants_in(cpp_api.APPLE_FOLDERS)
    for q, c in old_k.items():
        if q not in new_k:
            removed.append({"kind": "constant", "name": q, "path": c.path})
    added["constants"] = [q for q in new_k if q not in old_k]
    return removed, added


def find_usages(removed, repo: Path, folders) -> list:
    """Attaches `usages` (file:line) of removed API found in the given folders."""
    files = []
    for folder in folders:
        root = Path(repo) / folder
        if root.exists():
            files += [p for p in root.rglob("*") if p.suffix in (".hpp", ".h", ".cpp", ".mm", ".m")]
    contents = {p: p.read_text(errors="replace").splitlines() for p in files}
    for r in removed:
        if r["kind"] in ("member", "member_changed"):
            ns_class = r["class"].rsplit("::", 1)[-1]
            pattern = re.compile(rf"(->|\.|::)\s*{re.escape(r['member'])}\s*\(")
            cls_pattern = re.compile(rf"\b{re.escape(ns_class)}\b")
        else:
            short = r["name"].rsplit("::", 1)[-1]
            pattern = re.compile(rf"\b{re.escape(short)}\b")
            cls_pattern = None
        hits = []
        for p, lines in contents.items():
            if cls_pattern is not None and not any(cls_pattern.search(l) for l in lines):
                continue
            for i, line in enumerate(lines, 1):
                if pattern.search(line):
                    hits.append(f"{p.relative_to(repo).as_posix()}:{i}")
        r["usages"] = hits[:20]
    return removed
