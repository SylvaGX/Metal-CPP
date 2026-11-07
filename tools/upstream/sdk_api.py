"""Extracts the Objective-C API surface of the Apple SDK headers.

The SDK headers are what the developer documentation is generated from, so they are the reference used to decide
which members a binding class should have. The headers are parsed by clang (`-ast-dump`, text format: the JSON
format does not carry availability details) and reduced to a small JSON model that is cached per SDK version.
"""

import json
import re
import subprocess
import tempfile
from pathlib import Path

SDK_FRAMEWORKS = {
    "macosx": ["Foundation", "AppKit", "Metal", "MetalKit", "ModelIO", "QuartzCore", "MetalFX"],
    "iphoneos": ["Foundation", "Metal", "MetalKit", "ModelIO", "QuartzCore", "MetalFX"],
}

SDK_PLATFORM = {"macosx": "macos", "iphoneos": "ios"}

CACHE_FORMAT = 3


class SdkError(RuntimeError):
    pass


# SDK discovery ------------------------------------------------------------------------------------------------------


def _xcrun(*args) -> str:
    proc = subprocess.run(["xcrun", *args], capture_output=True, text=True)
    if proc.returncode != 0:
        raise SdkError(proc.stderr.strip().splitlines()[-1] if proc.stderr.strip() else f"xcrun {' '.join(args)} failed")
    return proc.stdout.strip()


def sdk_version(sdk: str) -> str:
    return _xcrun("--sdk", sdk, "--show-sdk-version")


def sdk_path(sdk: str) -> str:
    return _xcrun("--sdk", sdk, "--show-sdk-path")


def require_sdk(sdk: str, min_major: int) -> str:
    try:
        version = sdk_version(sdk)
    except SdkError as exc:
        raise SdkError(
            f"SDK '{sdk}' not found ({exc}).\n"
            "Install Xcode and select it with: sudo xcode-select -s /Applications/Xcode.app"
        )
    major = int(version.split(".")[0])
    if major < min_major:
        raise SdkError(
            f"SDK '{sdk}' is {version}, but {min_major} or newer is required.\n"
            "Install a newer Xcode and select it with: sudo xcode-select -s /Applications/Xcode.app"
        )
    return version


# Text AST parsing ---------------------------------------------------------------------------------------------------

_RE_TREE = re.compile(r"^((?:[| ] )*)[|`]-")
_RE_PATH = re.compile(r"(/[^<>:,'\s]+):\d+:\d+")
_RE_FRAMEWORK = re.compile(r"/(\w+)\.framework/(?:Versions/[^/]+/)?Headers/")
_RE_LOC = re.compile(
    r"^(?:line:\d+:\d+|col:\d+|/[^\s]+:\d+:\d+|<invalid sloc>|<scratch space>:\d+:\d+|<built-in>:\d+:\d+)\s*"
)
_RE_TYPES = re.compile(r"'([^']*)'(?::'([^']*)')?")
_RE_METHOD = re.compile(r"(?:^|\s)([+-]) (\S+) '([^']*)'(?::'([^']*)')?")
_RE_NAMED_TYPED = re.compile(r"(\w+) '([^']*)'(?::'([^']*)')?(.*)$")
_RE_AVAIL = re.compile(r"^(?:(?:Implicit|Inherited) )*(\w+) (\S+) (\S+) (\S+)( Unavailable)?")
_RE_VALUE = re.compile(r"^value: Int (-?\d+)")
_AVAIL_KINDS = ("AvailabilityAttr", "UnavailableAttr", "DeprecatedAttr")


