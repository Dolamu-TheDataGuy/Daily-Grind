# Build Simple TF-IDF Scores

**Date:** 2026-08-21

**Language:** Python

**Source:** boot.dev

**Concepts:** TF-IDF · inverted index · document frequency vs substring matching · `set()` iteration order · `try/except` vs guard clause · RAG fundamentals

---

## 🎯 The Problem

Complete two functions in `main.py`, both operating on a `documents` dictionary mapping document names to their text content.

### `build_inverted_index(documents)`

Return a dictionary that maps each **word** to the **list of document names** where that word appears.

Requirements:
- Treat words as **case-insensitive**.
- Split text using `split()`.
- Each document name should appear **only once per word**, even if the word repeats in that document.
- Keep document names **in the order they are first found**.

```python
build_inverted_index({
    "doc1": "red apple apple",
    "doc2": "green apple",
})
# {
#   "red": ["doc1"],
#   "apple": ["doc1", "doc2"],
#   "green": ["doc2"],
# }
```

### `get_tfidf_scores(term, documents)`

Return a dictionary mapping every document name to that document's **TF-IDF score** for `term`, using this simplified formula:

- **TF** = term count in document / total words in document
- **IDF** = total number of documents / number of documents containing the term
- **TF-IDF** = TF × IDF

More rules:
- Treat the search term as **case-insensitive**.
- Round each score to **2 decimal places**.
- If the term does not appear in any document, every score should be `0.0`.
- If `documents` is empty, return an empty dictionary.

```python
documents = {
    "doc1": "red apple apple",
    "doc2": "green apple",
    "doc3": "blue berry",
}
get_tfidf_scores("apple", documents)
# {"doc1": 1.0, "doc2": 0.75, "doc3": 0.0}
```

**Because:** total documents = 3, documents containing "apple" = 2, so `IDF = 3 / 2 = 1.5`. In `doc1`, `TF = 2/3`, score = `1.0` after rounding. In `doc2`, `TF = 1/2`, score = `0.75`. In `doc3`, `TF = 0/2`, score = `0.0`.

This is another **RAG-adjacent** exercise — TF-IDF is a classic term-weighting scheme, related to (and a conceptual ancestor of) the BM25 ranking from the earlier task in this repo. Both answer "how relevant is this document to this term," just with different formulas.

---

## 🧩 My Thought Process (My First Approach)

