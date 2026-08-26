# Build Semantic Chunks

**Date:** 2026-08-22

**Language:** Python

**Source:** boot.dev

**Concepts:** list slicing · token positions · overlap windows · `enumerate()` · `range()` vs `while` · list comprehension · `zip()` · boundary computation · separation of concerns

---

## 🎯 The Problem

Split a list of already-tokenized text into **semantic chunks**, where the chunk boundaries are decided by scores rather than fixed sizes — then attach an embedding to each chunk.

### `split_semantic_chunks(tokens, boundary_scores, threshold, overlap)`

Receives:

- **`tokens`** — text that has already been split into tokens.
- **`boundary_scores`** — one score per _gap_ between neighbouring tokens. `boundary_scores[i]` describes the gap between `tokens[i]` and `tokens[i + 1]`.
- **`threshold`** — a score at or above this value **starts a new chunk**.
- **`overlap`** — the number of tokens _before_ a boundary to include again at the start of the next chunk.

Rules:

- Build chunks in their original order.
- Use **token positions** when adding overlap, so a token position is never inserted twice into the same chunk.
- If the requested overlap reaches beyond the beginning of the text, include only the available tokens (clamp at 0).
- Return an empty list when `tokens` is empty.

### `attach_embeddings(chunks, embeddings)`

Receives the chunks and one provided embedding per chunk. Returns a list of dictionaries in this form:

```python
{
    "tokens": ["token", "values"],
    "embedding": [0.2, 0.8]
}
```

Return an empty list when both inputs are empty.

### Example

```python
tokens = ["Cats", "purr", "Dogs", "bark"]
scores = [0.1, 0.9, 0.2]

chunks = split_semantic_chunks(tokens, scores, 0.8, 1)
print(chunks)
# [["Cats", "purr"], ["purr", "Dogs", "bark"]]
```

The only score at or above the `0.8` threshold is `scores[1] = 0.9`, which sits between `tokens[1]` ("purr") and `tokens[2]` ("Dogs"). So a new chunk starts at position 2. With `overlap = 1`, the second chunk reaches back one token to re-include "purr" — giving `["purr", "Dogs", "bark"]`.

---

## 🧩 My Thought Process (My First Approach)

I broke the problem into two phases, and the first phase felt natural immediately: **find where the chunks start.**

I built a `result` list of start indices, seeded with `[0]` because the first chunk always starts at position 0. Then I walked the boundary scores, and every time a score cleared the threshold, I recorded that a new chunk starts at `index + 1`:

```python
result = [0]
for index, score in enumerate(boundary_scores):
    if score >= threshold:
        result.append(index+1)
```

For the example, this gives `result = [0, 2]` — chunks start at positions 0 and 2. That part I'm still happy with.

The **second phase** — turning those start indices into actual token slices with overlap — is where my solution got complicated. I reasoned that a chunk's slice runs from its own start index to the _next_ chunk's start index, and that overlap means reaching backward from the start. But the first chunk shouldn't reach backward (there's nothing before it), and the last chunk has no "next start" to stop at (it runs to the end). So I ended up enumerating every combination:

- **First and only chunk** (`index == 0 and index == len(result)-1`) → slice from `result[0]` to the end.
- **First of several** (`index == 0`) → slice from `result[0]` to `result[index+1]`, no overlap.
- **Last chunk** (`index == len(result)-1`) → slice from `max(0, result[index]-overlap)` to the end.
- **Middle chunk** (`else`) → slice from `max(0, result[index]-overlap)` to `result[index+1]`.

I drove this with a `while` loop and incremented `index` inside every branch.

It works — I verified it produces the correct output. But writing it, I already felt the shape was wrong: too many branches, too much repetition.

---

## 💥 What I'd Do Differently (Self-Critique)

I shipped a correct solution, but reviewing it I can name three specific weaknesses.

