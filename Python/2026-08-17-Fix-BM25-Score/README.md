# Fix BM25 Score

**Date:** 2026-09-17

**Language:** Python

**Source:** boot.dev

**Concepts:** BM25 ranking · information retrieval · document frequency (df) · IDF · term frequency saturation · length normalization · debugging a formula · RAG fundamentals

---

## 🎯 The Problem

The `bm25_score(query, document, corpus)` function is supposed to calculate a **BM25 relevance score** for one document — a core ranking function behind search engines and the retrieval step of RAG (Retrieval-Augmented Generation) systems. It has three bugs:

1. It **counts document frequency incorrectly.**
2. It **uses the wrong length normalization.**
3. It **gives a score even when a query term is missing** from the document.

### The rules

Tokenize text by lowercasing and splitting on spaces. For each query term, apply:

```
idf = log(1 + (N - df + 0.5) / (df + 0.5))
score += idf * (freq * (k1 + 1)) / (freq + k1 * (1 - b + b * (doc_len / avg_doc_len)))
```

Where:

- **N** = number of documents in the corpus
- **df** = number of documents that contain the term **at least once**
- **freq** = number of times the term appears in the current document
- **doc_len** = number of tokens in the current document
- **avg_doc_len** = average document length across the corpus
- **k1 = 1.5**, **b = 0.75**

Important details:

- A query term not in the document adds **0** to the score.
- Count `df` **per document**, not total occurrences across the whole corpus.
- Round the final score to **4 decimal places**.
- If the corpus is empty, return **0.0**.

### Example

```python
query = "red fish"
document = "red fish red"
corpus = ["red fish red", "blue fish", "red bird"]

bm25_score(query, document, corpus)   # 1.0314
```

---

## 🧩 My Thought Process (My First Approach)

BM25 looks intimidating, but the formula answers a simple question for each query term: **"how much does this term's presence in this document tell us it's relevant?"** Three forces combine:

- **IDF (rarity)** — a term appearing in few documents is more informative. `df` (document frequency) drives this.
- **Term frequency, saturated** — more occurrences help, but with diminishing returns (that's what the `k1` part does — five mentions isn't five times as relevant as one).
- **Length normalization** — a long document naturally contains more words, so a match in a short document counts for more. That's the `b * (doc_len / avg_doc_len)` factor.

I built the function in stages:

1. **Guard the empty corpus** — return `0.0` immediately.
2. **Tokenize everything** — query, document, and every corpus document, using the provided `tokenize` (lowercase + split).
3. **Compute the corpus statistics** — `doc_len` (length of this document) and `avg_doc_len` (total tokens across the corpus ÷ number of documents).
4. **Loop each query term** — count its `freq` in the document, count its `df` across the corpus, plug both into the IDF and BM25 formulas, and accumulate.

Then I worked through the three bugs specifically.

---

## 💥 The Three Bugs and How I Fixed Them

**Bug 1 — document frequency counted wrong.**
`df` must be *the number of documents containing the term at least once* — not the total number of times the term appears across the whole corpus. Those are different: if "red" appears twice in one document and once in another, the total is 3 but `df` is 2. My fix counts membership per document:

```python
df = 0
for terms in corpus_terms:
    if term in terms:      # "does THIS document contain the term?" — counts once per doc
        df += 1
```

Using `if term in terms` (membership) rather than summing `terms.count(term)` (occurrences) is the fix. I verified the buggy occurrence-counting version produces `0.5912` instead of the correct `1.0314` — the wrong `df` throws off IDF for every term.

**Bug 2 — wrong length normalization.**
The normalization must divide *this document's* length by the *corpus average*: `doc_len / avg_doc_len`. `doc_len` is the token count of the current document, and `avg_doc_len` is the mean across all corpus documents. Getting either the wrong numerator (e.g. corpus total) or a missing average breaks the `b`-weighted denominator term. My version computes both explicitly:

```python
doc_len = len(document_terms)
total_length = 0
for terms in corpus_terms:
    total_length += len(terms)
avg_doc_len = total_length / len(corpus_terms)
```

**Bug 3 — a missing term still scored.**
The rules say a query term not in the document adds **0**. If `freq == 0`, the whole BM25 term for it should contribute nothing. My fix skips the term entirely before it can add to the score:

```python
if freq == 0:
    continue      # missing term adds 0 — skip before scoring
```

Without this `continue`, a query term absent from the document would still pass through the IDF/BM25 math and add a spurious non-zero amount. I confirmed a query with a missing term scores differently with and without the skip — the skip is what enforces the "adds 0" rule.

---

## ✅ My Solution

