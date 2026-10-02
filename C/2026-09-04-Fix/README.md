# Fix snek_add

**Date:** 2026-09-04

**Language:** C

**Source:** boot.dev

**Concepts:** tagged unions · `enum`-based type discrimination · heap allocation with `malloc`/`free` · manual buffer-length arithmetic · `memcpy` vs `strcpy` · defensive NULL checks · AddressSanitizer / UndefinedBehaviorSanitizer

---

## 🎯 The Problem

You're working on a tiny object system for a toy language called **Snek**. Every Snek value is a `snek_obj_t` — a tagged union that currently supports two kinds of object: integers and strings.

```c
typedef enum {
  SNEK_INT,
  SNEK_STRING,
} snek_type_t;

typedef struct snek_obj {
  snek_type_t type;
  int refcount;
  union {
    long int_value;
    struct {
      char *data;
      int length;
    } str;
  } as;
} snek_obj_t;

snek_obj_t *snek_new_int(long value);
snek_obj_t *snek_new_string(const char *value);
void snek_free(snek_obj_t *obj);

snek_obj_t *snek_add(snek_obj_t *left, snek_obj_t *right);
```

The job is to fix `snek_add` so it correctly implements addition for Snek objects:

1. **Integer + Integer** — if both operands are `SNEK_INT`, return a new `SNEK_INT` object whose `int_value` is the sum. The inputs must not be modified.
2. **String + String** — if both operands are `SNEK_STRING`, return a new `SNEK_STRING` object whose `data` is the two strings joined with no extra characters in between, and whose `length` field equals the length of the concatenated string. The inputs must not be modified.
3. **Mismatched types** — any other combination (`INT + STRING`, `STRING + INT`, or any future type) must return `NULL`.
4. **Memory rules** — the result must always be heap-allocated through the provided constructors (`snek_new_int` / `snek_new_string`); the caller frees it with `snek_free`.

The task describes the *existing*, broken `snek_add` as having three problems: it sometimes modifies the `left` operand instead of allocating a new object, it uses a fixed-size stack buffer for strings (unsafe for longer inputs), and it doesn't properly handle mismatched types.

---

## 🧩 My Thought Process

I treated this as three separate, independent cases gated by an `enum` check, matching the shape of the spec exactly — one `if` block per valid type combination, falling through to a final `return NULL;` that catches everything else (mismatched types, and any type not yet handled by a future Snek type):

```c
if (left->type == SNEK_INT && right->type == SNEK_INT) { ... }
if (left->type == SNEK_STRING && right->type == SNEK_STRING) { ... }
return NULL;
```

This structure directly satisfies requirement 3 "for free" — I don't need a special mismatched-types branch, because *any* combination that isn't caught by the two explicit `if`s (including `INT + STRING` and `STRING + INT`) falls through to the trailing `NULL`.

**Integers** were straightforward: add the two `int_value`s into a local `long`, then hand that off to `snek_new_int`, which does its own allocation. Nothing about `left` or `right` is touched — I only read from them.

**Strings** needed more care, because of the "no fixed-size stack buffer" warning in the spec and the "don't modify the inputs" rule. My approach:

1. Compute the exact final size needed: `left_len + right_len + 1` (`+1` for the null terminator) — no guessing, no fixed `char buf[256]`-style buffer.
2. `malloc` a scratch buffer of exactly that size.
3. `memcpy` the left string's bytes into the front of the buffer (`left_len` bytes, no terminator yet).
4. `memcpy` the right string's bytes *and* its terminator into the back of the buffer, starting right after the left string (`right_len + 1` bytes) — this one copy both appends the right string and null-terminates the whole thing in a single step.
5. Hand the finished, null-terminated buffer to `snek_new_string`, which allocates the real object and makes its own internal copy.
6. `free` the scratch buffer — it was only ever a temporary staging area.

I deliberately used two separate `memcpy` calls with precise byte counts instead of `strcpy`/`strcat`, specifically because `strcpy`/`strcat` re-scan the buffer for the existing null terminator on every call, which is both unnecessary work (I already know every length) and the exact kind of implicit, buffer-size-blind string handling the spec's fixed-buffer warning seemed aimed at avoiding.

I also added an early `if (!left || !right) return NULL;` guard — dereferencing `left->type` on a `NULL` object would be undefined behaviour, and nothing in the spec says `snek_add` is guaranteed to only ever be called with valid objects.