**`build_inverted_index`** — for each document, I need the *set* of unique words it contains (a word shouldn't be listed twice for the same document), then for each of those words, record which document it came from. I reached for `set(docstring.split())` to get the unique words per document, then looped that set and appended the document name to each word's list in the index.

**`get_tfidf_scores`** — I split this into two passes over `documents`:

1. **First pass — compute IDF.** Count how many documents contain the term at all, then divide total documents by that count. I wrapped the division in `try/except ZeroDivisionError` in case no document contains the term.
2. **Second pass — compute TF per document.** For each document, split its text, count how many words exactly match the term (case-insensitively), divide by the document's total word count, multiply by the (already-computed) IDF, and round to 2 decimal places.

This two-pass structure mirrors the formula directly: IDF is a corpus-wide constant for this term, TF is per-document, and TF-IDF multiplies them together.

---

## ✅ My Solution

```python
def build_inverted_index(documents):
    if len(documents) == 0:
        return {}
    docmap = {}
    for document in documents:
        docstring = documents[document].lower()
        doclist = set(docstring.split())
        for word in doclist:
            if word not in docmap:
                docmap[word] = [document]
            else:
                docmap[word].append(document)
    return docmap


def get_tfidf_scores(term, documents):
    if len(documents) == 0:
        return {}

    total_document = len(documents)
    number_of_document_term = 0

    for document in documents:
        if term.lower() in documents[document].lower():
            number_of_document_term += 1
    try:
        idf = total_document / number_of_document_term
    except ZeroDivisionError:
        idf = 0

    doc_scores = {}

    for document in documents:
        total_word_document = len(documents[document].split())
        term_count_document = 0
        for word in documents[document].split():
            if word.lower() == term.lower():
                term_count_document += 1
        doc_scores[document] = round((term_count_document / total_word_document) * idf, 2)

    return doc_scores
```

**Verified:** produces the exact expected output on the task's own example — `{"doc1": 1.0, "doc2": 0.75, "doc3": 0.0}` — and the inverted index matches the spec's example too. So on the happy path, this is correct. Two issues surface once you probe further, both detailed below.

---

## 💥 A Real Bug: Substring Matching Instead of Whole-Word Matching

Look closely at the IDF-counting line:

```python
if term.lower() in documents[document].lower():
    number_of_document_term += 1
```

`in` on two strings is a **substring test**, not a word-membership test. `"apple" in "I love pineapple"` is `True`, because "apple" literally appears inside the letters of "pineapple" — even though the document does not contain the *word* "apple" anywhere.

I confirmed this produces a wrong score. Take:

```python
documents = {"doc1": "I love pineapple", "doc2": "apple pie"}
get_tfidf_scores("apple", documents)
```

The correct answer: only `doc2` actually contains the word "apple" (doc1 has "pineapple", a different word), so document frequency should be 1, giving `IDF = 2/1 = 2`, and `doc2`'s score should be `TF (1/2) × IDF (2) = 1.0`.

**What the buggy code actually returns:** `{'doc1': 0.0, 'doc2': 0.5}`. Because "apple" is a substring of "pineapple", `doc1` gets wrongly counted toward document frequency, inflating it to 2. That drags `IDF` down to `2/2 = 1`, and `doc2`'s score comes out as `0.5` — **half** what it should be. `doc1`'s own score still shows `0.0` correctly (its *word*-level TF loop, further down, does use `word.lower() == term.lower()`, an exact match) — so the bug is isolated to the IDF calculation, but it corrupts every document's final score because IDF is a shared multiplier.

**The fix:** the document-frequency check needs to ask "does this document contain the term as a *word*?" — the same exact-match test already correctly used later in the TF loop — not "is the term a substring of the document's raw text?"

---

## 🔍 Portions Worth Refactoring

**1. The document-text is re-split multiple times for the same document.**

In `get_tfidf_scores`, `documents[document].split()` is called once for `total_word_document` (via `len(...)`) and again for the `for word in documents[document].split()` loop right below it — two full string-splits of the same text per document. Splitting once and reusing the resulting list avoids the duplicate work:

```python
words = documents[document].lower().split()
total_word_document = len(words)
for word in words:
    if word == term_lower:
        ...
```

**2. `term.lower()` is recomputed on every loop iteration.**

`term.lower()` appears three separate times across the function — once per document in the IDF loop, and once per word in the innermost TF loop (so potentially dozens of times for a long document). `term` doesn't change during the function, so lowercasing it once at the top and reusing that value removes repeated, pointless work:

```python
term_lower = term.lower()
```

**3. `try/except ZeroDivisionError` is control flow for an expected, checkable condition.**

Reaching for `try/except` when "no document contains the term" is a perfectly normal, anticipated case (not an exceptional one) is a style mismatch. It also computes the surrounding context every time just to catch a division you could see coming with a simple `if`:

```python
# Current — treats a normal case as an exception
try:
    idf = total_document / number_of_document_term
except ZeroDivisionError:
    idf = 0

# Cleaner — check the condition directly
if number_of_document_term == 0:
    idf = 0
else:
    idf = total_document / number_of_document_term
```

Python's convention ("easier to ask forgiveness than permission") does favor `try/except` for things that are *rare* or *hard to check in advance* — like a file that might not exist. A zero count is neither; it's a value you already have in hand and can check directly.

**4. `set()` scrambles the word order inside `build_inverted_index` — a real, if easy-to-miss, non-determinism bug.**

```python
doclist = set(docstring.split())
for word in doclist:
    ...
```

Converting the split words to a `set()` correctly deduplicates them, but sets in Python do **not** preserve insertion order — they're ordered by hash, and **string hashing is randomized per process** in Python 3 for security reasons. That means the order words get inserted into `docmap` (and therefore the order the returned dictionary's keys print in) can differ **between separate runs of the exact same code** on the exact same input.