def _split_node(content: str):
    """Splits `Kind 0xADDR [prev 0x..] <range> loc rest` into (kind, location_text, rest)."""
    kind, _, tail = content.partition(" ")
    if not tail.startswith("0x"):
        return kind, "", tail
    _, _, tail = tail.partition(" ")
    while tail.startswith(("prev 0x", "parent 0x")):
        parts = tail.split(" ", 2)
        tail = parts[2] if len(parts) == 3 else ""
    location = ""
    if tail.startswith("<"):
        depth, i = 0, 0
        for i, ch in enumerate(tail):
            if ch == "<":
                depth += 1
            elif ch == ">":
                depth -= 1
                if depth == 0:
                    break
        location, tail = tail[: i + 1], tail[i + 1:].lstrip()
    m = _RE_LOC.match(tail)
    if m:
        location += " " + m.group(0)
        tail = tail[m.end():]
    return kind, location, tail


def _empty_avail():
    return {"attrs": {}, "unavailable": False, "deprecated": False}


def _add_avail(avail, rest, kind):
    if kind == "UnavailableAttr":
        avail["unavailable"] = True
        return
    if kind == "DeprecatedAttr":
        avail["deprecated"] = True
        return
    m = _RE_AVAIL.match(rest)
    if not m:
        return
    platform, introduced, deprecated, obsoleted, unavailable = m.groups()
    if platform not in ("macos", "ios"):
        return
    avail["attrs"][platform] = {
        "introduced": introduced,
        "deprecated": deprecated,
        "obsoleted": obsoleted,
        "unavailable": bool(unavailable),
    }


def _merge_avail(outer, inner):
    """Member availability: the member's own attributes override the container's, per platform."""
    attrs = dict(outer["attrs"])
    attrs.update(inner["attrs"])
    return {
        "attrs": attrs,
        "unavailable": outer["unavailable"] or inner["unavailable"],
        "deprecated": outer["deprecated"] or inner["deprecated"],
    }


def _strip_nullability(t: str) -> str:
    t = re.sub(r"\b(_Nonnull|_Nullable|_Null_unspecified|__nonnull|__nullable|__kindof)\b", "", t)
    return re.sub(r"\s+", " ", t).replace(" *", "*").strip()


def _property_members(prop):
    name = prop["name"]
    getter = prop["getter"] or name
    members = [{
        "kind": "getter", "static": prop["static"], "selector": getter, "property": name,
        "returns": prop["type"], "params": [], "avail": prop["avail"], "framework": prop["framework"],
    }]
    if not prop["readonly"]:
        setter = prop["setter"] or f"set{name[0].upper()}{name[1:]}:"
        members.append({
            "kind": "setter", "static": prop["static"], "selector": setter, "property": name,
            "returns": "void", "params": [[name, prop["type"]]], "avail": prop["avail"],
            "framework": prop["framework"],
        })
    return members


def member_key(static: bool, selector: str) -> str:
    return f"{'+' if static else '-'}{selector}"