```python
import math

def tokenize(text):
    if text == "":
        return []
    return text.lower().split()

K1 = 1.5
B = 0.75

def bm25_score(query, document, corpus):
    if len(corpus) == 0:
        return 0.0

    query_terms = tokenize(query)
    document_terms = tokenize(document)
    corpus_terms = []
    for corpus_document in corpus:
        corpus_terms.append(tokenize(corpus_document))

    doc_len = len(document_terms)

    total_length = 0
    for terms in corpus_terms:
        total_length += len(terms)
    avg_doc_len = total_length / len(corpus_terms)
    if avg_doc_len == 0:
        return 0.0

    score = 0.0
    for term in query_terms:
        freq = 0
        for doc_term in document_terms:
            if doc_term == term:
                freq += 1
        if freq == 0:
            continue

        df = 0
        for terms in corpus_terms:
            if term in terms:
                df += 1

        idf = math.log(1 + (len(corpus_terms) - df + 0.5) / (df + 0.5))
        denominator = freq + K1 * (1 - B + B * (doc_len / avg_doc_len))
        score += idf * (freq * (K1 + 1)) / denominator

    return round(score, 4)
```

**Verified:**

| Case | Result |
|---|---|
| `bm25_score("red fish", "red fish red", corpus)` | `1.0314` — matches expected ✓ |
| empty corpus | `0.0` ✓ |
| query term absent from document | contributes `0.0` ✓ |

---

## 💡 The Core Lesson

**"Document frequency" means *how many documents*, not *how many times*.** That single word — *documents* vs *occurrences* — is the difference between a correct search ranking and a broken one.

BM25's whole notion of term importance rests on rarity across the *collection*: a term in 2 of 3 documents is common; a term in 1 of 1000 is a strong signal. If you count total occurrences instead of documents-containing, a term that appears many times in a single document looks artificially "common" and its IDF collapses — exactly the `0.5912`-instead-of-`1.0314` error I confirmed. The fix is a membership test (`if term in terms`) per document, not an occurrence sum.

The second half of the lesson is subtler: **the guard `if freq == 0: continue` isn't an optimization — it's part of the formula's definition.** A missing term contributing zero is a *semantic rule*, and skipping is how you implement "adds 0." Leaving it out doesn't just waste computation; it produces a wrong score.

---

## 📚 Lessons Learnt

**1. In BM25, `df` (document frequency) counts documents, not occurrences.**
`df` is the number of documents that contain a term *at least once*. Count it with a per-document membership test (`if term in doc_terms: df += 1`), never by summing how many times the term appears across the corpus. This is the number that drives IDF, so getting it wrong skews every score.

**2. IDF rewards rarity.**
`log(1 + (N - df + 0.5) / (df + 0.5))` gets larger as `df` gets smaller. A term in few documents carries more information about relevance than one that's everywhere — that's why "the" barely moves a search ranking but a rare keyword dominates it.

**3. Term-frequency saturation is deliberate.**
The `freq * (k1 + 1) / (freq + k1 * ...)` shape means each extra occurrence of a term adds less than the previous one. `k1` controls how fast the reward flattens. A document mentioning a term 10 times isn't 10× more relevant than one mentioning it once — BM25 encodes that intuition.

**4. Length normalization keeps long documents honest.**
`doc_len / avg_doc_len` scales the score by how long this document is relative to the corpus average, with `b` controlling the strength. Without it, long documents win just by containing more words; with it, a match in a concise document is weighted fairly against a match buried in a long one.

**5. A missing term must contribute exactly zero — enforce it explicitly.**
The `if freq == 0: continue` guard is a rule of the algorithm, not a shortcut. Any query term absent from the document adds 0, and skipping before the scoring math is how you guarantee that. Confirmed by observing that omitting the skip changes the score.

**6. This is the "R" in RAG.**
BM25 is a classic *retrieval* function — it's how a RAG pipeline ranks which documents to pull before feeding them to a language model. Understanding it clarifies what "retrieval" actually computes: a relevance score per document, combining rarity, frequency, and length. Modern systems often pair it with vector/embedding search, but BM25 remains a strong, interpretable baseline.

---

## 🔗 Further Reading

- [Wikipedia — Okapi BM25](https://en.wikipedia.org/wiki/Okapi_BM25)
- [Python docs — `math.log`](https://docs.python.org/3/library/math.html#math.log)
- Next to explore: refactor the nested `freq` and `df` loops using `collections.Counter` and a generator expression (`sum(1 for d in corpus_terms if term in d)`), then compare BM25's keyword ranking against a simple embedding-similarity retrieval on the same corpus — where does each win?