I confirmed the *values* are still correct regardless — every word maps to the right document list — so this wouldn't fail a test that only checks dictionary contents (Python dict equality with `==` ignores order). But it's worth knowing: **any code that relies on `set()` for order-sensitive deduplication is silently non-deterministic.** The fix is to deduplicate while preserving encounter order, by tracking a `seen` set purely for membership-checking while iterating the original split list (not converted to a set):

```python
seen_words = set()
for word in docstring.split():
    if word in seen_words:
        continue
    seen_words.add(word)
    # ... use word, still in original order
```

This gives the same deduplication guarantee `set()` gave, without losing the original word order.

---

## ✅ Refactored, Bug-Fixed, Type-Hinted Solution

```python
def build_inverted_index(documents: dict[str, str]) -> dict[str, list[str]]:
    if not documents:
        return {}
    index: dict[str, list[str]] = {}
    for doc_name, text in documents.items():
        seen_words: set[str] = set()
        for word in text.lower().split():
            if word in seen_words:
                continue
            seen_words.add(word)
            if word not in index:
                index[word] = [doc_name]
            else:
                index[word].append(doc_name)
    return index


def get_tfidf_scores(term: str, documents: dict[str, str]) -> dict[str, float]:
    if not documents:
        return {}

    term_lower: str = term.lower()
    total_documents: int = len(documents)

    term_counts: dict[str, int] = {}
    doc_lengths: dict[str, int] = {}
    for doc_name, text in documents.items():
        words: list[str] = text.lower().split()
        doc_lengths[doc_name] = len(words)
        term_counts[doc_name] = sum(1 for w in words if w == term_lower)

    document_frequency: int = sum(1 for c in term_counts.values() if c > 0)
    if document_frequency == 0:
        return {doc_name: 0.0 for doc_name in documents}

    idf: float = total_documents / document_frequency
    scores: dict[str, float] = {}
    for doc_name in documents:
        length = doc_lengths[doc_name]
        tf: float = term_counts[doc_name] / length if length else 0.0
        scores[doc_name] = round(tf * idf, 2)
    return scores
```

**Verified against every case:**

| Check | Result |
|---|---|
| `build_inverted_index` on the spec example | `{"red": ["doc1"], "apple": ["doc1","doc2"], "green": ["doc2"]}` — matches spec order ✓ |
| Same call run 3× in separate processes | **identical key order every time** — deterministic ✓ |
| `get_tfidf_scores("apple", documents)` on the spec example | `{"doc1": 1.0, "doc2": 0.75, "doc3": 0.0}` — matches exactly ✓ |
| Substring-bug regression (`"pineapple"` vs `"apple pie"`) | `{"doc1": 0.0, "doc2": 1.0}` — correct whole-word result, bug fixed ✓ |
| Empty `documents` | `{}` ✓ |
| Term missing from all documents | every score `0.0` ✓ |
| `mypy` static check | **passes with zero errors** ✓ |

**What changed, mapped to each fix:**
- `get_tfidf_scores` now computes `term_counts` and `doc_lengths` in **one pass** over the documents (instead of two passes plus a duplicated split), then derives `document_frequency` from `term_counts` directly — this simultaneously fixes the substring bug (word-level exact match is now the *only* way to count a document) and removes the repeated `.split()` and `.lower()` calls.
- The `ZeroDivisionError` try/except became a direct `if document_frequency == 0` check.
- `build_inverted_index` deduplicates with a `seen_words` set used only for membership testing, while the actual iteration order comes from the original (unconverted) `split()` list — preserving first-seen order deterministically.

---

## 🧬 Type Hints

