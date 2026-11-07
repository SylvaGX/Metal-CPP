"""Compile checks run after a merge / apply.

- `cmake`: configures and builds `test/` (the same project the repo ships) in a temporary directory.
- `tests`: syntax-only compile of the `test/*.mm` files that `test/CMakeLists.txt` does not build.
- `plain-c++`: one translation unit including every umbrella header with all `*_PRIVATE_IMPLEMENTATION` macros,
  compiled as plain C++ (the headers must never require Objective-C).
- `single-header`: `SingleHeader/MakeSingleHeader.py` over Apple's umbrellas, then the same translation unit against
  it. The bundle with the SylvaGX umbrellas is also attempted, but only as a warning: the generator resolves includes
  relative to the including file, which the SylvaGX headers (`#include "Foundation.hpp"`) do not follow.

Redefinition / redeclaration errors are reported separately: they usually mean Apple added a class or enum that
also exists in a SylvaGX folder.
"""

import re
import shutil
import subprocess
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

import cpp_api

APPLE_UMBRELLAS = [
    "Foundation/Foundation.hpp",
    "QuartzCore/QuartzCore.hpp",
    "Metal/Metal.hpp",
    "MetalFX/MetalFX.hpp",
]
UMBRELLAS = APPLE_UMBRELLAS + [
    "AppKit/AppKit.hpp",
    "MetalKit/MetalKit.hpp",
]
MACROS = ["NS", "AK", "CA", "MTL", "MTLFX", "MDL", "MTK"]
STEPS = ("cmake", "tests", "plain-c++", "single-header")

_RE_DIAG = re.compile(r"^(?P<file>[^\s:][^:]*):(?P<line>\d+):(?P<col>\d+): (?P<level>error|fatal error): (?P<msg>.*)$", re.M)
_RE_REDEF = re.compile(r"redefinition of|redeclar|conflicting types|conflicting declaration|duplicate member")
_MAX_OUTPUT = 12000


@dataclass
class Step:
    name: str
    ok: bool
    cmd: str = ""
    output: str = ""
    errors: list = field(default_factory=list)          # {"file", "line", "message"}
    skipped: str = ""
    warning: bool = False                               # failure is reported but does not fail the build


@dataclass
class BuildResult:
    steps: list = field(default_factory=list)

    @property
    def ok(self):
        return all(s.ok or s.warning for s in self.steps)

    @property
    def redefinitions(self):
        return [e for s in self.steps for e in s.errors if _RE_REDEF.search(e["message"])]

    def to_dict(self):
        return {"ok": self.ok, "redefinitions": self.redefinitions,
                "steps": [{"name": s.name, "ok": s.ok, "warning": s.warning, "cmd": s.cmd, "skipped": s.skipped,
                           "errors": s.errors, "output": s.output[-_MAX_OUTPUT:]} for s in self.steps]}


def _run(cmd, cwd):
    proc = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    return proc.returncode, (proc.stdout + proc.stderr)


def _diagnostics(output, repo):
    out, seen = [], set()
    root = str(Path(repo).resolve())
    for m in _RE_DIAG.finditer(output):
        path = m.group("file")
        if path.startswith(root + "/"):
            path = path[len(root) + 1:]
        key = (path, m.group("line"), m.group("msg"))
        if key in seen:
            continue
        seen.add(key)
        out.append({"file": path, "line": int(m.group("line")), "message": m.group("msg")})
    return out


def _include_flags(repo):
    flags = ["-I", str(repo)]
    for folder in cpp_api.FRAMEWORKS:
        if (Path(repo) / folder).is_dir():
            flags += ["-I", str(Path(repo) / folder)]
    return flags


def _translation_unit(includes):
    lines = [f"#define {m}_PRIVATE_IMPLEMENTATION" for m in MACROS]
    lines += [f'#include "{h}"' for h in includes]
    lines.append("int main() { return 0; }")
    return "\n".join(lines) + "\n"