class _Parser:
    def __init__(self, frameworks):
        self.frameworks = set(frameworks) | {"objc"}
        self.classes, self.enums, self.constants = {}, {}, {}
        self.container = None

    def _class(self, name, kind, framework):
        return self.classes.setdefault(name, {
            "kind": kind, "framework": framework, "super": None, "protocols": [],
            "avail": _empty_avail(), "members": {},
        })

    def _close_container(self):
        c, self.container = self.container, None
        if c is None or c["target"] is None:
            return
        target = c["target"]
        if c["super"]:
            target["super"] = c["super"]
        if not c["is_category"] and c["is_definition"]:
            target["framework"] = c["framework"]
            target["avail"] = _merge_avail(target["avail"], c["avail"])
        for p in c["protocols"]:
            if p not in target["protocols"]:
                target["protocols"].append(p)
        for raw in c["members"]:
            items = _property_members(raw) if raw["kind"] == "property" else [raw]
            for m in items:
                m["avail"] = _merge_avail(c["avail"], m["avail"])
                key = member_key(m["static"], m["selector"])
                target["members"].setdefault(key, m)

    def parse(self, lines):
        current_file = ""
        skip_depth = None
        member = None
        member_depth = 0
        enum = None
        enum_const = None
        var = None

        for raw in lines:
            m = _RE_TREE.match(raw)
            if not m:
                continue
            depth = len(m.group(1)) // 2 + 1
            content = raw[m.end():].rstrip("\n")

            if skip_depth is not None:
                if depth > skip_depth:
                    if "/" in content:
                        paths = _RE_PATH.findall(content)
                        if paths:
                            current_file = paths[-1]
                    continue
                skip_depth = None

            kind, location, rest = _split_node(content)
            if location:
                paths = _RE_PATH.findall(location)
                if paths:
                    current_file = paths[-1]

            if depth == 1:
                self._close_container()
                member = enum = enum_const = var = None
                fws = _RE_FRAMEWORK.findall(current_file)
                framework = fws[-1] if fws else ("objc" if "/usr/include/objc/" in current_file else None)
                if framework not in self.frameworks:
                    skip_depth = 1
                    continue
                words = rest.split()
                if kind in ("ObjCInterfaceDecl", "ObjCProtocolDecl", "ObjCCategoryDecl"):
                    name = words[-1] if words else ""
                    ckind = {"ObjCInterfaceDecl": "interface", "ObjCProtocolDecl": "protocol"}.get(kind)
                    self.container = {
                        "target": self._class(name, ckind, framework) if ckind else None,
                        "framework": framework, "avail": _empty_avail(), "members": [],
                        "protocols": [], "super": None, "is_definition": False, "is_category": ckind is None,
                    }
                elif kind == "EnumDecl":
                    mm = _RE_NAMED_TYPED.search(rest)
                    if mm:
                        enum = self.enums.setdefault(mm.group(1), {
                            "framework": framework, "type": mm.group(2), "avail": _empty_avail(), "constants": {},
                        })
                        enum["_next"] = 0
                    else:
                        skip_depth = 1
                elif kind == "VarDecl" and " extern" in f" {rest}":
                    mm = _RE_NAMED_TYPED.search(rest)
                    if mm:
                        var = self.constants.setdefault(mm.group(1), {
                            "framework": framework, "type": mm.group(2), "avail": _empty_avail(),
                        })
                    else:
                        skip_depth = 1
                else:
                    skip_depth = 1
                continue

            c = self.container
            if c is not None:
                if depth == 2:
                    member = None
                    if kind in _AVAIL_KINDS:
                        _add_avail(c["avail"], rest, kind)
                    elif content.startswith("super ObjCInterface"):
                        tm = _RE_TYPES.search(content)
                        if tm:
                            c["super"] = tm.group(1)
                    elif kind == "ObjCInterface":
                        tm = _RE_TYPES.search(content)
                        if tm and c["is_category"] and c["target"] is None:
                            c["target"] = self._class(tm.group(1), "interface", c["framework"])
                    elif kind == "ObjCProtocol":
                        tm = _RE_TYPES.search(content)
                        if tm:
                            c["protocols"].append(tm.group(1))
                    elif kind == "ObjCMethodDecl":
                        c["is_definition"] = True
                        head = rest.split("'", 1)[0]
                        mm = _RE_METHOD.search(rest)
                        if " implicit " in f" {head} " or not mm:
                            skip_depth = depth
                            continue
                        member = {
                            "kind": "method", "static": mm.group(1) == "+", "selector": mm.group(2),
                            "returns": _strip_nullability(mm.group(3)), "params": [], "avail": _empty_avail(),
                            "framework": c["framework"], "variadic": "variadic" in rest[mm.end():],
                        }
                        member_depth = depth
                        c["members"].append(member)
                    elif kind == "ObjCPropertyDecl":
                        c["is_definition"] = True
                        mm = _RE_NAMED_TYPED.search(rest)
                        if not mm:
                            skip_depth = depth
                            continue
                        attrs = mm.group(4).split()
                        member = {
                            "kind": "property", "name": mm.group(1), "static": "class" in attrs,
                            "type": _strip_nullability(mm.group(2)), "readonly": "readonly" in attrs,
                            "getter": None, "setter": None, "avail": _empty_avail(), "framework": c["framework"],
                        }
                        member_depth = depth
                        c["members"].append(member)
                    else:
                        if kind == "ObjCIvarDecl":
                            c["is_definition"] = True
                        skip_depth = depth
                    continue

                if member is not None and depth == member_depth + 1:
                    if kind in _AVAIL_KINDS:
                        _add_avail(member["avail"], rest, kind)
                    elif kind == "ParmVarDecl" and member["kind"] == "method":
                        mm = _RE_NAMED_TYPED.search(rest)
                        if mm:
                            member["params"].append([mm.group(1), _strip_nullability(mm.group(2))])
                        else:
                            tm = _RE_TYPES.search(rest)
                            member["params"].append(["", _strip_nullability(tm.group(1)) if tm else ""])
                    elif member["kind"] == "property" and content.startswith(("getter ObjCMethod", "setter ObjCMethod")):
                        tm = _RE_TYPES.search(content)
                        if tm:
                            member["getter" if content.startswith("getter") else "setter"] = tm.group(1)
                continue

            if enum is not None:
                if depth == 2:
                    enum_const = None
                    if kind in _AVAIL_KINDS:
                        _add_avail(enum["avail"], rest, kind)
                    elif kind == "EnumConstantDecl":
                        mm = _RE_NAMED_TYPED.search(rest)
                        if mm:
                            enum_const = enum["constants"].setdefault(
                                mm.group(1), {"value": enum["_next"], "avail": _empty_avail()}
                            )
                            enum["_next"] = enum_const["value"] + 1
                    continue
                if enum_const is not None:
                    if depth == 3 and kind in _AVAIL_KINDS:
                        _add_avail(enum_const["avail"], rest, kind)
                    else:
                        vm = _RE_VALUE.match(content)
                        if vm:
                            enum_const["value"] = int(vm.group(1))
                            enum["_next"] = enum_const["value"] + 1
                continue

            if var is not None and depth == 2 and kind in _AVAIL_KINDS:
                _add_avail(var["avail"], rest, kind)

        self._close_container()
        for e in self.enums.values():
            e.pop("_next", None)
        return {"classes": self.classes, "enums": self.enums, "constants": self.constants}