- **`build_inverted_index(documents: dict[str, str]) -> dict[str, list[str]]`** — a dict of document-name-to-text in, a dict of word-to-list-of-document-names out. Two different dict shapes on either side of the signature, which is worth noticing: the value type flips from `str` to `list[str]`.
- **`get_tfidf_scores(term: str, documents: dict[str, str]) -> dict[str, float]`** — same `documents` shape as above, plus the search `term` as a plain string, returning a dict of document-name-to-score.
- Internal annotations (`term_counts: dict[str, int]`, `doc_lengths: dict[str, int]`, `idf: float`) mirror the "Fix Nested Type Hints" lesson from an earlier entry — they're optional for `mypy` to pass, since most of them are inferable, but they make the *shape* of the intermediate data explicit to a reader without having to trace the loop bodies.

---

## 💡 The Core Lesson

**"Does the term appear in this document?" and "is the term a substring of this document's raw text?" are different questions — and a bug can hide in code that looks right because it passes the example test.**

Both `documents[document].lower()` checks in the original code — one using `in` (substring), one using `word.lower() == term.lower()` (exact word match) — happened to agree on the task's own example, because `"apple"` never appears as a substring of any *other* word in that particular corpus. The bug was invisible until tested against a corpus containing a word like "pineapple" that happens to *contain* the search term as a substring. This is the same shape of lesson as the BM25 task's `df` bug: a formula's inputs need the *exact* definition specified (here, "documents containing the term," meaning the term as a word), and a substring or occurrence-count shortcut that looks equivalent on friendly data can silently diverge on real data.

---

## 📚 Lessons Learnt

**1. `in` on two strings is substring matching, not word matching.**
`"apple" in "pineapple"` is `True`. To test whether a *word* appears in a document, split the document first and compare against the resulting list (or use `word == term`), never a raw substring check against the whole text.

**2. Split once, reuse the result — don't call `.split()` twice on the same string.**
Each `.split()` call walks the entire string again. When a function needs both the length and the contents of the tokenized text, split once into a variable and derive both from it.

**3. Lowercase (or any other repeated transformation) belongs outside the loop when the input doesn't change.**
`term.lower()` is the same value on every iteration; compute it once before the loops begin rather than recomputing it per document or per word.

**4. Prefer a direct `if` check over `try/except` for conditions you can see coming.**
`try/except ZeroDivisionError` is idiomatic Python for genuinely exceptional or hard-to-predict failures. A count that might legitimately be zero — which you already have in a variable — is easier and clearer to guard with `if count == 0` than to let fail and catch.

**5. `set()` deduplicates but does not preserve order — and Python's string-hash randomization makes that order vary between runs.**
When you need "unique items, in the order they were first seen," don't convert to a `set()` and iterate that; instead, iterate the original sequence and use a separate `set` only to test membership (`if item in seen: continue`). This keeps both the dedup guarantee and deterministic ordering.

**6. A formula's terms need their exact specified definition, not a shortcut that happens to look equivalent.**
"Documents containing the term" means the term as a discrete word. A substring check, an occurrence-count, or any other approximation can match the spec's own worked example by coincidence while being wrong in general — the failure only appears on inputs the original example didn't cover.

---

## 🔗 Further Reading

- [Wikipedia — tf–idf](https://en.wikipedia.org/wiki/Tf%E2%80%93idf)
- [Python docs — `str.split()`](https://docs.python.org/3/library/stdtypes.html#str.split)
- [Python docs — hash randomization (`PYTHONHASHSEED`)](https://docs.python.org/3/using/cmdline.html#envvar-PYTHONHASHSEED)
- Related entry in this repo: [Fix BM25 Score](../2026-09-17-fix-bm25-score/README.md) — a more sophisticated relevance-scoring formula that also hinges on getting document-frequency exactly right.
- Next to explore: extend `get_tfidf_scores` to score *every* term in a query at once and sum the results — the natural next step toward a working multi-term search ranking.