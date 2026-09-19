# Build Your Own Map, Filter, and Reduce

**Date:** 2026-09-19

**Language:** Python

**Source:** boot.dev

**Concepts:** first-class functions · higher-order functions · closures over built-ins (`map`/`filter`/`functools.reduce`) · generics with `TypeVar` · `Callable` type hints · accumulator pattern

---

## 🎯 The Problem

Implement three higher-order functions from scratch — simplified versions of Python's built-in `map`, `filter`, and `functools.reduce` — **without using those built-ins**, driving everything with plain `for` loops instead.

You're building a tiny RPG inventory and combat helper. You will apply effects to lists of values, filter out unwanted values, and combine values into a single result.

### `apply_to_all(values, func)`

Takes a list of values and a function that transforms one value into a new value. Returns a **new list** where `func` has been applied to every value, left to right.

```python
health = [10, 20, 30]
def add_five(x): return x + 5

apply_to_all(health, add_five)          # [15, 25, 35]
apply_to_all(health, lambda x: x * 2)   # [20, 40, 60]
```

### `keep_if(values, predicate)`

Takes a list of values and a predicate function returning `True`/`False`. Returns a **new list** containing only the values where `predicate(value)` is `True`.

```python
prices = [20, 50, 120, 5, 80]
def is_expensive(price): return price >= 50

keep_if(prices, is_expensive)                        # [50, 120, 80]
keep_if([1, 2, 3, 4, 5, 6], lambda n: n % 2 == 0)     # [2, 4, 6]
```

### `fold(values, combine, initial)`

Takes a list of values, a two-argument combining function `(accumulator, value) -> new_accumulator`, and a starting value. Walks the list left to right, folding it down to a **single** value.

```python
loot = [10, 25, 5]
def add(a, b): return a + b
fold(loot, add, 0)   # 40

items = ["sword", "shield", "potion"]
def join_names(current, name):
    if current == "":
        return name
    return current + ", " + name
fold(items, join_names, "")   # "sword, shield, potion"
```

### Rules and edge cases

- Do **not** use `map`, `filter`, or `functools.reduce` — implement the behaviour yourself with loops.
- Do **not** modify the input list in place. Always build and return a new result.
- `values` is always a list; `func`/`predicate`/`combine` are always valid callables.
- Empty `values`: `apply_to_all` and `keep_if` return `[]`; `fold` returns `initial` unchanged.

---

## 🧩 My Thought Process (My First Approach)

The task's own framing — **first-class functions** (a function you can store, pass, or return) and **higher-order functions** (a function that takes or returns another function) — is exactly the mental shift needed here. `func`, `predicate`, and `combine` aren't special syntax; they're just values being passed around like any list or integer. Once that clicks, all three functions reduce to the same shape: **loop, call the passed-in function, accumulate.**

**`apply_to_all`** — for every value, call `func(value)` and collect the results into a brand-new list. One-to-one transformation, same length in and out.

**`keep_if`** — for every value, call `predicate(value)`; if it's `True`, keep the *original* value (not a transformed one) in the output list. Selection, not transformation — the output is a subset of the input.

**`fold`** — the only one that doesn't return a list. Start an accumulator at `initial`, and for every value, replace the accumulator with `combine(accumulator, value)`. By the end, the whole list has been "folded" down into one value. This is the most general of the three — `apply_to_all` and `keep_if` can both be expressed as special cases of `fold` if you think about it, though the task asked for three independent implementations.

All three share the identical skeleton: **empty result container (or seeded value), loop through `values`, call the given function, place the result somewhere.** The only thing that changes between them is what "place the result somewhere" means.

---

## ✅ My Solution

```python
def apply_to_all(values, func):
    map_list = []
    for value in values:
        map_list.append(func(value))
    return map_list


def keep_if(values, predicate):
    filter_list = []
    for value in values:
        if predicate(value):
            filter_list.append(value)
    return filter_list


def fold(values, combine, initial):
    sum = initial
    for value in values:
        sum = combine(sum, value)
    return sum
```

**Verified** against every example in the task, plus the empty-list edge cases and a mutation check:

