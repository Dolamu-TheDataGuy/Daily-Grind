# Validate Project Paths

**Date:** 2026-08-25

**Language:** Python

**Source:** boot.dev

**Concepts:** `os.path` module · path normalization · `normpath` / `abspath` / `join` / `isabs` / `commonpath` · path traversal security · shared-prefix trap · boolean expression vs `if/return`

---

## 🎯 The Problem

Complete `is_path_allowed(project_dir, requested_path)`. It should return `True` **only when a requested path remains inside the allowed project directory after path normalization.**

The function receives:

- **`project_dir`** — the allowed project directory.
- **`requested_path`** — a relative or absolute requested path.

Rules:

- Relative requested paths begin **inside** `project_dir`. Absolute requested paths are allowed **only when they point inside it.**
- Normalization resolves path components such as `.` and `..`. Reject paths that escape into a **parent or sibling** directory.
- Directory names that merely **share a prefix** with the project directory are also outside it.
- Do **not** access the filesystem — validate only the path strings.

### Examples

```python
is_path_allowed("/srv/app", "assets/../index.html")   # True
is_path_allowed("/srv/app", "../../etc/passwd")        # False
```

The first path looks like it leaves (`assets/..`) but normalization cancels `assets` against `..`, landing back at `/srv/app/index.html` — inside. The second climbs two levels out of the project entirely — rejected.

This is a real **path-traversal security check** — the kind that stops a web server from serving `/etc/passwd` when a user requests `../../etc/passwd`.

---

## 🧩 My Thought Process (My First Approach)

This was my first proper tour of Python's `os.path` module, and the task broke naturally into three stages: **normalize the project dir, normalize the requested path, then check containment.**

**Stage 1 — pin down the project directory.**
I turned `project_dir` into a single canonical form with `os.path.abspath(os.path.normpath(project_dir))`. `normpath` collapses `.`/`..` and redundant separators; `abspath` makes it absolute so there's a fixed reference to compare against.

**Stage 2 — normalize the requested path, handling relative vs absolute.**
The rules treat the two cases differently, so I branched on `os.path.isabs(requested_path)`:

- **Absolute** — normalize it on its own with `abspath(normpath(...))`. An absolute path speaks for itself; it's allowed only if it happens to fall inside the project.
- **Relative** — a relative path *begins inside* the project, so I first `os.path.join` it onto the normalized project dir, *then* normalize. Joining before normalizing is what lets `..` in the requested path resolve against the project directory correctly.

**Stage 3 — is the result still inside?**
This was the part I had to think hardest about. My first instinct was "does the requested path *start with* the project path?" — but that's the classic bug the task explicitly warns about: `/srv/app-secrets` starts with `/srv/app` as a string, yet it's a completely different, sibling directory. String prefix matching is wrong.

The correct tool is `os.path.commonpath([project, requested])`, which returns the longest *directory* both share. If that common ancestor equals the project path exactly, the requested path is genuinely inside it. `commonpath` compares path *components*, so it isn't fooled by `app` vs `app-secrets`.

I wrapped it in `try/except ValueError` because `commonpath` raises when given a mix it can't compare (e.g. an absolute and a relative path, or paths on different Windows drives) — in which case the safe answer is `False`.

---

## ✅ My Solution

```python
import os

def is_path_allowed(project_dir, requested_path):
    norm_project_path = os.path.abspath(os.path.normpath(project_dir))
    if os.path.isabs(requested_path):
        requested_path = os.path.abspath(os.path.normpath(requested_path))
    else:
        requested_path = os.path.abspath(os.path.normpath(os.path.join(norm_project_path, requested_path)))

    try:
        if os.path.commonpath([norm_project_path, requested_path]) == norm_project_path:
            return True
        return False
    except ValueError:
        return False
```

**Verified** against the examples plus the security-critical edge cases:

| `project_dir` | `requested_path` | Result | Why |
|---|---|---|---|
| `/srv/app` | `assets/../index.html` | `True` | `..` cancels `assets`, stays inside ✓ |
| `/srv/app` | `../../etc/passwd` | `False` | escapes to a parent ✓ |
| `/srv/app` | `index.html` | `True` | plain relative path, inside ✓ |
| `/srv/app` | `/srv/app/config/settings.py` | `True` | absolute, inside ✓ |
| `/srv/app` | `/srv/app-secrets/keys.txt` | `False` | **shared prefix, different dir** ✓ |
| `/srv/app` | `/etc/passwd` | `False` | absolute, outside ✓ |

The `app` vs `app-secrets` case is the one that proves the logic is right — a naive `startswith` check would wrongly allow it.

---

## 🔬 Comparing My Solution to boot.dev's — and a Redundancy to Refactor

```python
import os

def is_path_allowed(project_dir, requested_path):
    normalized_project = os.path.abspath(os.path.normpath(project_dir))

    if os.path.isabs(requested_path):
        normalized_request = os.path.abspath(os.path.normpath(requested_path))
    else:
        normalized_request = os.path.abspath(
            os.path.normpath(os.path.join(normalized_project, requested_path))
        )

    try:
        return os.path.commonpath([normalized_project, normalized_request]) == normalized_project
    except ValueError:
        return False
```

