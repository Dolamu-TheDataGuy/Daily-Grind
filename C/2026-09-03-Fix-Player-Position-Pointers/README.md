# Fix Player Positions (Pointers)

**Date:** 2026-09-03

**Language:** C

**Source:** boot.dev

**Concepts:** pointers · pointer arithmetic · dereferencing (`*ptr`) · address-of (`&`) · `NULL` as "no valid address" · in-place array mutation · C99 vs C89 for-loop declarations

---

## 🎯 The Problem

You're tracking player positions in a simple 1D world, stored in an `int` array. Two functions use **pointers and addresses**, and both are buggy:

```c
void move_players(int *positions, int length, int delta);
int *find_player(int *positions, int length, int target);
```

### `move_players`

Must **change the array in-place**. For each player, add `delta` to their current position:

```c
int positions[5] = {10, 20, 30, 40, 50};
move_players(positions, 5, 3);
// positions becomes {13, 23, 33, 43, 53}
```

You must use the `int *positions` pointer (or another pointer variable) to update the elements.

### `find_player`

Must **search the array** and return a pointer to the first element matching `target`:

- If `target` is found, return the **address** of that element (e.g. `&positions[i]`).
- If `target` is not found, return `NULL`.

```c
int positions[5] = {13, 23, 33, 43, 53};
int *found = find_player(positions, 5, 33);
// found points to positions[2]:  *found == 33,  found == &positions[2]

int *missing = find_player(positions, 5, 999);
// missing == NULL
```

### Pointer hints given

- An array name like `positions` can act like a pointer to its first element.
- `&positions[i]` is the address of the element at index `i`.
- `*(positions + i)` is the same as `positions[i]`.
- To change a value through a pointer, assign to `*ptr` — e.g. `*ptr = *ptr + delta;`.
- `NULL` is a special pointer value meaning "no valid address."

Constraints: fix the existing functions only — no `main`, no `malloc`/`free`.

---

## 🧩 My Thought Process (My First Approach)

The task's own hints point at two different, equally valid ways to touch every element through a pointer, and I ended up using **both** — one per function, without initially noticing the inconsistency.

**`move_players` — "walk the pointer forward."**
I treated `positions` itself as a moving cursor: dereference it to touch the current element, then advance the pointer by one position, `length` times:

```c
for (int i = 0; i < length; i++) {
    *positions += delta;
    positions++;
}
```

Each iteration, `*positions` is "the player at the pointer's current address," and `positions++` moves the cursor to the next player. By the time the loop ends, every element has been touched exactly once.

**`find_player` — "keep the pointer fixed, offset with an index."**
Here I left `positions` pointing at the start of the array the whole time, and used `*(positions + i)` to reach into each slot without ever changing `positions` itself:

```c
for (int i = 0; i < length; i++) {
    int value = *(positions + i);
    if (value == target) {
        return positions + i;
    }
}
```

This matters here specifically because the function needs to **return an address** (`positions + i`) once a match is found — keeping the original `positions` value intact throughout means that arithmetic is always relative to the true start of the array, so `positions + i` is always a valid, correctly-indexed address to hand back.

I also added a defensive `if (positions == NULL) return NULL;` at the top of `find_player`, reasoning from the hint that "`NULL` is a special pointer value meaning no valid address" — if the caller somehow passed a null array pointer, dereferencing it inside the loop would be undefined behaviour, so bailing out immediately keeps the function safe.

---

## ✅ My Solution

```c
#include <stddef.h>

void move_players(int *positions, int length, int delta) {
    for (int i = 0; i < length; i++) {
        *positions += delta;
        positions++;
    }
}

int *find_player(int *positions, int length, int target) {
    if (positions == NULL) {
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        int value = *(positions + i);
        if (value == target) {
            return positions + i;
        }
    }
    return NULL;
}
```

**Verified** by compiling with `gcc -std=c99 -Wall -Wextra` (zero warnings) and running against the task's examples plus additional edge cases:

| Call | Result |
|---|---|
| `move_players({10,20,30,40,50}, 5, 3)` | `{13, 23, 33, 43, 53}` ✓ |
| `find_player({13,23,33,43,53}, 5, 33)` | `*found == 33`, `found == &positions[2]` ✓ |
| `find_player({13,23,33,43,53}, 5, 999)` | `NULL` ✓ |
| `move_players({5,10,15}, 3, -5)` (negative delta) | `{0, 5, 10}` ✓ |
| `move_players(arr, 0, 100)` (zero length) | array unchanged ✓ |
| `find_player(arr, 0, 42)` (zero length) | `NULL` ✓ |

Both functions are **correct** on every case tested. The two refactoring points below are about style and portability, not bugs.

