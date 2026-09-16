# Fix the Inventory Summary

**Date:** 2026-08-24

**Language:** Python

**Source:** boot.dev

**Concepts:** type hints · `Optional[T]` · `None`-handling · shallow vs deep copy · input mutation · nested dictionaries · `dict.get()` with defaults

---

## 🎯 The Problem

The `summarize_inventory` function has **incorrect type hints** and **mutates some input data**. Fix it so that:

- `inventory` is annotated as a dictionary of warehouse names containing item dictionaries.
- Each item quantity can be an integer **or** `None`.
- `warehouse` can be a string **or** `None`.
- The return value is annotated as a dictionary mapping item names to total integer quantities.
- Passing a **warehouse name** summarizes only that warehouse.
- Passing **`None`** combines quantities from every warehouse.
- `None` and negative quantities are **ignored**. Zero is valid.
- A missing warehouse returns an **empty dictionary**.
- **The input is never changed.**

The task explicitly points you to use `Optional` from Python's `typing` module for values that may be `None`.

### Example

```python
inventory = {
    "north": {"bolts": 4, "nuts": None},
    "south": {"bolts": 3, "nuts": 2},
}

summarize_inventory(inventory, None)
# {"bolts": 7, "nuts": 2}

summarize_inventory(inventory, "north")
# {"bolts": 4}
```

With `None`, both warehouses combine: bolts `4 + 3 = 7`, nuts `None + 2 = 2` (the `None` is ignored, not treated as zero-and-added). With `"north"`, only that warehouse counts: bolts `4`, and nuts is `None` so it's dropped entirely.

---

## 🧩 My Thought Process (My First Approach)

The task has two distinct jobs bundled together: **fix the type hints** and **stop the mutation**. I treated them separately.

**The type hints.** Walking the structure from the outside in:

- `inventory` is a dict whose keys are warehouse names (`str`) and whose values are item dicts. Each item dict maps an item name (`str`) to a quantity that may be an integer or `None` → `dict[str, dict[str, Optional[int]]]`.
- `warehouse` may be a name or `None` → `Optional[str]`.
- The return maps item names to *total* quantities, which are always integers by the time we're done → `dict[str, int]`.

**The logic.** I split on whether a specific warehouse was requested:

- If `warehouse is None`, I want every warehouse, so I take a copy of the whole inventory.
- Otherwise, I build a small dict holding just the one requested warehouse, using `inventory.get(warehouse, {})` so a missing name falls back to an empty dict (which then produces an empty summary).

Then I iterate every item of every selected warehouse, skipping any quantity that's `None` or negative, and accumulate the rest with `summary.get(item, 0) + quantity`.

---

## ✅ My Solution

```python
from typing import Optional


def summarize_inventory(
    inventory: dict[str, dict[str, Optional[int]]],
    warehouse: Optional[str],
) -> dict[str, int]:
    if warehouse is None:
        selected_inventory = inventory.copy()
    else:
        selected_inventory = {
            warehouse: inventory.get(warehouse, {})
        }

    summary = {}
    for items in selected_inventory.values():
        for item, quantity in items.items():
            if quantity is not None and quantity >= 0:
                summary[item] = summary.get(item, 0) + quantity

    return summary
```

**Verified:** correct on all three example cases (`None` → `{"bolts": 7, "nuts": 2}`, `"north"` → `{"bolts": 4}`, missing warehouse → `{}`), correctly keeps zero while dropping negatives, and — critically — **does not mutate the input**.

---

## 💥 The Mutation Trap (Why `.copy()` Is Enough Here — But Only Just)

The task title says the original mutated its input, so the fix has to be provably mutation-free. My solution uses `inventory.copy()` in the `None` branch, and that turns out to be safe — but understanding *why* is the whole lesson.

`inventory.copy()` is a **shallow copy**. It creates a new outer dictionary, but the values inside it are the *same* item-dict objects as the original — not copies of them. So `selected_inventory` is a new top-level dict pointing at the original's inner dicts.

That's safe in this solution for one specific reason: **I only ever read from the inner dicts, never write to them.** The accumulation happens in a *separate* `summary` dictionary. The inner item dicts are only iterated, so sharing them with the original is harmless.

The trap would spring if the code did something like `items[item] += something` — writing back into an inner dict. Because a shallow copy shares those inner dicts, that write would reach through and corrupt the original inventory, even though I "copied" it. In that case I'd need `copy.deepcopy(inventory)` instead.

```python
inventory.copy()        # new outer dict, SAME inner dicts (shallow)
copy.deepcopy(inventory) # new outer dict AND new inner dicts (deep)
```