def parse_ast_dump(lines, frameworks) -> dict:
    return _Parser(frameworks).parse(lines)


# Dumping and caching ------------------------------------------------------------------------------------------------


def dump_sdk(sdk: str, frameworks=None) -> dict:
    frameworks = frameworks or SDK_FRAMEWORKS[sdk]
    version = sdk_version(sdk)
    path = sdk_path(sdk)
    with tempfile.TemporaryDirectory() as tmp:
        src = Path(tmp) / "sdk_api.m"
        src.write_text("".join(f"#import <{fw}/{fw}.h>\n" for fw in frameworks))
        cmd = ["xcrun", "--sdk", sdk, "clang", "-x", "objective-c", "-fsyntax-only", "-fno-color-diagnostics",
               "-isysroot", path]
        if sdk == "iphoneos":
            cmd += ["-target", f"arm64-apple-ios{version}"]
        cmd += ["-Xclang", "-ast-dump", str(src)]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, errors="replace")
        model = parse_ast_dump(proc.stdout, frameworks)
        stderr = proc.stderr.read()
        if proc.wait() != 0 and not model["classes"]:
            raise SdkError(f"clang failed for SDK '{sdk}':\n{stderr[-2000:]}")
    return {"format": CACHE_FORMAT, "sdk": sdk, "version": version, "frameworks": frameworks, **model}


def load_sdk(sdk: str, cache_dir: Path, refresh=False) -> dict:
    version = sdk_version(sdk)
    cache = Path(cache_dir) / f"sdk-{sdk}-{version}.json"
    if cache.exists() and not refresh:
        data = json.loads(cache.read_text())
        if data.get("format") == CACHE_FORMAT and data.get("frameworks") == SDK_FRAMEWORKS[sdk]:
            return data
    data = dump_sdk(sdk)
    cache.parent.mkdir(parents=True, exist_ok=True)
    cache.write_text(json.dumps(data))
    return data


