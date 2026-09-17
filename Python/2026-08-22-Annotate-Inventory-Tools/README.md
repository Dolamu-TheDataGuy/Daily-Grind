# Annotate Inventory Tools

**Date:** 2026-08-22

**Language:** Python

**Source:** boot.dev

**Concepts:** type hints · parameter annotations · return annotations · `list[int]` · type-hints-as-documentation · runtime behaviour vs static hints

---

## 🎯 The Problem

Add parameter and return type hints to all three functions in `main.py` **without changing their runtime behaviour**.

Use these types:

- Item names are **strings**.
- Quantities and stock counts are **integers**.
- A collection of inventory counts is a **list of integers**.
- Formatted item labels are **strings**.

The completed functions should still behave exactly as before:

```python
format_item("Potion", 4)
# "Potion: 4"

add_stock(7, 3)
# 10

total_inventory([4, 2, 6])
# 12
```

---

## 🧩 My Thought Process (My First Approach)

The instruction "without changing their runtime behaviour" is the key constraint: type hints in Python are **annotations, not enforcement**. They don't alter what the code does at runtime — the interpreter doesn't check or coerce them. So this task is purely about *describing* the existing types accurately, not changing any logic.

I annotated each function by reading what it consumes and what it hands back:

- **`format_item(item_name, quantity)`** — takes an item name (`str`) and a quantity (`int`), returns a formatted label like `"Potion: 4"`, which is a `str`.
- **`add_stock(current_count, delivery_count)`** — takes two integer counts, returns their sum, an `int`.
- **`total_inventory(counts)`** — takes a list of integer counts (`list[int]`), sums them, returns an `int`.

The only annotation needing more than a bare type was the list: a plain `list` would be true but vague; `list[int]` says *what's inside* the list, which is the useful part.

---

## ✅ My Solution

```python
def format_item(item_name: str, quantity: int) -> str:
    return item_name + ": " + str(quantity)


def add_stock(current_count: int, delivery_count: int) -> int:
    return current_count + delivery_count


def total_inventory(counts: list[int]) -> int:
    total = 0
    for count in counts:
        total += count
    return total
```

**Verified:** produces `"Potion: 4"`, `10`, and `12` for the three example calls — unchanged from the unannotated version, exactly as the "don't change runtime behaviour" rule requires.

---

## 🔬 Comparing My Solution to boot.dev's

The two solutions are **identical** — same parameter annotations, same return annotations, same `list[int]` on the collection. That's a good outcome for a type-hinting task: when the types are read straight off the code, there's usually one correct annotation, so converging on the same answer confirms it's right.

| Function | Parameters | Return |
|---|---|---|
| `format_item` | `item_name: str`, `quantity: int` | `-> str` |
| `add_stock` | `current_count: int`, `delivery_count: int` | `-> int` |
| `total_inventory` | `counts: list[int]` | `-> int` |

The one small point worth naming: `counts: list[int]` uses the built-in generic syntax available in Python 3.9+. On older versions you'd need `from typing import List` and write `List[int]`. The modern `list[int]` is preferred now — no import required.

---

## 📚 Lessons Learnt

**1. Type hints are documentation, not enforcement.**
Python does not check or coerce types at runtime based on annotations. `add_stock("a", "b")` would still run and concatenate strings despite the `int` hints. Hints exist for *readers*, *editors*, and *static type checkers* like `mypy` — never for the interpreter. That's exactly why annotating a function can't change its runtime behaviour: the annotations are metadata the running code ignores.

**2. Annotate parameters and the return separately.**
Each parameter gets `name: type`, and the return gets `-> type` after the parentheses. They're two independent parts of the signature: `def f(x: int, y: str) -> bool:`. Reading a function's job as "what goes in, what comes out" maps directly onto this structure.

**3. Container types should describe their contents.**
`list` is correct but uninformative; `list[int]` says the list holds integers, which is the detail that actually helps a reader or a type checker. The same applies to `dict[str, int]`, `tuple[int, ...]`, and so on — annotate the *inside*, not just the outer container.

**4. Use the built-in generics (`list[int]`) on modern Python.**
Since Python 3.9 you can subscript built-in collection types directly — `list[int]`, `dict[str, int]` — with no import. The older `typing.List[int]` form is only needed for Python 3.8 and earlier. Prefer the built-in form; it's cleaner and import-free.

**5. Good type hints make a function self-documenting.**
`def total_inventory(counts: list[int]) -> int` tells you everything about the contract without reading the body: give it a list of integers, get an integer back. On a large codebase this is the difference between understanding a function at a glance and having to trace its implementation.

---

## 🔗 Further Reading

- [Python docs — `typing` — Support for type hints](https://docs.python.org/3/library/typing.html)
- [PEP 585 — Type hinting generics in standard collections](https://peps.python.org/pep-0585/) (the `list[int]` syntax)
- [mypy — optional static type checker for Python](https://mypy-lang.org/)
- Next to explore: run `mypy` on a file where the hints are deliberately wrong (e.g. pass a `str` where an `int` is annotated) and see how a static checker catches what the interpreter silently allows.