**1. I used a `while` loop when I knew the iteration count in advance.**
I was iterating over `result`, whose length I already knew. That's the textbook case for a `for` loop. A `while` loop signals to a reader "the number of iterations depends on something computed inside the loop" — which wasn't true here, so it's misleading as well as more verbose.

**2. I wrote `index += 1` in every single branch.**
Four branches, four identical increments. This is both redundant and _dangerous_: if I ever add a branch and forget the increment, the `while` loop spins forever. A `for` loop removes the increment entirely — the language handles it — so the whole class of infinite-loop bugs disappears.

**3. I branched on every combination of first/middle/last instead of computing boundaries independently.**
This was the real insight. Every branch was doing the same two jobs — compute a start index, compute an end index — just with different values. I was branching on the _combination_ of those two decisions (2 start cases × 2 end cases = 4 branches) when I could have made each decision _once, separately_:

- **Start index:** for `index == 0`, start is `result[0]` (no overlap). For `index > 0`, start is `max(0, result[index] - overlap)`.
- **End index:** for the last chunk (`index == len(result) - 1`), end is `len(tokens)`. Otherwise it's `result[index + 1]`.

Two independent binary decisions instead of one four-way branch. Same result, a third of the code.

---

## ✅ My Solution

```python
def split_semantic_chunks(tokens, boundary_scores, threshold, overlap):
    if not tokens:
        return []
    result = [0]
    for index, score in enumerate(boundary_scores):
        if score >= threshold:
            result.append(index+1)

    outcome = []
    index = 0
    while index <= len(result)-1:
        if index == 0 and index == len(result)-1:
            outcome.append(tokens[result[0]:])
            index += 1
            continue
        if index == 0:
            outcome.append(tokens[result[0]:result[index+1]])
            index += 1
        elif index == len(result)-1:
            outcome.append(tokens[max(0, result[index]-overlap):])
            index += 1
        else:
            outcome.append(tokens[max(0, result[index]-overlap) : result[index+1]])
            index += 1
    return outcome


def attach_embeddings(chunks, embeddings):
    if not chunks or not embeddings:
        return []
    final_result = []
    for index, chunk in enumerate(chunks):
        content = {}
        content["tokens"] = chunks[index]
        content["embedding"] = embeddings[index]
        final_result.append(content)
    return final_result
```

**Verified:** produces `[["Cats", "purr"], ["purr", "Dogs", "bark"]]` on the example, and matches both the boot.dev and refactored versions across 2000 randomized test inputs — zero mismatches. The logic is correct; the critique below is purely about style and robustness.

---

## 🔬 Comparing the Three Versions

### boot.dev's solution

```python
def split_semantic_chunks(tokens, boundary_scores, threshold, overlap):
    if not tokens:
        return []
    chunk_starts = [0]
    for index in range(len(boundary_scores)):
        if boundary_scores[index] >= threshold:
            chunk_starts.append(index + 1)
    chunks = []
    for index in range(len(chunk_starts)):
        base_start = chunk_starts[index]
        chunk_start = base_start
        if index > 0:
            chunk_start = max(0, base_start - overlap)
        chunk_end = len(tokens)
        if index + 1 < len(chunk_starts):
            chunk_end = chunk_starts[index + 1]
        chunks.append(tokens[chunk_start:chunk_end])
    return chunks
```

The key move: it **computes `chunk_start` and `chunk_end` independently.** It defaults `chunk_end` to `len(tokens)` (the last-chunk case) and _overrides_ it only when there's a next start. Same for the start with overlap. No four-way branch — just two small independent `if`s. This is exactly the "compute boundaries separately" insight I identified after the fact.

### The refactored version

```python
def split_semantic_chunks(tokens, boundary_scores, threshold, overlap):
    if not tokens:
        return []

    starts = [0] + [i + 1 for i, score in enumerate(boundary_scores) if score >= threshold]

    outcome = []
    for i in range(len(starts)):
        start = starts[i] if i == 0 else max(0, starts[i] - overlap)
        end = starts[i + 1] if i + 1 < len(starts) else len(tokens)
        outcome.append(tokens[start:end])
    return outcome


def attach_embeddings(chunks, embeddings):
    return [
        {"tokens": chunk, "embedding": emb}
        for chunk, emb in zip(chunks, embeddings)
    ]
```

