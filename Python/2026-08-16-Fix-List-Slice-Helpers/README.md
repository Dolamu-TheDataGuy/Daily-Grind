# Fix List Slice Helpers

**Date:** 2026-09-17

**Language:** Python

**Source:** boot.dev

**Concepts:** list slicing · slice syntax `[start:end:step]` · negative step · slicing returns a new list · safe out-of-bounds slicing · loopless iteration

---

## 🎯 The Problem

Fix three functions in `main.py` so they use list slicing correctly:

- **`extract_range(items, start, end)`** — returns the elements from `start` up to, but not including, `end`.
- **`reverse_items(items)`** — returns all elements in reverse order.
- **`take_every_other(items, start)`** — returns every second element, beginning at `start`.

### Requirements

- Use **list slicing**.
- Do **not** use explicit loops.
- Empty lists and slice bounds beyond the list must work **normally without raising errors**.
- Do **not** modify the input list.

### Example

```python
items = ["map", "key", "torch", "rope", "coin"]

extract_range(items, 1, 4)      # ["key", "torch", "rope"]
reverse_items(items)            # ["coin", "rope", "torch", "key", "map"]
take_every_other(items, 0)      # ["map", "torch", "coin"]
```

---

## 🧩 My Thought Process (My First Approach)

Python's slice syntax is `items[start:stop:step]`, and each of the three functions maps onto one part of it:

- **`extract_range`** is the plain two-argument form — `items[start:end]`. The stop index is exclusive, which matches "up to, but not including, `end`" exactly, so no adjustment is needed.
- **`reverse_items`** needs a **negative step** — `items[::-1]`. Omitting start and stop means "the whole list," and `-1` walks it backwards.
- **`take_every_other`** uses the **step** with a start — `items[start::2]`. Begin at `start`, take every second element to the end.

The requirements to avoid loops and avoid mutation were the giveaway that slicing was the intended tool: a slice expression is a single, loopless operation that returns a brand-new list, so both constraints are satisfied automatically.

---

## ✅ My Solution

```python
def extract_range(items, start, end):
    return items[start:end]


def reverse_items(items):
    return items[::-1]


def take_every_other(items, start):
    return items[start::2]
```

Three one-line functions. **Verified** against every example and every edge case the task lists:

| Call | Result |
|---|---|
| `extract_range(items, 1, 4)` | `['key', 'torch', 'rope']` ✓ |
| `reverse_items(items)` | `['coin', 'rope', 'torch', 'key', 'map']` ✓ |
| `take_every_other(items, 0)` | `['map', 'torch', 'coin']` ✓ |
| `extract_range([], 0, 3)` | `[]` — empty list, no error ✓ |
| `extract_range(items, 2, 999)` | `['torch', 'rope', 'coin']` — bound past the end, no error ✓ |
| input list after all calls | **unchanged** ✓ |

---

## 💡 The Core Lesson

**Slicing already handles the edge cases the task warns about — you don't write code for them, the operation does.**

The requirements list three things that would normally need defensive code: empty lists, out-of-bounds indices, and not mutating the input. With a manual loop you'd have to guard each one — check `if not items`, clamp indices to `len(items)`, build a fresh list to avoid touching the original. **Slicing gives you all three for free:**

1. **Out-of-bounds is silent, not fatal.** `items[2:999]` on a 5-element list doesn't raise — slicing clamps the bounds to the list and returns what exists. Compare this to indexing: `items[999]` raises `IndexError`, but `items[2:999]` never does. This is the single most important property for this task.

2. **Empty lists just work.** `[][0:3]` returns `[]`. There's nothing to special-case; the slice of an empty list is an empty list.

3. **A slice returns a new list.** `items[start:end]` builds a fresh list object — it never modifies the original. I verified `result is items` is `False` even for a full slice. So the "do not modify the input" rule is satisfied without any copying on my part; the slice *is* the copy.

That's why the three functions are one line each. The task looks like it's testing whether you can add the right guards — but the real answer is recognising that slicing makes the guards unnecessary.

---

## 📚 Lessons Learnt

**1. The slice syntax is `items[start:stop:step]`, and `stop` is exclusive.**
`items[1:4]` gives indices 1, 2, 3 — not 4. The exclusive stop is what makes "from `start` up to but not including `end`" a direct, no-arithmetic translation to `items[start:end]`.

**2. A negative step reverses.**
`items[::-1]` reads the whole list back to front. The empty start and stop mean "everything," and `-1` sets the direction. This is the idiomatic Python reversal — no `reversed()`, no loop, no `.reverse()` (which would mutate).

**3. The `step` value takes every *n*th element.**
`items[start::2]` starts at `start` and takes every second element to the end. `step` combines freely with start and stop: `items[1:10:2]` would take every second element from index 1 up to 10.

**4. Slicing never raises on out-of-bounds — indexing does.**
`items[999]` raises `IndexError`; `items[2:999]` silently returns whatever is there. When a range might exceed the list, slicing is the safe choice precisely because it clamps instead of failing. This is the property that made the "bounds beyond the list must work" requirement trivial.

**5. A slice returns a new list, so it never mutates the input.**
`items[:]` is in fact a common idiom for *copying* a list. Because every slice builds a new list object, functions that return slices satisfy a "don't modify the input" rule automatically — the original is only read, never touched.

**6. Reach for slicing over a loop when the task is "select a sub-sequence."**
"Do not use explicit loops" was a hint, not a restriction: extracting a range, reversing, or stepping through are all things slicing expresses in one clear expression. A loop for any of these would be more code, more error-prone, and less readable.

---

## 🔗 Further Reading

- [Python docs — Common Sequence Operations (slicing)](https://docs.python.org/3/library/stdtypes.html#common-sequence-operations)
- [Python Tutorial — Lists and slicing](https://docs.python.org/3/tutorial/introduction.html#lists)
- Next to explore: slice assignment — `items[1:3] = ["a", "b", "c"]` *does* mutate the list in place and can even change its length. How does that differ from reading a slice, and when is each appropriate?