---

## ✅ My Solution

```c
#include <stdlib.h>
#include <string.h>
#include "exercise.h"

snek_obj_t *snek_new_int(long value) {
  snek_obj_t *obj = malloc(sizeof(snek_obj_t));
  if (!obj) return NULL;
  obj->type = SNEK_INT;
  obj->refcount = 1;
  obj->as.int_value = value;
  return obj;
}

snek_obj_t *snek_new_string(const char *value) {
  if (!value) return NULL;
  snek_obj_t *obj = malloc(sizeof(snek_obj_t));
  if (!obj) return NULL;

  size_t len = strlen(value);
  char *data = malloc(len + 1);
  if (!data) {
    free(obj);
    return NULL;
  }

  memcpy(data, value, len + 1);

  obj->type = SNEK_STRING;
  obj->refcount = 1;
  obj->as.str.data = data;
  obj->as.str.length = (int)len;
  return obj;
}

void snek_free(snek_obj_t *obj) {
  if (!obj) return;
  if (obj->type == SNEK_STRING) {
    free(obj->as.str.data);
  }
  free(obj);
}

snek_obj_t *snek_add(snek_obj_t *left, snek_obj_t *right) {
  if (!left || !right) {
    return NULL;
  }

  if (left->type == SNEK_INT && right->type == SNEK_INT) {
    long value = left->as.int_value + right->as.int_value;
    snek_obj_t *new_obj = snek_new_int(value);
    return new_obj;
  }

  if (left->type == SNEK_STRING && right->type == SNEK_STRING) {
    int left_len = left->as.str.length;
    int right_len = right->as.str.length;

    int new_len = left_len + right_len + 1;
    char *new_string = malloc(new_len);
    if (new_string == NULL) {
      return NULL;
    }
    memcpy(new_string, left->as.str.data, left_len);
    memcpy(new_string + left_len, right->as.str.data, right_len + 1);

    snek_obj_t *obj_string = snek_new_string(new_string);

    free(new_string);

    return obj_string;
  }

  return NULL;
}
```

---

## ✅ Verification — Plain C, No External Test Library

The pasted test script is boot.dev's `main.c`, written against their bundled `munit.h`. Following the standard I'm now using for every C exercise in this repo, I didn't chase down `munit.h` itself — I reproduced the same five cases it checks in a dependency-free plain-C file, using the `CHECK_*` macro pattern established in the pointer-positions entry, extended here with a `CHECK_STR` macro (`strcmp`-based) since this task involves string comparison:

```c
#include <stdio.h>
#include <string.h>
#include "exercise.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK_INT(actual, expected, msg) /* ...same pattern as before... */
#define CHECK_PTR_NOT_NULL(actual, msg)  /* ... */
#define CHECK_NULL(actual, msg)          /* ... */

#define CHECK_STR(actual, expected, msg)                             \
    do {                                                             \
        tests_run++;                                                 \
        if ((actual) != NULL && strcmp((actual), (expected)) == 0) { \
            tests_passed++;                                          \
        } else {                                                     \
            printf("    FAIL: %s -- expected \"%s\", got \"%s\"\n",  \
                   msg, expected, (actual) ? (actual) : "(null)");   \
        }                                                             \
    } while (0)

void test_snek_add_ints(void) { /* 10 + 5 == 15, operands unchanged */ }
void test_snek_add_strings(void) { /* "Hello, " + "world" == "Hello, world" */ }
void test_snek_add_empty_strings(void) { /* "" + "Snek!" and "Snek!" + "" */ }
void test_snek_add_mismatched_types(void) { /* INT + STRING -> NULL, operands unchanged */ }
void test_snek_add_long_strings(void) { /* longer two-sided concatenation */ }

int main(void) {
    test_snek_add_ints();
    test_snek_add_strings();
    test_snek_add_empty_strings();
    test_snek_add_mismatched_types();
    test_snek_add_long_strings();
    printf("\n%d/%d checks passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
```

Compiled and run two ways — a plain build, and a second build under **AddressSanitizer + UndefinedBehaviorSanitizer**, specifically because this code does its own hand-rolled buffer-length math (`left_len + right_len + 1`), which is exactly the kind of code where an off-by-one would show up as a silent heap overflow rather than a visible test failure:

