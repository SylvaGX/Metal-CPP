"""Thin wrappers around the git CLI used by the upstream sync tool."""

import re
import subprocess
from pathlib import Path

UPSTREAM_URL = "https://github.com/apple/metal-cpp.git"
TAG_PREFIX = "release/metal-cpp_"


class GitError(RuntimeError):
    pass


class Git:
    def __init__(self, repo: Path):
        self.repo = Path(repo)

    def run(self, *args, check=True, input_text=None) -> str:
        proc = subprocess.run(
            ["git", *args],
            cwd=self.repo,
            input=input_text,
            capture_output=True,
            text=True,
        )
        if check and proc.returncode != 0:
            raise GitError(f"git {' '.join(args)} failed:\n{proc.stderr.strip() or proc.stdout.strip()}")
        return proc.stdout

    def ok(self, *args) -> bool:
        return subprocess.run(["git", *args], cwd=self.repo, capture_output=True).returncode == 0

    # Repository state -------------------------------------------------------------------------------------------------

    def root(self) -> Path:
        return Path(self.run("rev-parse", "--show-toplevel").strip())

    def current_branch(self) -> str:
        return self.run("rev-parse", "--abbrev-ref", "HEAD").strip()

    def is_clean(self) -> bool:
        return self.run("status", "--porcelain", "--untracked-files=no").strip() == ""

    def merge_in_progress(self) -> bool:
        return self.ok("rev-parse", "-q", "--verify", "MERGE_HEAD")

    def branch_exists(self, name: str) -> bool:
        return self.ok("rev-parse", "-q", "--verify", f"refs/heads/{name}")

    def create_branch(self, name: str):
        self.run("switch", "-c", name)

    # Upstream ---------------------------------------------------------------------------------------------------------

    def ensure_upstream(self, url: str = UPSTREAM_URL) -> bool:
        """Adds the `upstream` remote if missing. Returns True when it was added."""
        remotes = self.run("remote").split()
        if "upstream" in remotes:
            return False
        self.run("remote", "add", "upstream", url)
        return True

    def fetch_upstream(self):
        self.run("fetch", "upstream", "--tags", "--no-recurse-submodules")

    def release_tags(self) -> list:
        """All `release/metal-cpp_*` tags, oldest to newest."""
        tags = [t for t in self.run("tag", "--list", f"{TAG_PREFIX}*").split() if t]
        return sorted(tags, key=tag_version)

    def is_ancestor(self, rev: str, of: str = "HEAD") -> bool:
        return self.ok("merge-base", "--is-ancestor", rev, of)

    def merged_tags(self, of: str = "HEAD") -> list:
        return [t for t in self.release_tags() if self.is_ancestor(t, of)]

    def pending_tags(self, of: str = "HEAD") -> list:
        return [t for t in self.release_tags() if not self.is_ancestor(t, of)]

    # Merge ------------------------------------------------------------------------------------------------------------

    def merge(self, rev: str, commit: bool = False) -> bool:
        """Merges `rev` without opening an editor. Returns False when conflicts are left.

        Unless `commit` is set the merge is left staged (`--no-commit`), so nothing is committed behind the user's back.
        """
        proc = subprocess.run(
            ["git", "merge", "--no-edit", "--no-ff", *([] if commit else ["--no-commit"]), rev],
            cwd=self.repo,
            capture_output=True,
            text=True,
        )
        if proc.returncode == 0:
            return True
        if self.merge_in_progress() and self.unmerged_files():
            return False
        raise GitError(f"git merge {rev} failed:\n{proc.stderr.strip() or proc.stdout.strip()}")

    def predict_conflicts(self, rev: str, of: str = "HEAD") -> list:
        """Files that merging `rev` into `of` would leave conflicted, without touching the work tree."""
        proc = subprocess.run(
            ["git", "merge-tree", "--write-tree", "--name-only", "--no-messages", of, rev],
            cwd=self.repo, capture_output=True, text=True,
        )
        if proc.returncode == 0:
            return []
        if proc.returncode != 1:
            raise GitError(f"git merge-tree failed:\n{proc.stderr.strip()}")
        return [p for p in proc.stdout.splitlines()[1:] if p.strip()]

    def merge_base(self, a: str = "HEAD", b: str = "MERGE_HEAD") -> str:
        return self.run("merge-base", a, b).strip()

    def unmerged_files(self) -> list:
        return [p for p in self.run("diff", "--name-only", "--diff-filter=U").splitlines() if p]

    def unmerged_stages(self, path: str) -> set:
        """Stage numbers present for an unmerged path (1 = base, 2 = ours, 3 = theirs)."""
        stages = set()
        for line in self.run("ls-files", "-u", "--", path).splitlines():
            parts = line.split()
            if len(parts) >= 3:
                stages.add(int(parts[2]))
        return stages

    def show(self, spec: str):
        """Contents of `<rev>:<path>` or `:<stage>:<path>`, or None when it does not exist."""
        proc = subprocess.run(["git", "show", spec], cwd=self.repo, capture_output=True, text=True)
        return proc.stdout if proc.returncode == 0 else None

    def checkout_theirs(self, path: str):
        self.run("checkout", "--theirs", "--", path)

    def add(self, *paths):
        self.run("add", "-A", "--", *paths)

    def rm(self, *paths):
        self.run("rm", "-q", "--", *paths)

    def commit(self, message=None):
        if message is None:
            self.run("commit", "--no-edit")
        else:
            self.run("commit", "-m", message)

    def commit_all(self, message: str):
        self.run("add", "-A")
        self.run("commit", "-m", message)

    # Tree access ------------------------------------------------------------------------------------------------------

    def ls_tree(self, rev: str, *paths) -> list:
        return [p for p in self.run("ls-tree", "-r", "--name-only", rev, "--", *paths).splitlines() if p]

    def read_tree(self, rev: str, *paths) -> dict:
        """Reads every file under `paths` at `rev` in a single `git cat-file --batch` call."""
        files = self.ls_tree(rev, *paths)
        if not files:
            return {}
        request = "".join(f"{rev}:{f}\n" for f in files)
        proc = subprocess.run(
            ["git", "cat-file", "--batch"],
            cwd=self.repo,
            input=request.encode(),
            capture_output=True,
        )
        if proc.returncode != 0:
            raise GitError(proc.stderr.decode(errors="replace"))
        out, result, pos = proc.stdout, {}, 0
        for f in files:
            header_end = out.index(b"\n", pos)
            header = out[pos:header_end].split()
            pos = header_end + 1
            if len(header) < 3 or header[1] == b"missing":
                continue
            size = int(header[2])
            result[f] = out[pos:pos + size].decode(errors="replace")
            pos += size + 1
        return result


def tag_version(tag: str) -> tuple:
    """`release/metal-cpp_macOS27.0.1_iOS27.0.1` -> (27, 0, 1)."""
    m = re.search(r"macOS(\d+(?:\.\d+)*)", tag)
    if not m:
        return (0,)
    parts = tuple(int(p) for p in m.group(1).split("."))
    return parts + (0,) * (3 - len(parts))


def tag_short_name(tag: str) -> str:
    """`release/metal-cpp_macOS27.0.1_iOS27.0.1` -> `27.0.1`."""
    m = re.search(r"macOS(\d+(?:\.\d+)*)", tag)
    return m.group(1) if m else tag.rsplit("/", 1)[-1]
