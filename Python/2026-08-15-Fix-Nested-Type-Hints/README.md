# Fix Nested Type Hints

**Date:** 2026-08-15

**Language:** Python

**Source:** boot.dev

**Concepts:** nested container types · `dict[str, list[str]]` · union types (`X | None`) · optional values · type-hints-as-contract · inference limits of static checkers

---

## 🎯 The Problem

The functions in `main.py` already mostly do the right work, but their **type hints are wrong or incomplete**. Fix the annotations so the tests pass — **without changing the core behaviour**.

Guidance from the task:

- Do **not** change what the functions do.
- Focus on the parameter and return type hints.
- Use **nested container types** where needed.
- Use **optional types** where a value can be missing.

### The three functions

**`invert_roster`** — takes a dictionary mapping each team name to a list of player names, and returns a new dictionary mapping each player name to a list of team names.

```python
invert_roster({
    "red": ["ana", "bo"],
    "blue": ["bo", "cy"]
})
# {
#   "ana": ["red"],
#   "bo": ["red", "blue"],
#   "cy": ["blue"]
# }
```

**`best_total`** — takes a list of score groups (a list of lists of integers), sums each inner list, and returns the largest total. Returns `None` if there are no score groups.

```python
best_total([[3, 4], [10], [2, 2, 2]])   # 10
best_total([])                           # None
```

**`first_notes`** — takes a dictionary mapping each day to a list of notes, where some notes can be `None`. Returns the first real (non-`None`) note for each day, or `None` for a day with no real note.

```python
first_notes({
    "mon": [None, "deploy", "backup"],
    "tue": [None, None]
})
# { "mon": "deploy", "tue": None }
```

---

## 🧩 My Thought Process (My First Approach)

This task is a step up from flat type hints — every function involves **containers nested inside containers**, and two of them involve values that can be `None`. The discipline is the same as before (read the types off the data), but now I had to annotate *each layer* of the nesting.

**`invert_roster` — nesting, no `None`.**
The input is a dict whose keys are team names (`str`) and whose values are lists of player names (`list[str]`) → `dict[str, list[str]]`. The output has the same shape — player name (`str`) → list of team names (`list[str]`) → also `dict[str, list[str]]`. No optional values anywhere, so no unions needed.

**`best_total` — nesting plus an optional return.**
The input is a list of score groups, and each group is itself a list of integers → `list[list[int]]`, a list nested inside a list. The return is the trickier part: it's an integer *most* of the time, but `None` when there are no groups. That's a value that can be missing → the return type is a union, `int | None` (I wrote it `None | int`, which is equivalent).

**`first_notes` — nesting plus optional *inside* the container.**
The input is a dict mapping a day (`str`) to a list of notes — but individual notes can be `None`. So the innermost type isn't `str`, it's `str | None`, giving `dict[str, list[str | None]]`. The `None` lives *inside* the list this time, not just on the return. The return maps each day (`str`) to its first real note, which is either a string or `None` → `dict[str, str | None]`.

---

## ✅ My Solution

```python
def invert_roster(roster: dict[str, list[str]]) -> dict[str, list[str]]:
    players_to_teams = {}
    for team_name in roster:
        for player_name in roster[team_name]:
            if player_name not in players_to_teams:
                players_to_teams[player_name] = []
            players_to_teams[player_name].append(team_name)
    return players_to_teams


def best_total(score_groups: list[list[int]]) -> None | int:
    if len(score_groups) == 0:
        return None
    highest_total = None
    for group in score_groups:
        total = 0
        for score in group:
            total += score
        if highest_total is None or total > highest_total:
            highest_total = total
    return highest_total


def first_notes(notes_by_day: dict[str, list[str | None]]) -> dict[str, str | None]:
    result = {}
    for day in notes_by_day:
        first_note = None
        for note in notes_by_day[day]:
            if note is not None:
                first_note = note
                break
        result[day] = first_note
    return result
```

**Verified:** all three produce the exact expected output —

| Call | Result |
|---|---|
| `invert_roster({"red": ["ana","bo"], "blue": ["bo","cy"]})` | `{'ana': ['red'], 'bo': ['red','blue'], 'cy': ['blue']}` ✓ |
| `best_total([[3,4],[10],[2,2,2]])` | `10` ✓ |
| `best_total([])` | `None` ✓ |
| `first_notes({"mon": [None,"deploy","backup"], "tue": [None,None]})` | `{'mon': 'deploy', 'tue': None}` ✓ |