```bash
gcc -std=c99 -Wall -Wextra -o verify_plain exercise.c verify_snek_add.c
./verify_plain

gcc -std=c99 -Wall -Wextra -fsanitize=address,undefined -g -o verify_asan exercise.c verify_snek_add.c
./verify_asan
```

**Result (both builds, identical):**

```
Running checks for snek_add

  test_snek_add_ints
  test_snek_add_strings
  test_snek_add_empty_strings
  test_snek_add_mismatched_types
  test_snek_add_long_strings

24/24 checks passed
```

The ASan/UBSan build exits `0` with no sanitizer report — no heap-buffer-overflow, no use-after-free, no memory leak, and no undefined-behaviour warning. That specifically confirms the `new_string` scratch buffer is sized and filled exactly right (not off by one in either direction), and that the intermediate allocate-then-free-after-copy pattern in the string branch doesn't leak.

---

## 🔍 How This Addresses the Three Stated Bugs

The task describes the *original, broken* `snek_add` as having three specific problems. Worth checking this solution against each one directly, rather than assuming "it passes tests" automatically means all three are fixed:

**1. "It sometimes modifies the left operand instead of creating a new object."**
This solution never writes through `left` or `right` — both branches only *read* `left->as.int_value` / `left->as.str.data` to compute a value, then call `snek_new_int` or `snek_new_string` to allocate a genuinely new object. Verified directly: `test_snek_add_ints` and `test_snek_add_strings` both re-check the operands' original values *after* calling `snek_add`, and all pass.

**2. "It uses a fixed-size stack buffer for strings, which is not safe for longer strings."**
There's no fixed-size buffer here — `new_string` is `malloc`'d at exactly `left_len + right_len + 1` bytes, computed from the actual input lengths every time, so it scales to any input size. `test_snek_add_long_strings` exercises this with inputs long enough that a small fixed buffer (e.g. a 16- or 32-byte stack array) would have silently overflowed; the ASan build confirms no overflow occurs.

**3. "It does not properly handle mismatched types."**
Handled structurally rather than as a special case: the two `if` blocks only match same-type pairs, and everything else — including both orders of `INT`/`STRING` mismatch — falls through to the trailing `return NULL;`. `test_snek_add_mismatched_types` checks both `snek_add(num, txt)` and `snek_add(txt, num)` return `NULL`, and that neither operand was modified in the process.

---

## 🔍 Portions Worth Refactoring

**1. The string branch allocates twice where once would do.**

```c
char *new_string = malloc(new_len);
/* ...fill new_string... */
snek_obj_t *obj_string = snek_new_string(new_string);
free(new_string);
```

`snek_new_string` does its *own* `malloc` + `memcpy` internally. So the current code allocates a scratch buffer, fills it, hands it to `snek_new_string` (which allocates a second buffer and copies the first one into it byte-for-byte), then frees the first buffer. The result is correct — confirmed leak-free under ASan — but it's one extra `malloc`/`memcpy`/`free` cycle per string concatenation that isn't strictly necessary. A version that built the `snek_obj_t` and its final buffer directly inside `snek_add` (bypassing `snek_new_string`) would do one allocation instead of two. Whether that's worth doing is a judgment call: reusing `snek_new_string` keeps all object-construction logic in one place, which is a real readability/maintainability win that a hand-rolled allocation would give up. For objects this small, the extra copy is unlikely to be measurable in practice — this is a "worth knowing the trade-off exists" note, not a correctness issue.

**2. `new_len` is computed as `int`, which could theoretically overflow for very large strings.**

```c
int left_len = left->as.str.length;
int right_len = right->as.str.length;
int new_len = left_len + right_len + 1;
```

If `left_len` and `right_len` were both near `INT_MAX` (not realistic for this exercise, but worth naming as a general C habit), their sum could overflow a 32-bit `int` and wrap to a small or negative number, which would then under-allocate the buffer — a real security-relevant bug class in C string code. Since `str.length` is itself declared as `int` in `exercise.h`, this ceiling is baked into the data structure, not just this function; a hardened version would use `size_t` for length arithmetic and check for overflow before allocating. Not a live bug against this spec's test inputs, but the kind of thing worth flagging out of habit when the pattern is "add two lengths, then allocate."

**3. No performance concern for the integer path.**