| Call | Result |
|---|---|
| `apply_to_all([10,20,30], add_five)` | `[15, 25, 35]` ✓ |
| `apply_to_all([10,20,30], lambda x: x*2)` | `[20, 40, 60]` ✓ |
| `keep_if([20,50,120,5,80], is_expensive)` | `[50, 120, 80]` ✓ |
| `keep_if([1,2,3,4,5,6], lambda n: n%2==0)` | `[2, 4, 6]` ✓ |
| `fold([10,25,5], add, 0)` | `40` ✓ |
| `fold([2,2,3], multiply, 1)` | `12` ✓ |
| `fold(["sword","shield","potion"], join_names, "")` | `"sword, shield, potion"` ✓ |
| `apply_to_all([], ...)`, `keep_if([], ...)`, `fold([], add, 0)` | `[]`, `[]`, `0` ✓ |
| input list after all calls | **unchanged** ✓ |

---

## 🔍 Portions Worth Refactoring

**1. `sum` as a variable name shadows the built-in `sum()`.**

```python
def fold(values, combine, initial):
    sum = initial          # ← shadows Python's built-in sum() function
    for value in values:
        sum = combine(sum, value)
    return sum
```

This runs correctly — nothing in `fold` needs the real `sum()` — but it's a latent trap. If a future edit inside this function (or a copy-pasted version of it elsewhere) ever needed to call the built-in `sum()`, it would silently call this local variable instead and crash or misbehave. It's also a naming mismatch: this isn't always a numeric sum — the RPG loot example folds strings. A generic name like `accumulator` or `total` avoids the shadowing risk and describes the role more accurately regardless of what's being folded:

```python
def fold(values, combine, initial):
    accumulator = initial
    for value in values:
        accumulator = combine(accumulator, value)
    return accumulator
```

**2. All three functions can be written as one-line comprehensions — worth knowing, not necessarily worth doing here.**

The task explicitly says *"Do not use Python's built-in `map`, `filter`, or `functools.reduce`"* — but list comprehensions aren't those built-ins, they're loop syntax, so this is a legal alternative shape for the two list-returning functions:

```python
def apply_to_all(values, func):
    return [func(value) for value in values]

def keep_if(values, predicate):
    return [value for value in values if predicate(value)]
```

`fold` doesn't have a comprehension form — accumulation across iterations genuinely needs a loop (or a real `functools.reduce`, which is off-limits here) — so the explicit loop in your version is the correct shape for it, not a missed shortcut.

Whether to use the comprehension form for the first two is a judgment call, not a bug fix: the explicit-loop version you wrote is arguably *more* pedagogically honest for this exercise, since it makes visible exactly what `map` and `filter` are doing under the hood — which is the whole point of "build your own." A comprehension would hide that mechanism behind syntax. Keep the loop version if the goal is to internalize the mechanism; reach for the comprehension in day-to-day code once the mechanism is second nature.

**3. No performance issue — all three are already O(n), single pass.**

Each function walks `values` exactly once, does O(1) work per element (a function call, an append, or an accumulator update), and allocates one output container. There's no nested loop, no re-scanning, nothing to optimize algorithmically. This is as efficient as these operations can be.

---

## 🧬 Adding Type Hints — Generics with `TypeVar`

These functions are unusual to annotate because they're **generic**: `apply_to_all` doesn't just work on `list[int]`, it works on a list of *any* type, transformed into a list of *any other* type. A plain `list` or `list[int]` hint would be too narrow and actively wrong for a truly generic helper. The correct tool is `typing.TypeVar`, combined with `Callable` for the function arguments:

```python
from typing import Callable, TypeVar

T = TypeVar("T")   # the type of elements going IN
U = TypeVar("U")   # the type of elements/result coming OUT


def apply_to_all(values: list[T], func: Callable[[T], U]) -> list[U]:
    map_list: list[U] = []
    for value in values:
        map_list.append(func(value))
    return map_list


def keep_if(values: list[T], predicate: Callable[[T], bool]) -> list[T]:
    filter_list: list[T] = []
    for value in values:
        if predicate(value):
            filter_list.append(value)
    return filter_list


def fold(values: list[T], combine: Callable[[U, T], U], initial: U) -> U:
    accumulator: U = initial
    for value in values:
        accumulator = combine(accumulator, value)
    return accumulator
```

**Verified with `mypy`: passes with zero errors**, and every example still produces identical output after annotating — confirming the hints describe the existing behaviour rather than changing it.

Reading each signature:

- **`apply_to_all(values: list[T], func: Callable[[T], U]) -> list[U]`** — takes a list of `T`, a function from `T` to `U`, returns a list of `U`. `T` and `U` can be the same type (`int → int`, as in `add_five`) or different (`int → str`, if you mapped health values to formatted labels) — the signature allows both without knowing in advance which.