This takes it furthest:

- The start indices are built with a **list comprehension** in one line.
- The start/end boundaries use **conditional expressions** (`a if cond else b`) so each is a single line.
- `attach_embeddings` becomes a **single comprehension with `zip()`**, pairing each chunk with its embedding directly instead of indexing both by position.

### Side by side

| Aspect                   | My version     | boot.dev            | Refactored                |
| ------------------------ | -------------- | ------------------- | ------------------------- |
| Loop type                | `while`        | `for range`         | `for range`               |
| Branches for slicing     | 4              | 2 independent `if`s | 2 conditional expressions |
| `index += 1` bookkeeping | manual, ×4     | none                | none                      |
| Start-index list         | `for` + append | `for` + append      | list comprehension        |
| `attach_embeddings`      | `for` + append | (same style)        | `zip()` comprehension     |
| Correct?                 | ✓              | ✓                   | ✓                         |
| Lines (split fn)         | ~20            | ~16                 | ~10                       |

All three return identical output. The difference is entirely readability and resistance to future bugs.

---

## 💡 The Key Insight

**Separate the _decision_ from the _action_.**

My branching solution tangled two decisions (where does the slice start? where does it end?) into one four-way structure, then repeated the same action (`append` + `index += 1`) inside each branch. Once the start and end are computed as independent values, the action happens exactly once:

```python
start = ...   # one decision
end   = ...   # another, independent decision
outcome.append(tokens[start:end])   # one action, no repetition
```

The four-branch version and the two-decision version encode the _same logic_ — but the second makes it obvious that "first chunk" only affects the start, and "last chunk" only affects the end, and those two facts are unrelated to each other. Branching on their combination hid that independence.

---

## 📚 Lessons Learnt

**1. Use `for` loops or comprehensions over `while` loops when iterating over a known range.**
If you know how many iterations you'll do before the loop starts — which you do whenever you're walking a list — a `for` loop is clearer and safer. A `while` loop implies the count depends on runtime state, so using one where a `for` fits misleads the reader. It also eliminates manual `index += 1` bookkeeping and the infinite-loop risk that comes with it.

**2. Separate boundary calculations from list appending to remove repetitive branching.**
When every branch of an `if/elif/else` performs the same action with different values, that's a signal the values should be computed first and the action performed once. Compute `start` and `end` independently, then slice — instead of branching on every combination of conditions.

**3. Use `zip()` when pairing elements from two lists of equal length.**
Indexing two lists by a shared counter (`chunks[index]`, `embeddings[index]`) works, but `zip(chunks, embeddings)` pairs them directly and reads as what it is: "for each chunk with its embedding." It removes the index variable entirely and can't go out of sync.

**4. Conditional expressions collapse simple two-way branches into one line.**
`start = starts[i] if i == 0 else max(0, starts[i] - overlap)` replaces a four-line `if/else` with a single assignment. Used sparingly on genuinely simple decisions, this keeps the eye on the logic rather than the scaffolding.

**5. A correct solution can still be worth refactoring.**
Mine passed every test. The refactor didn't fix a bug — it fixed _clarity_ and _robustness_. Recognising that "it works" and "it's good code" are different bars is itself a skill, and writing the messy version first is often how you discover the clean one.

---

## 🔗 Further Reading

- [Python docs — `zip()`](https://docs.python.org/3/library/functions.html#zip)
- [Python docs — List comprehensions](https://docs.python.org/3/tutorial/datastructures.html#list-comprehensions)
- [Conditional expressions (PEP 308)](https://peps.python.org/pep-0308/)
- Next to explore: what happens to overlap when two boundaries are adjacent (two consecutive scores both above threshold)? Trace how each version handles a one-token chunk with overlap larger than the chunk.