`snek_add`'s integer branch does a fixed amount of work regardless of input — O(1). The string branch is O(n) in the combined string length, which is the theoretical minimum for building a concatenated copy (you cannot produce an n-character output without touching n characters). Both are already optimal for what they do.

---

## 💡 The Core Lesson

**A tagged union turns "what kind of value is this?" into a single `enum` comparison, and once you commit to checking that tag explicitly for every valid combination, "handle the invalid case" stops being a separate thing you have to remember — it's just whatever falls through the bottom.**

The mismatched-types requirement wasn't implemented as its own `if (left->type != right->type) return NULL;` branch; it fell out naturally from only writing `if` blocks for the combinations that are actually valid, and letting everything else reach the trailing `return NULL;`. This is a more general pattern than this one exercise: when a spec says "do X for these cases, and reject everything else," it's often safer to enumerate only the *valid* cases explicitly and let the "everything else" be implicit, rather than trying to enumerate every invalid combination by hand — the implicit fallthrough can't miss a case the explicit list forgot to name.

---

## 📚 Lessons Learnt

**1. A tagged union (`enum` + `union`) encodes "what kind of value is this?" as data you can switch on, not as separate C types.**
`snek_obj_t` can represent either an int or a string because its `type` field tells every function which member of the `union` is currently valid to read — the union itself doesn't enforce that, the discipline of always checking `type` first does.

**2. Enumerate the valid cases explicitly; let the invalid case be the fallthrough.**
Writing `if (int+int)`, `if (string+string)`, then a bare `return NULL;` at the end handles every mismatched combination automatically, without needing a dedicated "is this a mismatch?" check that could itself have a bug or an unhandled combination.

**3. Compute buffer sizes from real input lengths, never from a fixed guess.**
`new_len = left_len + right_len + 1` scales exactly to the actual inputs. A fixed-size buffer (`char buf[64]`) works until an input is larger than the guess, at which point it silently corrupts memory instead of failing loudly — exactly the bug class the original task description calls out.

**4. `memcpy` with an exact byte count says more than `strcpy`/`strcat`.**
`memcpy(new_string + left_len, right->as.str.data, right_len + 1)` states precisely how many bytes move, including the deliberate `+1` to also carry over the null terminator in the same call. `strcpy`/`strcat` would work here too, but they re-derive the length by scanning for a null terminator at each call — redundant work when the length is already known, and a function that can run away past a buffer's end if a string somewhere isn't actually null-terminated.

**5. "Don't modify the inputs" means read-only access, enforced by never writing through the pointer.**
Both branches of `snek_add` only ever read `left->as...` / `right->as...` to compute a value that's handed to a *new* allocation. There's no code path that assigns into `left->as.int_value` or `left->as.str.data` — the guarantee holds structurally, not by convention.

**6. AddressSanitizer is the right tool for verifying hand-rolled buffer arithmetic, not just "does the output look right."**
Functional tests (`24/24 checks passed`) confirm the *values* come out correct. They don't, by themselves, prove the buffer was sized and filled without reading or writing one byte past its allocation — a bug that can pass every functional test today and crash unpredictably tomorrow. Running the identical test binary again under `-fsanitize=address,undefined` is what actually confirms the memory safety of code that does manual `malloc` + `memcpy` + length math.

**7. An intermediate allocate-then-copy-then-free step isn't automatically a leak — but it's worth naming as avoidable work.**
The string branch's scratch-buffer-then-`snek_new_string` pattern does one more allocation cycle than strictly needed. Confirming it doesn't leak (via ASan) answers "is this safe?"; it doesn't answer "is this the fewest allocations possible?" — those are two different questions, and only the first one is a correctness requirement here.

---

## 🔗 Further Reading

- [cppreference — Unions](https://en.cppreference.com/w/c/language/union)
- [cppreference — `memcpy`](https://en.cppreference.com/w/c/string/byte/memcpy)
- [AddressSanitizer (Google/LLVM)](https://github.com/google/sanitizers/wiki/AddressSanitizer) — what it catches and how to enable it with `-fsanitize=address`
- Next to explore: add a `SNEK_LIST` type to the `enum`/`union` and extend `snek_add` to concatenate two lists — a good test of whether the "enumerate valid cases, let invalid fall through" pattern from this task scales cleanly to a third type.