# tools/upstream

Keeps this repository in sync with [apple/metal-cpp](https://github.com/apple/metal-cpp) without losing the SylvaGX
bindings. Python 3 standard library only; needs Xcode (the SDK headers are the reference) and git.

```shell
python3 tools/upstream/check_upstream.py update            # merge the next release, reconcile, build, report
python3 tools/upstream/check_upstream.py update --dry-run  # what would happen, nothing is touched
python3 tools/upstream/check_upstream.py audit             # bindings vs SDK headers
```

Every command writes `upstream-report.md` (and `upstream-report.json`) at the repository root and exits with `1`
when something needs you, `2` on errors.

## Rules

The SDK headers (what Apple's documentation is generated from) are the reference. For a class that exists both in
Apple's folders (`Foundation`, `Metal`, `MetalFX`, `QuartzCore`) and in a SylvaGX folder (`AppKit`, `MetalKit`,
`ModelIO`):

- members that the SDK has, that are not deprecated, and that only the SylvaGX class binds are **copied into Apple's
  class** (declaration, `_XX_INLINE` definition with the macros converted, selector and class lookups in Apple's
  private header);
- the SylvaGX class is then **removed**, with its private selectors / class lookups that nothing uses anymore. Empty
  headers are deleted and includes pointing at them are redirected to Apple's header;
- SylvaGX members that are not in the SDK or are deprecated are dropped (listed in the report);
- when both sides bind a member with different signatures and Apple's does not match the SDK, nothing is changed for
  that class until you decide (see Decisions).

Enums and constants follow the same rules (missing enum values are copied into Apple's enum).

Classes are matched by ObjC name, so `NS::Bundle` in `AppKit/` matches `NSBundle` in `Foundation/` even when the
C++ names differ (use `class_map` for unusual names).

## update

1. aborts if tracked files have uncommitted changes or a merge is in progress;
2. adds the `upstream` remote if missing and fetches its tags (`--no-fetch` to skip);
3. picks the newest `release/metal-cpp_*` tag that is not an ancestor of `HEAD` (`--tag` to choose) and checks
   that Xcode's SDKs are at least that version;
4. lists what upstream removed between the previous release (`--from`) and the new one, with the SylvaGX code that
   still uses it;
5. creates `update/<version>` and merges with `--no-commit`;
6. resolves conflicts in Apple's folders by taking upstream's file and re-injecting the members, enum values,
   selectors and class lookups that were added locally (our side is saved in `.upstream-cache/conflicts/`).
   Conflicts in SylvaGX files and delete/modify conflicts are left for you;
7. reconciles duplicated classes, then builds `test/` with CMake, compiles every umbrella as plain C++ and the
   single header.

Nothing is committed unless `--commit` is given and nothing is pending (then: one merge commit, one reconciliation
commit). Nothing is ever pushed.

If conflicts are left, fix them, `git add` the files, then run `check --apply` and commit.

## Other commands

| Command | What it does |
|-|-|
| `check` | dry run of the reconciliation on the current tree; `--apply` writes it; `--from <rev>` adds upstream removals |
| `resolve-conflicts` | step 6 above on a merge in progress |
| `audit --scope apple\|mine\|all\|<Folder>` | every bound class, enum and constant against the SDK: wrong values, signatures, deprecated or unknown selectors, missing members |
| `report` | re-renders the markdown from the JSON report |

Common options: `--repo`, `--sdk macosx|iphoneos` (repeatable), `--refresh-sdk`, `--decisions`, `--no-build`.

## Decisions

`tools/upstream/decisions.json`:

```json
{
  "members": { "MTLFXFrameInterpolatorDescriptor -isDistortionTextureEnabled": "mine" },
  "class_map": { "NS::SomeClass": "NSSomeOtherObjCName" },
  "type_aliases": { "MenuItemState": "NSControlStateValue" },
  "ignore_classes": ["NS::Responder"]
}
```

- `members`: `"<ObjC class> <+|-><selector>": "apple" | "mine" | "skip"`. `mine` replaces Apple's member with yours,
  `apple` keeps Apple's, `skip` ignores the member everywhere (including the audit);
- `class_map`: C++ class to ObjC class when the name can not be derived;
- `type_aliases`: C++ type name to ObjC type name for signature comparison;
- `ignore_classes`: C++ classes left out of the reconciliation and the audit.

## Files

| File | Role |
|-|-|
| `check_upstream.py` | CLI |
| `gitops.py` | git wrappers (remote, tags, merge, conflict stages) |
| `sdk_api.py` | SDK model from `clang -Xclang -ast-dump` (classes, members, availability, enums), cached per SDK version in `.upstream-cache/` |
| `cpp_api.py` | C++ model of the headers (classes, declarations, inlines, selectors, enums, constants) from disk or a git revision |
| `compare.py` | rules above, audit and upstream removals |
| `apply.py` | header edits and conflict resolution |
| `build.py` | compile checks |
| `report.py` | `upstream-report.md` / `.json` |

The single header check with the SylvaGX umbrellas is only a warning: `SingleHeader/MakeSingleHeader.py` resolves
includes relative to the including file, and the SylvaGX headers include `Foundation.hpp` through the include path.