All the **signature** annotations — every parameter and every return — are correct.

---

## 🔍 What Running mypy Revealed

I ran the static type checker `mypy` on my solution to confirm the hints. Every parameter and return annotation passed. But mypy raised one flag — not on a signature, on an *internal variable*:

```
error: Need type annotation for "players_to_teams"
       (hint: "players_to_teams: dict[<type>, <type>] = ...")
```

This is worth understanding rather than just silencing. `players_to_teams = {}` starts as an empty dict. From that line alone, mypy can't tell what will go into it — an empty `{}` could become a `dict[str, list[str]]`, a `dict[int, bool]`, anything. The task only asked me to fix the *parameter and return* hints, so the tests pass and the function is correct. But for a checker to fully verify the body, an empty container sometimes needs its own annotation:

```python
players_to_teams: dict[str, list[str]] = {}
```

The same applies to `result = {}` in `first_notes`. This is optional for the task (the signatures are what's graded), but it's the difference between "correct annotations on the interface" and "fully type-checkable implementation."

**Why `best_total` didn't need it:** `highest_total = None` is later narrowed by the `if highest_total is None or ...` logic, and mypy can follow that flow. Empty *containers* are the specific case where inference stalls, because there are no elements yet to infer the element type from.

---

## 💡 The Key Insight

**Annotate every layer of the nesting, and put `| None` exactly where the `None` actually lives.**

The three functions form a neat progression in where optionality appears:

- `invert_roster` — no `None` anywhere → plain nested containers: `dict[str, list[str]]`.
- `best_total` — `None` only on the **return** → `list[list[int]]` in, `int | None` out.
- `first_notes` — `None` **inside** the input container *and* on the output values → `dict[str, list[str | None]]` in, `dict[str, str | None]` out.

The skill is placing the union at the right depth. In `first_notes`, the `None` isn't on the list or the dict — it's on the individual *note*, so `| None` attaches to the innermost `str`: `list[str | None]`, not `list[str] | None`. Those two mean completely different things: "a list containing strings-or-None" versus "either a list of strings, or None instead of a list." Reading the data tells you which — the notes themselves are sometimes `None`, so the union belongs on the element.

---

## 📚 Lessons Learnt

**1. Annotate containers layer by layer, from the outside in.**
A list of lists of ints is `list[list[int]]`; a dict of string-to-list-of-strings is `dict[str, list[str]]`. Build the annotation by describing each nesting level in turn. The structure of the type mirrors the structure of the data exactly.

**2. `X | None` marks a value that can be missing.**
When a value is sometimes absent — a return that can be `None`, a list element that might be `None` — express it as a union with `None`. `int | None` is the modern syntax (Python 3.10+); it's equivalent to `Optional[int]` from the `typing` module. Order doesn't matter: `None | int` and `int | None` are the same type.

**3. Put the `| None` at the exact depth where `None` occurs.**
`list[str | None]` (the *elements* can be None) is a different type from `list[str] | None` (the whole *list* can be None). Attach the union to the specific layer that's actually optional. In `first_notes`, individual notes are `None`, so the union sits on the innermost element type.

**4. Type hints don't change runtime behaviour — the task proves it.**
"Do not change the core behaviour" plus "fix the annotations" only makes sense because annotations are inert at runtime. The functions already worked; correcting the hints made them *describable* and *checkable*, not different.

**5. A static checker verifies more than the interpreter, and stalls on empty containers.**
Running `mypy` confirmed the signatures and surfaced that an empty `{}` sometimes needs its own annotation (`result: dict[str, str | None] = {}`), because there are no elements yet for the checker to infer from. The interpreter never cares; a type checker does. Knowing this distinction is what separates "hints that look right" from "hints a tool can actually validate."

---

## 🔗 Further Reading

- [Python docs — `typing`](https://docs.python.org/3/library/typing.html)
- [PEP 604 — the `X | Y` union syntax](https://peps.python.org/pep-0604/)
- [PEP 585 — generics in standard collections (`list[int]`, `dict[str, ...]`)](https://peps.python.org/pep-0585/)
- [mypy documentation](https://mypy.readthedocs.io/)
- Next to explore: annotate the two internal accumulators (`players_to_teams`, `result`) so `mypy` passes cleanly with `--strict`, then try `TypedDict` for a dict with a fixed, known set of keys.