def _compile(repo, source, language, std):
    cmd = ["xcrun", "clang++", "-x", language, f"-std={std}", "-fsyntax-only", *_include_flags(repo), str(source)]
    code, output = _run(cmd, repo)
    return code == 0, " ".join(cmd), output


def _cmake_step(repo, work):
    if shutil.which("cmake") is None:
        return Step("cmake", True, skipped="cmake not found")
    build_dir = work / "cmake"
    configure = ["cmake", "-S", str(Path(repo) / "test"), "-B", str(build_dir), "-DCMAKE_BUILD_TYPE=Debug"]
    code, output = _run(configure, repo)
    if code != 0:
        return Step("cmake", False, " ".join(configure), output, _diagnostics(output, repo))
    build = ["cmake", "--build", str(build_dir), "--parallel"]
    code, output2 = _run(build, repo)
    output += output2
    return Step("cmake", code == 0, " ".join(build), output, _diagnostics(output, repo))


def _tests_step(repo):
    test_dir = Path(repo) / "test"
    cmake_text = (test_dir / "CMakeLists.txt").read_text() if (test_dir / "CMakeLists.txt").exists() else ""
    sources = [p for p in sorted(test_dir.glob("*.mm")) if p.name not in cmake_text]
    if not sources:
        return Step("tests", True, skipped="every test/*.mm is built by CMake")
    step = Step("tests", True)
    for src in sources:
        ok, cmd, output = _compile(repo, src, "objective-c++", "c++23")
        step.ok &= ok
        step.cmd += cmd + "\n"
        step.output += output
        step.errors += _diagnostics(output, repo)
    return step


def _plain_cpp_step(repo, work):
    includes = [h for h in UMBRELLAS if (Path(repo) / h).exists()]
    tu = work / "plain.cpp"
    tu.write_text(_translation_unit(includes))
    ok, cmd, output = _compile(repo, tu, "c++", "c++17")
    return Step("plain-c++", ok, cmd, output, _diagnostics(output, repo))


def _single_header_step(repo, work, name, umbrellas, warning=False):
    script = Path(repo) / "SingleHeader" / "MakeSingleHeader.py"
    if not script.exists():
        return Step(name, True, skipped="SingleHeader/MakeSingleHeader.py not found")
    single = work / name / "Metal.hpp"
    single.parent.mkdir(parents=True, exist_ok=True)
    includes = [str(Path(repo) / h) for h in umbrellas if (Path(repo) / h).exists()]
    gen = ["python3", str(script), "-o", str(single), *includes]
    code, output = _run(gen, repo)
    if code != 0:
        return Step(name, False, " ".join(gen), output, _diagnostics(output, repo), warning=warning)
    tu = single.parent / "single.cpp"
    tu.write_text(_translation_unit(["Metal.hpp"]))
    cmd = ["xcrun", "clang++", "-x", "c++", "-std=c++17", "-fsyntax-only", "-I", str(single.parent), str(tu)]
    code, output = _run(cmd, repo)
    return Step(name, code == 0, " ".join(cmd), output, _diagnostics(output, repo), warning=warning)


def run(repo, steps=STEPS, keep=None) -> BuildResult:
    """Runs the requested checks. `keep` is a directory to build in (kept afterwards); a temp dir otherwise."""
    repo = Path(repo).resolve()
    result = BuildResult()
    tmp = None
    if keep:
        work = Path(keep)
        work.mkdir(parents=True, exist_ok=True)
    else:
        tmp = tempfile.TemporaryDirectory(prefix="metal-cpp-build-")
        work = Path(tmp.name)
    try:
        if "cmake" in steps:
            result.steps.append(_cmake_step(repo, work))
        if "tests" in steps:
            result.steps.append(_tests_step(repo))
        if "plain-c++" in steps:
            result.steps.append(_plain_cpp_step(repo, work))
        if "single-header" in steps:
            result.steps.append(_single_header_step(repo, work, "single-header", APPLE_UMBRELLAS))
            result.steps.append(_single_header_step(repo, work, "single-header+SylvaGX", UMBRELLAS, warning=True))
    finally:
        if tmp is not None:
            tmp.cleanup()
    return result