---

## 🧪 How `verifyplayer.c` Verifies It

The table above summarizes results, but [`verifyplayer.c`](verifyplayer.c) is the actual script that produced them. It's a **self-contained test harness written in plain C — no `munit.h`, no external test library** — built so the solution can be checked with nothing but a C compiler, independent of boot.dev's own `main.c`/`task.c` grading setup.

### Structure, top to bottom

**1. The solution, copied in verbatim (lines 6–31).**
`move_players` and `find_player` are pasted directly into the file, under the comment `/* ===== task.c — your solution, unmodified ===== */`. This is deliberate: the test script owns its own copy of the functions under test, so it compiles and runs as a single `.c` file with no linking step against `task.c`.

**2. Three assertion macros (lines 35–79).**
Instead of pulling in a library, the script defines its own tiny assertion helpers:

```c
static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_INT(actual, expected, msg) \
    do { \
        tests_run++; \
        if ((actual) == (expected)) { tests_passed++; } \
        else { printf("    FAIL: %s -- expected %d, got %d\n", msg, (int)(expected), (int)(actual)); } \
    } while (0)
```

- `tests_run` / `tests_passed` are file-scoped counters, incremented by every check, that drive the final score and exit code.
- `CHECK_INT(actual, expected, msg)` compares two integers — used for checking array contents after `move_players` runs.
- `CHECK_PTR(actual, expected, msg)` compares two pointers by casting both to `void *` before `==` — used for checking the address `find_player` returns, since comparing `int *` values of different provenance can warn under strict flags.
- `CHECK_NULL(ptr, msg)` is `CHECK_PTR` specialized to expect `NULL` — used for the "not found" case.
- All three are wrapped in `do { ... } while (0)`. This is the standard C idiom for writing a multi-statement macro that still behaves like a single statement — it lets `CHECK_INT(...)​;` be followed by a semicolon and safely nested inside an `if`/`else` without the braces leaking into the surrounding code.
- On failure, each macro prints a `FAIL:` line naming the check and the mismatch, but does **not** abort — every check in a test function always runs, so one failure doesn't hide a second, unrelated failure in the same test.

**3. Four test functions, one per scenario (lines 83–150).**
Each mirrors a `munit_case` from `task.c`, but as a plain `void` function that calls the macros directly:

| Function | What it checks |
|---|---|
| `test_move_and_find_basic` | `move_players` shifts all 5 elements by `+3`, then `find_player` locates the value `33` and returns `&positions[2]` exactly. |
| `test_move_negative_delta` | `move_players` correctly *subtracts* when `delta` is negative (`-5`), confirming the function doesn't assume positive movement. |
| `test_find_not_found` | `find_player` returns `NULL` when `target` isn't in the array, rather than an out-of-bounds address or garbage. |
| `test_move_and_find_multiple` | After moving, two elements are set to the same value (`99`) on purpose, then `find_player` must return the **first** match (`&positions[1]`), not the last — this is the test that pins down "first match wins" as part of the contract. |

**4. The runner (lines 154–165).**
`main()` calls all four test functions in sequence, then prints a final `tests_passed/tests_run` tally and returns `0` only if every check passed (`tests_passed == tests_run`), or `1` otherwise — so the script's exit code alone is enough to tell a CI step or shell script whether the solution is still correct.

### Running it

```sh
gcc -std=c99 -Wall -Wextra -o verifyplayer verifyplayer.c
./verifyplayer
echo "exit code: $?"
```

A clean run prints each test name, no `FAIL:` lines, and `19/19 checks passed` (one check per array element plus one per pointer/NULL assertion across all four tests — more than 4, since each test makes several checks), followed by exit code `0`.

---

## 🔍 Portions Worth Refactoring

**1. The two functions use two different pointer idioms for the same job — worth picking one.**

`move_players` advances the pointer itself (`positions++`); `find_player` leaves the pointer fixed and offsets with an index (`positions + i`). Both are textbook-correct C, and the task's own hints present both styles — but using one in each function, without a specific reason, makes the file slightly harder to read as a whole: a reader has to track which mental model applies to which function.

Since `find_player` *has* to keep the original `positions` value intact (it needs to return `positions + i` as an address), the index-offset style is the right one there — it can't be rewritten as pointer-walking without losing the ability to return the correct address. But `move_players` *could* be written in the same index-offset style for consistency, since it doesn't need to preserve or return the original pointer:

```c
void move_players(int *positions, int length, int delta) {
    for (int i = 0; i < length; i++) {
        *(positions + i) += delta;
    }
}
```