The two solutions are **logically identical** — same normalization, same relative/absolute branch, same `commonpath` containment check, same `ValueError` guard. I verified they return the same result on every test case. The only differences are cosmetic, and one of them is a genuine redundancy in mine worth fixing:

**Redundancy: my `if/return True/return False` should be a single boolean return.**

```python
# My version — three lines to produce a boolean
if os.path.commonpath([norm_project_path, requested_path]) == norm_project_path:
    return True
return False

# boot.dev's version — the comparison IS the boolean
return os.path.commonpath([normalized_project, normalized_request]) == normalized_project
```

The expression `commonpath(...) == norm_project_path` already evaluates to `True` or `False`. Wrapping it in `if ...: return True / return False` just re-computes a boolean that's already sitting right there. Returning the comparison directly is shorter, clearer, and removes a branch. This is the same "derive the boolean, don't rebuild it" lesson from earlier tasks — an `if x: return True else return False` is almost always `return x`.

**Refactored version of mine:**

```python
import os

def is_path_allowed(project_dir, requested_path):
    norm_project_path = os.path.abspath(os.path.normpath(project_dir))
    if os.path.isabs(requested_path):
        requested_path = os.path.abspath(os.path.normpath(requested_path))
    else:
        requested_path = os.path.abspath(
            os.path.normpath(os.path.join(norm_project_path, requested_path))
        )

    try:
        return os.path.commonpath([norm_project_path, requested_path]) == norm_project_path
    except ValueError:
        return False
```

One other minor note: I reassigned the `requested_path` parameter in place, while boot.dev used a new variable `normalized_request`. boot.dev's choice is slightly cleaner — keeping the original parameter unmodified makes the code easier to debug (you can still see what was passed in). A small readability win, not a correctness issue.

---

## 💡 The Core Lesson

**Never validate "is this path inside that directory?" with string prefix matching. Compare normalized path *components* with `commonpath`.**

The whole task pivots on one security insight: `"/srv/app-secrets".startswith("/srv/app")` is `True`, but `/srv/app-secrets` is *not* inside `/srv/app` — it's a sibling that happens to share leading characters. Path containment is about directory structure, not string prefixes.

`os.path.commonpath` gets this right because it works on path *segments*: the common path of `/srv/app` and `/srv/app-secrets` is `/srv`, not `/srv/app`, so the containment check correctly fails. Combined with normalization (which resolves `..` *before* the comparison, so an escape attempt is already unmasked), this is the correct and safe way to validate a path — and exactly how real web servers and sandboxes prevent directory-traversal attacks.

---

## 📚 Lessons Learnt

**1. `os.path.normpath` resolves `.` and `..` lexically — no filesystem needed.**
It collapses `assets/../index.html` to `index.html` and `a/./b` to `a/b` by string manipulation alone. Because it doesn't touch the disk, it's the right tool for validating a path *before* you'd ever open it — which is exactly what this task required ("do not access the filesystem").

**2. `os.path.join` before you normalize a relative path.**
A relative requested path only means something *relative to* the project dir. Joining it onto the project dir first, then normalizing, is what lets its `..` components resolve against the correct base. Normalizing the relative path on its own would lose that anchor.

**3. `os.path.abspath` gives a single canonical form to compare against.**
Making both paths absolute removes ambiguity — you're comparing two fully-qualified paths, not a mix of relative and absolute fragments that could line up by accident.

**4. Use `commonpath`, not `startswith`, for containment.**
String prefix checks are fooled by shared-prefix sibling directories (`app` vs `app-secrets`). `commonpath` compares whole path components, so it only reports containment when one path is genuinely an ancestor of the other. This is the security crux of the task.

**5. `commonpath` raises `ValueError` on incomparable paths — guard it.**
Mixing absolute and relative paths, or paths on different drives, makes `commonpath` raise. Wrapping it in `try/except ValueError` and returning `False` means "if I can't even compare them, it's not allowed" — the safe default for a security check.

**6. `if cond: return True else: return False` is just `return cond`.**
When a condition already evaluates to a boolean, return it directly. My original wrapped the `commonpath` comparison in an `if/return True/return False` that rebuilt a boolean already in hand. Returning the expression is shorter and removes a needless branch.

**7. Prefer a new variable over reassigning a parameter.**
boot.dev kept `requested_path` untouched and wrote into `normalized_request`. Leaving the original argument intact makes the function easier to read and debug, since the input is still visible throughout.

---

## 🔗 Further Reading

- [Python docs — `os.path`](https://docs.python.org/3/library/os.path.html)
- [`os.path.commonpath`](https://docs.python.org/3/library/os.path.html#os.path.commonpath) and [`os.path.normpath`](https://docs.python.org/3/library/os.path.html#os.path.normpath)
- [OWASP — Path Traversal](https://owasp.org/www-community/attacks/Path_Traversal) — the real-world attack this function defends against
- Next to explore: `pathlib.Path` — the modern object-oriented alternative to `os.path`. How would `Path.resolve()` and `Path.is_relative_to()` express this same check, and what changes on Windows?