# Merged view --------------------------------------------------------------------------------------------------------


class SdkApi:
    """Union of one or more SDK dumps, with availability queries."""

    def __init__(self, dumps):
        self.sdks = {d["sdk"]: d["version"] for d in dumps}
        self.classes, self.enums, self.constants = {}, {}, {}
        for d in dumps:
            sdk = d["sdk"]
            for name, cls in d["classes"].items():
                dst = self.classes.setdefault(name, {**cls, "members": {}, "sdks": []})
                dst["sdks"].append(sdk)
                if cls["members"] and not dst.get("super"):
                    dst["super"] = cls["super"]
                for p in cls["protocols"]:
                    if p not in dst["protocols"]:
                        dst["protocols"].append(p)
                for key, m in cls["members"].items():
                    dm = dst["members"].setdefault(key, {**m, "sdks": []})
                    dm["sdks"].append(sdk)
            for name, e in d["enums"].items():
                dst = self.enums.setdefault(name, {**e, "constants": {}, "sdks": []})
                dst["sdks"].append(sdk)
                for cname, c in e["constants"].items():
                    dst["constants"].setdefault(cname, {**c, "sdks": []})["sdks"].append(sdk)
            for name, c in d["constants"].items():
                self.constants.setdefault(name, {**c, "sdks": []})["sdks"].append(sdk)

    @classmethod
    def load(cls, sdks, cache_dir, refresh=False):
        return cls([load_sdk(s, cache_dir, refresh) for s in sdks])

    # Availability -----------------------------------------------------------------------------------------------------

    def status(self, avail, sdks, outer=None) -> dict:
        """Returns {status: ok|deprecated|unavailable, platforms: [...], deprecated_on: [...]}."""
        if outer is not None:
            avail = _merge_avail(outer, avail)
        present, deprecated_on = [], []
        for sdk in sdks:
            platform = SDK_PLATFORM[sdk]
            attr = avail["attrs"].get(platform)
            if avail["unavailable"] or (attr and attr["unavailable"]):
                continue
            present.append(platform)
            if avail["deprecated"] or (attr and attr["deprecated"] not in ("0", "")):
                deprecated_on.append(platform)
        if not present:
            status = "unavailable"
        elif len(deprecated_on) == len(present):
            status = "deprecated"
        else:
            status = "ok"
        return {"status": status, "platforms": present, "deprecated_on": deprecated_on}

    def class_status(self, name) -> dict:
        cls = self.classes.get(name)
        if cls is None:
            return {"status": "missing", "platforms": [], "deprecated_on": []}
        return self.status(cls["avail"], cls["sdks"])

    def member_status(self, class_name, key) -> dict:
        cls = self.classes[class_name]
        m = cls["members"][key]
        return self.status(m["avail"], m["sdks"], cls["avail"])

    # Lookup -----------------------------------------------------------------------------------------------------------

    def ancestors(self, name):
        """The class itself, its superclasses and every adopted protocol (transitively)."""
        seen, order, stack = set(), [], [name]
        while stack:
            n = stack.pop(0)
            if n in seen or n not in self.classes:
                continue
            seen.add(n)
            order.append(n)
            cls = self.classes[n]
            if cls.get("super"):
                stack.append(cls["super"])
            stack.extend(cls["protocols"])
        return order

    def find_member(self, class_name, key):
        """Finds `key` on the class or any ancestor. Returns (owner_class_name, member) or (None, None)."""
        for owner in self.ancestors(class_name):
            m = self.classes[owner]["members"].get(key)
            if m is not None:
                return owner, m
        return None, None

    def find_selector(self, class_name, selector):
        """Like find_member, but accepts either instance or class method."""
        for static in (False, True):
            owner, m = self.find_member(class_name, member_key(static, selector))
            if m is not None:
                return owner, m
        return None, None