So the honest summary: `.copy()` is sufficient here because the inner dicts are read-only in my logic — but it's a shallow copy, and if I ever mutated an item dict, this would silently break. Worth a comment in the code to flag that assumption.

---

## 🔬 Comparing My Solution to boot.dev's

```python
from typing import Optional


def summarize_inventory(
    inventory: dict[str, dict[str, Optional[int]]],
    warehouse: Optional[str],
) -> dict[str, int]:
    summary = {}

    if warehouse is None:
        warehouse_items = inventory.values()
    else:
        items = inventory.get(warehouse)
        if items is None:
            return {}
        warehouse_items = [items]

    for items in warehouse_items:
        for item, quantity in items.items():
            if quantity is not None and quantity >= 0:
                current_quantity = summary.get(item, 0)
                summary[item] = current_quantity + quantity

    return summary
```

The identical type hints — both solutions annotate exactly the same way, which is a good confirmation the hints are right.

The interesting difference is **how each avoids mutation**:

| Aspect | My version | boot.dev |
|---|---|---|
| `None` branch | `inventory.copy()` then iterate its values | iterate `inventory.values()` **directly** — no copy |
| Specific warehouse | `{warehouse: inventory.get(warehouse, {})}` | `inventory.get(warehouse)`, early-return `{}` if `None` |
| Missing warehouse | falls back to `{}` → empty summary | explicit `if items is None: return {}` |
| Mutation-safe? | ✓ (shallow copy, read-only inner) | ✓ (never copies, only reads) |

boot.dev's key realisation is that **it doesn't need to copy at all.** Since the code only ever *reads* from the inventory to build a separate `summary`, iterating `inventory.values()` directly is already mutation-free. My `.copy()` is defensive insurance that happens to be unnecessary given the read-only access pattern — it costs a shallow copy of the outer dict for no functional benefit here.

Neither is wrong. boot.dev's is leaner; mine is more defensive. The lesson is recognising that **the mutation risk comes from *writing*, not from *reading***, so if you never write to the input, you don't need to copy it in the first place.

boot.dev also handles the missing warehouse more explicitly, with an early `return {}`. Mine handles it implicitly through the `{}` fallback in `.get()` — both reach the same result, but the explicit version states the intent more clearly to a reader.

---

## 📚 Lessons Learnt

**1. `Optional[T]` means "this can be `T` or `None`."**
`Optional[int]` is shorthand for `int | None`. Reach for it in a type hint whenever a value legitimately may be absent — item quantities that might be `None`, a `warehouse` argument that might be `None`. It documents the `None` possibility right in the signature, so callers know to handle it.

**2. Type the structure from the outside in.**
For a nested container, annotate layer by layer: `inventory` is a dict of (`str` → item dict), and an item dict is a dict of (`str` → `Optional[int]`), giving `dict[str, dict[str, Optional[int]]]`. Reading the nesting outward-in makes even deep annotations mechanical.

**3. Mutation risk comes from *writing* to the input, not *reading* it.**
The whole point of the task was "don't change the input." The realisation: if your code only ever reads the input and accumulates results in a *separate* structure, it can't mutate the input — no copy needed. The danger only appears when you write back into the input's objects.

**4. `.copy()` is shallow — it shares the inner objects.**
`dict.copy()` duplicates the top-level dict but the nested dicts inside are the *same objects*. Reading through them is safe; writing through them reaches back and mutates the original. When you need to safely modify nested structures, use `copy.deepcopy()`. Knowing which one you need depends entirely on whether you'll write to the nested layer.

**5. `dict.get(key, default)` handles the missing-key case cleanly.**
`inventory.get(warehouse, {})` returns an empty dict for a missing warehouse, and `summary.get(item, 0)` starts a running total at zero. Both replace a `try/except KeyError` or an explicit `if key in dict` check with a single expression — the idiomatic way to say "use this value, or this fallback if it's absent."

**6. `None` is not zero.**
The spec says ignore `None` quantities but keep zero. That's why the guard is `if quantity is not None and quantity >= 0` — the `is not None` check must come first, because `None >= 0` raises a `TypeError` in Python 3. Order matters: reject `None` before doing any numeric comparison on the value.

---

## 🔗 Further Reading

- [Python docs — `typing.Optional`](https://docs.python.org/3/library/typing.html#typing.Optional)
- [Python docs — `copy` — shallow vs deep copy](https://docs.python.org/3/library/copy.html)
- [Python docs — `dict.get()`](https://docs.python.org/3/library/stdtypes.html#dict.get)
- Next to explore: rewrite with `collections.defaultdict(int)` so the `summary.get(item, 0)` fallback disappears — when does a `defaultdict` improve readability, and when does it hide intent?