- **`keep_if(values: list[T], predicate: Callable[[T], bool]) -> list[T]`** — only **one** type variable, `T`, because filtering never changes what type the elements are — it only removes some of them. The input and output list share the same element type, which the single `T` enforces.

- **`fold(values: list[T], combine: Callable[[U, T], U], initial: U) -> U`** — the richest signature. `values` holds `T`s, but the accumulator and the return type are `U` — and critically, `U` need not equal `T`. The `join_names` example proves this: `values` is `list[str]` (`T = str`), but if you were folding a list of numbers into a formatted string report, `T` would be `int` and `U` would be `str`. `Callable[[U, T], U]` captures the shape precisely: "takes the current accumulator and one element, returns the next accumulator."

This is a good demonstration of *why* generics exist: without `TypeVar`, you'd need a separate, differently-typed version of `fold` for every combination of input/accumulator type. `TypeVar` lets one signature describe the whole family of valid calls while still being precise enough for a type checker to catch a real mistake — e.g., passing a `combine` function whose second parameter doesn't match `T`.

---

## 💡 The Core Lesson

**A higher-order function is just a function that treats another function as ordinary data** — store it, receive it as an argument, return it. `apply_to_all`, `keep_if`, and `fold` don't know or care *what* `func`, `predicate`, or `combine` do internally; they only know each one's shape (how many arguments it takes, and — once type-hinted — what types flow through it). That's the entire mechanism behind Python's real `map`, `filter`, and `functools.reduce`, and behind most data-pipeline code in any language: a small set of generic control-flow shapes (transform each, keep some, collapse to one), parameterized by whatever specific function the caller plugs in.

---

## 📚 Lessons Learnt

**1. First-class functions mean a function is just another value.**
`func`, `predicate`, and `combine` are passed into these functions exactly like a list or an integer would be — stored in a parameter name, called with `()`. There's no special syntax for "receiving a function"; a function is data.

**2. `apply_to_all`, `keep_if`, and `fold` share one skeleton: loop, call, accumulate.**
The difference between map, filter, and reduce isn't the loop — it's *where the result of calling the function goes*. Map appends the transformed value; filter conditionally appends the original value; reduce replaces the accumulator. Recognising the shared skeleton makes all three easy to remember together instead of as three unrelated recipes.

**3. Don't name a local variable after a built-in you're not using but might need later.**
`sum = initial` works today because nothing in `fold` calls Python's real `sum()`. But shadowing a built-in is a latent footgun for future edits, and `accumulator` or `total` is both safer and more descriptive — especially since this particular `fold` doesn't always sum numbers.

**4. A list comprehension is loop syntax, not a built-in — legal even under "no `map`/`filter`" rules — but the explicit loop can be the more honest choice for a learning exercise.**
`[func(v) for v in values]` doesn't call `map`, so it would satisfy this task's constraints. Whether to use it is a readability and pedagogy choice, not a correctness one: a comprehension is more idiomatic in production code; an explicit loop is more revealing when the point of the exercise is understanding the mechanism.

**5. Generic functions need `TypeVar`, not a concrete type hint.**
A function that works across *any* element type (not just `int` or `str`) should be annotated with `TypeVar` placeholders (`T`, `U`) rather than a specific type — otherwise the hint would be false for most valid calls. `Callable[[T], U]` describes a function's input and output types the same way `list[T]` describes a container's element type.

**6. `fold`'s accumulator type doesn't have to match the list's element type.**
The `join_names` example — folding a list of item-name strings into a single joined string — happens to have `T == U == str`, which can obscure this. A fold over `list[int]` producing a formatted `str` report (`T = int`, `U = str`) shows why `fold` needs *two* type variables while `apply_to_all`'s simpler cousin, `keep_if`, only needs one.

---

## 🔗 Further Reading

- [Python docs — `map()`](https://docs.python.org/3/library/functions.html#map), [`filter()`](https://docs.python.org/3/library/functions.html#filter), [`functools.reduce()`](https://docs.python.org/3/library/functools.html#functools.reduce) — the built-ins these three functions reimplement
- [Python docs — `typing.TypeVar`](https://docs.python.org/3/library/typing.html#typing.TypeVar)
- [Python docs — `typing.Callable`](https://docs.python.org/3/library/typing.html#typing.Callable)
- Next to explore: implement `fold` using real `functools.reduce` and compare the two side by side — then try writing `keep_if` in terms of `fold` alone (a predicate-driven accumulator that appends conditionally) to see the "reduce is the most general" idea in action.