I verified this produces identical output to the original. Whether to make this change is a judgment call, not a correctness fix: the pointer-walking version is arguably a better illustration of "a pointer is just an address you can move," which may be exactly the muscle this exercise is meant to build. Matching `find_player`'s style would improve consistency at the cost of that particular teaching moment. Worth knowing both forms exist and why each function chose the one it did, rather than treating either as automatically "more correct."

**2. `for (int i = 0; ...)` requires C99 — worth knowing as a portability fact, not a bug.**

I compiled this file under `-std=c89 -pedantic` specifically to check, and it fails:

```
error: 'for' loop initial declarations are only allowed in C99 or C11 mode
```

Declaring the loop counter inside the `for` statement itself (`for (int i = 0; ...)`) is valid in C99 and later, but not in the older C89/ANSI C standard, which requires all variable declarations at the top of a block. This isn't a defect in your code — modern C compilers default to C99 or later, and boot.dev's own grader almost certainly does too, since the starter code already uses this style. It's worth knowing, though, because some embedded-systems toolchains and older codebases still compile strictly against C89, where this exact line would fail to build. If you ever target one of those, the fix is to hoist the declaration above the loop: `int i; for (i = 0; ...)`.

**3. No performance concern — both functions are already optimal.**

`move_players` and `find_player` each walk the array exactly once, doing O(1) work per element. There is no way to search or update every element of an array in less than O(n) time, so both are already at the best possible complexity for what they do. The earlier BFS entry in this repo had a real O(n) vs O(n²) refactor available; this task doesn't — it's correct and efficient as written.

---

## 💡 The Core Lesson

**A pointer is just a variable holding an address, and there is almost always more than one valid way to walk it across memory — `ptr++` and `*(ptr + i)` both move you through the same array, and the right choice depends on whether you need to keep the original address around.**

`find_player` needs the original `positions` to compute a correct returned address at any index, so it must *not* mutate the pointer — the index-offset form is the only option. `move_players` returns nothing and never needs the original pointer again, so pointer-walking is equally valid there. This is the heart of what "pointer logic" means in C: the same abstract operation (visit every array element) can be written multiple structurally different ways, and picking between them is about what the rest of the function needs from the pointer afterward — not about one style being inherently more correct.

---

## 📚 Lessons Learnt

**1. An array name decays to a pointer to its first element.**
`positions` in a function signature *is* a pointer — `int *positions` and `int positions[]` are interchangeable as parameters. This is why both `*positions` (dereference the pointer) and `positions[i]` (array indexing) work on the same variable.

**2. `*(ptr + i)` and `ptr[i]` are the same operation.**
Pointer arithmetic and array indexing are two notations for identical machine behaviour — `ptr[i]` is defined in C as shorthand for `*(ptr + i)`. Either is correct; which one to use is a readability choice.

**3. `ptr++` moves the pointer itself; it does not touch what it points to.**
Advancing a pointer changes *which* address it holds, not the value stored there. `*positions += delta` changes the value at the current address; the separate `positions++` statement is what then moves to the next slot.

**4. Whether a function can safely advance its own pointer parameter depends on what it still needs from that pointer.**
`move_players` can destroy its local copy of `positions` by walking it forward, because the caller's own pointer is untouched (C passes pointers by value) and the function never needs the original address again. `find_player` cannot do the same, because it must compute `positions + i` relative to the *true* start of the array to return a correct address — so it has to leave `positions` fixed and index with a separate counter.

**5. `NULL` represents "no valid address," and checking for it before dereferencing is a real safety habit, not boilerplate.**
Dereferencing a `NULL` pointer is undefined behaviour in C — it can crash, or worse, silently corrupt memory depending on the platform. Guarding with `if (ptr == NULL) return ...;` before any `*ptr` is a habit worth forming early, even in exercises where the grader may never actually pass a null pointer.

**6. The C99 vs C89 for-loop declaration difference is a real, occasionally-relevant portability fact.**
`for (int i = 0; ...)` needs `-std=c99` or later. Code that will only ever run through a modern compiler never notices this; code aimed at older embedded toolchains can fail to compile over exactly this line. Worth knowing the boundary exists even if it doesn't bite in this exercise.

---

## 🔗 Further Reading

- [C FAQ — Arrays and Pointers](https://c-faq.com/aryptr/index.html)
- [cppreference — Pointer declaration](https://en.cppreference.com/w/c/language/pointer)
- [cppreference — C99 for-loop scope](https://en.cppreference.com/w/c/language/for) (see "Notes" on C99's `for` scoping)
- Next to explore: rewrite `find_player` to return the **index** instead of a pointer (returning `-1` for not-found instead of `NULL`), and compare which interface — pointer-or-NULL vs index-or-sentinel — is more natural for different callers.