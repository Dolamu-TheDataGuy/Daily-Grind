# Daily-Grind: Building a BM25 Search Ranker in Python

Date: 2026-09-17-BM25-Ranker

## 1. Project Overview

This project implements a keyword-based search ranker using **BM25 (Best Matching 25)**, a probabilistic information retrieval algorithm used to rank documents according to their relevance to a search query.

The objective was to implement a simple search ranker from first principles using Python's built-in data structures, loops, and mathematical operations.

Rather than relying on an external search library, I implemented four core components:

- **Tokenization:** Converts text into searchable terms.
- **Inverted indexing:** Maps terms to documents and records their frequencies.
- **BM25 scoring:** Calculates the relevance score of each document.
- **Result ranking:** Sorts documents by score, with a deterministic tie-breaking rule.

This exercise provides a foundation for understanding Retrieval-Augmented Generation (RAG), where relevant information is retrieved from a document collection before being passed to a language model.

### Learning objectives

By completing this task, I aimed to understand how to:

- Tokenize queries and documents consistently.
- Build an inverted index using dictionaries and loops.
- Distinguish Term Frequency (TF), Document Frequency (DF), and Inverse Document Frequency (IDF).
- Translate the BM25 formula into Python.
- Understand term-frequency saturation and document-length normalization.
- Sort search results using relevance scores and deterministic tie-breaking.
- Identify correctness issues, refactoring opportunities, and scalability considerations.

---

## 2. My Implementation

The following is my original solution, preserved without modification.

```python
import math


def tokenize(text):
    tokens = []
    current = ""
    for ch in text.lower():
        if ch.isalnum():
            current += ch
        else:
            if current != "":
                tokens.append(current)
                current = ""
    if current != "":
        tokens.append(current)
    return tokens


def build_inverted_index(docs_tokens):
    index = {}
    for doc_id in range(len(docs_tokens)):
        tokens = docs_tokens[doc_id]
        counts = {}
        for term in tokens:
            if term in counts:
                counts[term] += 1
            else:
                counts[term] = 1

        for key in counts:
            if key not in index:
                index[key] = {}
            index[key][doc_id] = counts[key]

    return index


def bm25_rank(query, docs, k1=1.5, b=0.75):
    query_token = tokenize(query)

    docs_tokens = []
    doc_length = []
    for doc in docs:
        tokens = tokenize(doc)
        docs_tokens.append(tokens)
        doc_length.append(len(tokens))

    N = len(docs_tokens)
    avgdl = sum(doc_length)/len(doc_length)
    index = build_inverted_index(docs_tokens)

    result = []
    for doc_id in range(N):
        score = 0
        for term in query_token:
            if term in index:
                tf = index[term].get(doc_id, 0)
                df = len(index[term])
                idf = math.log(1 + (N - df + 0.5) / (df + 0.5))
                dl = doc_length[doc_id]
                score += idf * (tf * (k1 + 1)) / (
                    tf + k1 * (1 - b + b * dl / avgdl)
                )

        result.append((doc_id, score))

    result.sort(key=lambda content: (-content[1], content[0]))

    return result
```

---

## 3. Understanding the Tokenizer

The `tokenize()` function converts text into lowercase alphanumeric tokens.

For example:

```python
tokenize("Python is Great! Python supports AI.")
```

Output:

```python
["python", "is", "great", "python", "supports", "ai"]
```

### How it works

**Step 1: Initialize the variables.**

```python
tokens = []
current = ""
```

`tokens` stores completed words, while `current` accumulates the characters of the word being processed.

**Step 2: Normalize the text and iterate through its characters.**

```python
for ch in text.lower():
```

Converting text to lowercase ensures that `Python`, `PYTHON`, and `python` are treated as the same term.

**Step 3: Identify characters that belong to a token.**

```python
if ch.isalnum():
    current += ch
```

The `isalnum()` method recognizes letters and digits. When a character is not alphanumeric, it acts as a token boundary.

For example, `data-driven` becomes `["data", "driven"]`.

**Step 4: Store completed tokens.**

```python
else:
    if current != "":
        tokens.append(current)
        current = ""
```

This prevents empty strings from being added to the token list when multiple separators appear consecutively.

**Step 5: Capture the final token.**

```python
if current != "":
    tokens.append(current)
```

Without this final check, a word ending at the end of the input could be omitted because no separator would trigger its addition to the list.

### Lessons from tokenization

Tokenization defines what the retrieval system considers a term. Since both queries and documents use the same tokenizer, they follow consistent normalization rules.

The implementation is intentionally simple. It does not perform stemming, lemmatization, or stop-word removal. Those techniques could be introduced later if the retrieval task required them.

---

## 4. Understanding the Inverted Index

An inverted index maps each unique term to the documents containing it and the frequency of that term in each document.

Consider the following tokenized documents:

```python
docs_tokens = [
    ["python", "python", "search"],
    ["python", "search", "engine"],
    ["search", "engine"]
]
```

The inverted index would be:

```python
{
    "python": {0: 2, 1: 1},
    "search": {0: 1, 1: 1, 2: 1},
    "engine": {1: 1, 2: 1}
}
```

The structure has two levels:

- The outer dictionary maps a term to its postings.
- The inner dictionary maps document IDs to term frequencies.

For example:

```python
"python": {0: 2, 1: 1}
```

This means that `python` appears twice in document 0 and once in document 1.

### How my implementation builds the index

First, the function iterates through the documents and counts the occurrences of each term:

```python
counts = {}

for term in tokens:
    if term in counts:
        counts[term] += 1
    else:
        counts[term] = 1
```

It then transfers those counts into the global index:

```python
for key in counts:
    if key not in index:
        index[key] = {}

    index[key][doc_id] = counts[key]
```

This two-stage approach separates counting terms within a document from storing their frequencies across the collection.

### Why the inverted index matters

The index provides the information needed to calculate TF and DF:

```python
tf = index[term].get(doc_id, 0)
df = len(index[term])
```

TF is retrieved for a specific document, while DF is calculated by counting the number of distinct document entries for a term.

The inverted index is therefore not just a storage structure. It is the foundation from which the BM25 ranking function obtains the statistics it needs.

---

## 5. Understanding TF, DF, and IDF

TF, DF, and IDF are three fundamental concepts in information retrieval. They are related, but they measure different things.

- **TF measures how often a term occurs in a particular document.**
- **DF measures how many documents contain that term.**
- **IDF uses DF to estimate how informative the term is across the collection.**

Understanding their relationship explains why your implementation uses an inverted index and how the BM25 score is calculated.

### 5.1 Term Frequency (TF)

Term Frequency measures the number of times a particular term occurs within an individual document.

In my implementation:

```python
tf = index[term].get(doc_id, 0)
```

Consider this collection:

```python
docs = [
    "python python search",
    "python search engine",
    "search engine"
]
```

The term frequencies for `python` are:

| Document | Occurrences of `python` | TF |
|---|---|---:|
| Document 0 | Two occurrences | 2 |
| Document 1 | One occurrence | 1 |
| Document 2 | No occurrences | 0 |

#### Why TF matters

A document that mentions a query term multiple times may be more relevant than one that mentions it only once.

However, repeating a word indefinitely should not necessarily increase relevance indefinitely. BM25 therefore incorporates TF into a saturation formula rather than using the raw count as the entire score.

In my implementation, TF determines the frequency component of the current document's BM25 contribution.

### 5.2 Document Frequency (DF)

Document Frequency measures the number of distinct documents in the collection that contain a particular term.

In my implementation:

```python
df = len(index[term])
```

Using the same collection:

| Term | Documents containing the term | DF |
|---|---|---:|
| `python` | 0, 1 | 2 |
| `search` | 0, 1, 2 | 3 |
| `engine` | 1, 2 | 2 |

Notice the difference between TF and DF.

The term `python` appears twice in document 0, but its DF is not 2 because it appears twice in that document. Its DF is 2 because it appears in two distinct documents.

Likewise, `search` has a DF of 3 because all three documents contain it.

#### Why DF matters

DF tells us how widespread a term is across the document collection.

A term found in almost every document may be less useful for distinguishing one document from another. A term found in only a few documents may provide a stronger signal about which documents are relevant.

DF is not itself the relevance score of an individual document. Instead, it supplies the collection-level statistic used to calculate IDF.

### 5.3 Inverse Document Frequency (IDF)

Inverse Document Frequency uses document frequency to estimate how informative a term is across the collection.

My implementation calculates IDF using:

```python
idf = math.log(
    1 + (N - df + 0.5) / (df + 0.5)
)
```

The formula is:

```text
IDF(t) = ln(1 + (N - DF(t) + 0.5) / (DF(t) + 0.5))
```

Where:

- `N` is the total number of documents.
- `DF(t)` is the number of documents containing term `t`.
- `ln` is the natural logarithm.

This is a common smoothed BM25 IDF variant.

#### Worked example

Suppose the collection contains three documents:

```text
N = 3
```

The term `python` appears in two documents:

```text
DF(python) = 2
```

Substituting the values into the formula:

```text
IDF(python) = ln(1 + (3 - 2 + 0.5) / (2 + 0.5))
            = ln(1 + 1.5 / 2.5)
            = ln(1.6)
            ≈ 0.4700
```

Now consider `search`, which appears in all three documents:

```text
IDF(search) = ln(1 + (3 - 3 + 0.5) / (3 + 0.5))
            = ln(1 + 0.5 / 3.5)
            ≈ 0.1335
```

The IDF of `python` is higher because it occurs in fewer documents.

#### Why IDF matters

Suppose a collection contains thousands of technical documents. A common term such as `system` may appear in nearly every document. Its presence alone does little to distinguish relevant documents from irrelevant ones.

A more specialized term, such as `galvanization`, may appear in a smaller proportion of the collection. Its presence can provide a stronger indication that a document discusses corrosion protection.

IDF helps BM25 account for this difference.

A higher IDF does not automatically make a document relevant. The term must also contribute through its frequency in that document, and the final score depends on all the query terms processed.

### 5.4 TF, DF, and IDF compared

| Measurement | Question answered | Scope | Role in BM25 |
|---|---|---|---|
| TF | How often does the term occur in this document? | Individual document | Determines the term-frequency contribution |
| DF | How many documents contain the term? | Entire collection | Supplies the statistic used to calculate IDF |
| IDF | How informative is the term across the collection? | Entire collection | Weights the term's contribution to relevance |

The relationship can be summarized as:

```text
Inverted Index
      |
      +----> TF: frequency in the current document
      |
      +----> DF: number of documents containing the term
                         |
                         v
                    IDF formula
                         |
                         v
             BM25 scoring calculation
```

**Key lesson:** TF and DF are measurements; IDF is a weight derived from DF; BM25 combines IDF with a normalized TF contribution.

---

## 6. Understanding the BM25 Formula

BM25 combines IDF, TF, and document-length normalization.

The contribution of a query term to a document's score is:

```text
TermScore(t, d) =
    IDF(t) * [TF(t, d) * (k1 + 1)]
    ------------------------------------------------
    TF(t, d) + k1 * (1 - b + b * DL(d) / AvgDL)
```

Where:

| Symbol | Meaning |
|---|---|
| `t` | Query term |
| `d` | Current document |
| `TF(t, d)` | Frequency of term `t` in document `d` |
| `IDF(t)` | Inverse document frequency of term `t` |
| `DL(d)` | Length of document `d` in tokens |
| `AvgDL` | Average document length |
| `k1` | Controls TF saturation |
| `b` | Controls document-length normalization |

The total BM25 score is the sum of the contributions from the query terms processed:

```text
BM25(query, document) = Sum of TermScore(t, document)
                         over the query terms
```

This corresponds to the following line in my code:

```python
score += idf * (tf * (k1 + 1)) / (
    tf + k1 * (1 - b + b * dl / avgdl)
)
```

### 6.1 The role of `k1`

My implementation uses:

```python
k1 = 1.5
```

This parameter controls how quickly the influence of term frequency saturates.

Increasing TF generally increases the term's contribution, but the benefit of additional occurrences diminishes as TF grows.

### 6.2 The role of `b`

My implementation uses:

```python
b = 0.75
```

This parameter controls the strength of document-length normalization.

- When `b = 0`, the document-length adjustment disappears.
- When `b = 1`, the full length adjustment is applied.
- When `b = 0.75`, the formula applies a substantial length adjustment.

Length normalization matters because longer documents have more opportunities to contain a query term simply because they contain more words.

### 6.3 How the variables work together

BM25 considers several signals:

1. Higher TF generally increases a term's contribution, with diminishing returns.
2. Higher IDF increases the weight of a matching term.
3. Document length influences the normalized TF contribution.
4. The contributions of the query terms are accumulated into the document's final score.

Therefore, a document is not ranked solely by the number of matching words. Its score also depends on how widely those words occur across the collection and how long the document is relative to the average.

---

## 7. A Complete BM25 Calculation

Consider the following documents:

```python
docs = [
    "python python search",
    "python search engine",
    "search engine"
]

query = "python"
```

### Step 1: Calculate the collection statistics

There are three documents:

```text
N = 3
```

Their token lengths are:

```text
Document 0: 3 tokens
Document 1: 3 tokens
Document 2: 2 tokens
```

Therefore:

```text
AvgDL = (3 + 3 + 2) / 3
      = 8 / 3
      ≈ 2.6667
```

### Step 2: Calculate TF and DF

| Document ID | Length | TF for `python` | DF for `python` |
|---|---:|---:|---:|
| 0 | 3 | 2 | 2 |
| 1 | 3 | 1 | 2 |
| 2 | 2 | 0 | 2 |

DF is the same in every row because it describes the term's distribution across the collection, not its frequency within an individual document.

### Step 3: Calculate IDF

```text
IDF(python) = ln(1 + (3 - 2 + 0.5) / (2 + 0.5))
            = ln(1.6)
            ≈ 0.4700
```

### Step 4: Calculate the score for document 0

Document 0 contains `python` twice.

Using `k1 = 1.5`, `b = 0.75`, `DL = 3`, and `AvgDL = 8/3`:

```text
TermScore(python, document 0)
    = 0.4700 * [2 * (1.5 + 1)]
      / [2 + 1.5 * (1 - 0.75 + 0.75 * 3 / (8/3))]

    ≈ 0.6463
```

### Step 5: Calculate the score for document 1

Document 1 contains `python` once:

```text
TermScore(python, document 1) ≈ 0.4700
```

### Step 6: Calculate the score for document 2

Document 2 does not contain `python`, so it receives a score of zero.

The resulting ranking is approximately:

```python
[
    (0, 0.6463),
    (1, 0.4700),
    (2, 0.0)
]
```

This example demonstrates how TF affects the ranking while IDF remains constant for the same term within a fixed collection.

Document 2 remains in the results because the function is required to return every document, including those with zero scores.

---

## 8. Explaining the `bm25_rank()` Function

The ranking function combines tokenization, collection statistics, the inverted index, BM25 scoring, and sorting.

### Step 1: Tokenize the query

```python
query_token = tokenize(query)
```

The query is converted into lowercase alphanumeric terms.

### Step 2: Tokenize the documents and record their lengths

```python
docs_tokens = []
doc_length = []

for doc in docs:
    tokens = tokenize(doc)
    docs_tokens.append(tokens)
    doc_length.append(len(tokens))
```

This stores the tokenized documents and the length of each document.

### Step 3: Calculate the collection statistics

```python
N = len(docs_tokens)
avgdl = sum(doc_length)/len(doc_length)
index = build_inverted_index(docs_tokens)
```

These statements calculate the total number of documents, average document length, and inverted index.

An empty document collection must be handled before calculating `avgdl`, otherwise the function raises `ZeroDivisionError`.

### Step 4: Initialize the results and score each document

```python
result = []

for doc_id in range(N):
    score = 0
```

Every document starts with a score of zero.

### Step 5: Process the query terms

```python
for term in query_token:
    if term in index:
```

The function checks whether each query term exists in the index. Terms absent from the collection contribute nothing and are skipped.

### Step 6: Retrieve TF, DF, and IDF

```python
tf = index[term].get(doc_id, 0)
df = len(index[term])

idf = math.log(
    1 + (N - df + 0.5) / (df + 0.5)
)
```

TF is specific to the current document, whereas DF and IDF depend on the collection.

### Step 7: Calculate and accumulate the BM25 contribution

```python
dl = doc_length[doc_id]

score += idf * (tf * (k1 + 1)) / (
    tf + k1 * (1 - b + b * dl / avgdl)
)
```

The function calculates the term's contribution and adds it to the current document's score.

### Step 8: Store and sort the results

```python
result.append((doc_id, score))

result.sort(key=lambda content: (-content[1], content[0]))

return result
```

Each document's score is stored as a tuple. The results are sorted by descending score, with ascending document ID used to break ties.

---

## 9. Why I Chose `.sort()` Instead of `sorted()`

Python provides two common ways to sort collections: the list method `.sort()` and the built-in function `sorted()`.

I chose `.sort()` because I was comfortable mutating the initial list of document scores.

Once the scores had been calculated, I no longer needed to preserve the order in which documents were processed. The list existed specifically to hold the final ranking.

### Using `.sort()`

```python
result.sort(key=lambda item: (-item[1], item[0]))
```

This sorts the existing list in place and returns `None`.

### Using `sorted()`

```python
ranked_result = sorted(
    result,
    key=lambda item: (-item[1], item[0])
)
```

This creates and returns a new sorted list, leaving the original list unchanged.

### Comparison

| Feature | `.sort()` | `sorted()` |
|---|---|---|
| Modifies the original list | Yes | No |
| Returns | `None` | New sorted list |
| Accepts arbitrary iterables | No, it is a list method | Yes |
| Appropriate for this task | Yes | Also valid |

### System-level justification

The result list is created locally inside `bm25_rank()`. No other part of the function depends on its original order after the scoring loop finishes.

Using `.sort()` therefore avoids creating a separate result list when the original list can safely be reordered.

The sorting key also establishes the required ranking contract:

```python
(-item[1], item[0])
```

- The negative score puts higher-scoring documents first.
- The document ID puts lower IDs first when scores are equal.

The decision is based on the ownership and intended use of the list, not on the assumption that `.sort()` is always faster or better than `sorted()`.

---

## 10. Lessons Learned

### Lesson 1: Data structures influence system behavior

The inverted index makes term-level information easier to retrieve. Its structure directly supports the calculation of TF, DF, and IDF.

### Lesson 2: TF and DF measure different things

TF counts occurrences within an individual document, while DF counts the number of documents containing a term. Confusing the two would lead to incorrect statistical calculations.

### Lesson 3: IDF provides collection-level context

A term's usefulness depends partly on how widely it is distributed across the collection. IDF helps distinguish common terms from more discriminative terms.

### Lesson 4: Relevance is a combination of signals

BM25 combines term frequency, inverse document frequency, and document-length normalization instead of relying only on the number of matching words.

### Lesson 5: Correct formulas do not guarantee robust software

The scoring formula can be implemented correctly while the surrounding function still fails on an empty document collection. Input contracts, edge cases, and tests are also part of correctness.

### Lesson 6: Sorting rules should be explicit

A composite sorting key produces deterministic results when scores are equal. This improves predictability and makes testing easier.

### Lesson 7: Mutation is a design decision

Using `.sort()` is appropriate because the result list does not need to preserve its original ordering. Mutation is reasonable when ownership and subsequent use make it safe.

### Lesson 8: Index construction and ranking have different responsibilities

Index construction depends on the document collection, whereas query-time ranking depends on the query and collection statistics. Separating these responsibilities can avoid repeated work.

---

## 11. Refactoring Opportunities and System Justification

Refactoring should address correctness, maintainability, or measurable system requirements rather than simply making code shorter.

### Priority 1: Handle an empty document collection

**Current code:**

```python
avgdl = sum(doc_length)/len(doc_length)
```

When `docs` is empty, the denominator is zero.

A reasonable policy for this function is to return an empty list before calculating the average:

```python
if not docs:
    return []
```

Alternatively, the application could reject empty collections with a deliberate `ValueError`. The important point is to define the function's contract.

### Priority 2: Define repeated-query-term behavior

The current loop processes every occurrence of a query term:

```python
for term in query_token:
```

Consequently, the query `"python python"` adds the contribution of `python` twice.

This may be intentional in a ranking model that incorporates query-term frequency, but the behavior should be explicit.

If each distinct query term should contribute only once, an ordered deduplication approach is:

```python
for term in dict.fromkeys(query_token):
```

This changes ranking semantics, so the expected behavior and tests should determine whether it is appropriate.

### Priority 3: Reuse the inverted index across queries

The current implementation tokenizes all documents and rebuilds the index on every call.

That is reasonable for a small coding exercise. However, a retrieval service that handles many queries against the same collection would repeat expensive collection-dependent work.

A more scalable design would separate index construction from query-time ranking and reuse the index, document lengths, and collection statistics.

This is the most consequential architectural improvement if the system is expected to serve multiple queries.

### Priority 4: Simplify frequency counting

The manual counting logic is correct, but Python's `Counter` can make its intention clearer:

```python
from collections import Counter

counts = Counter(tokens)
```

This is a readability improvement rather than a necessary correctness fix.

### Priority 5: Use `enumerate()` and clearer variable names

Instead of:

```python
for doc_id in range(len(docs_tokens)):
    tokens = docs_tokens[doc_id]
```

use:

```python
for doc_id, tokens in enumerate(docs_tokens):
```

Likewise, using names such as `term` and `frequency` can make dictionary operations easier to understand.

These are maintainability improvements, not fundamental algorithmic changes.

### Priority 6: Extract intermediate scoring variables

The current formula can be made easier to inspect by separating the normalization and TF contribution:

```python
length_norm = 1 - b + b * dl / avgdl

tf_weight = (
    tf * (k1 + 1)
) / (
    tf + k1 * length_norm
)

score += idf * tf_weight
```

This makes the code easier to compare with the mathematical definition and debug.

### Priority 7: Validate BM25 parameters

For conventional BM25 usage, `k1` should be non-negative and `b` should lie between 0 and 1.

A reusable component should validate these parameters and reject invalid or non-finite values rather than silently producing unintended scores.

### Priority 8: Precompute IDF values

For a fixed collection, a term's DF and IDF remain constant across documents.

The current implementation recalculates IDF inside the scoring loop. Precomputing IDF for the distinct query terms would avoid repeated calculations.

This becomes more valuable as the collection or query workload grows.

### Priority 9: Add tests for edge cases

I would test:

- An empty document collection.
- An empty query.
- Query terms absent from the index.
- Documents containing no tokens.
- Equal scores and document-ID tie-breaking.
- Repeated query terms.
- Repeated occurrences of a query term within a document.
- Invalid BM25 parameters, if validation is introduced.

Tests should capture the intended behavior before refactoring so that improvements do not accidentally change the function's contract.

---

## 12. Complexity Analysis

Let:

- `N` be the number of documents.
- `T` be the total number of tokens in the collection.
- `Q` be the number of query tokens.
- `C` be the total number of characters in the document collection.

### Tokenization

The tokenizer traverses the input characters. Its expected traversal cost is linear in the input size, although repeated immutable-string concatenation can increase the cost for very long tokens.

### Inverted-index construction

The index builder counts document terms and stores their frequencies. Assuming expected constant-time dictionary operations, the time complexity is:

```text
O(T)
```

### BM25 scoring

The outer loop visits every document, and the inner loop visits every query token.

The scoring phase therefore has time complexity:

```text
O(N * Q)
```

### Sorting

Sorting the `N` results has typical time complexity:

```text
O(N log N)
```

### Overall complexity

A simplified expected time-complexity bound is:

```text
O(C + T + N*Q + N log N)
```

This excludes query-tokenization cost, which is proportional to the query's character count, and assumes expected constant-time dictionary operations.

### System implications

The inverted index supports efficient term lookups, but the current implementation still evaluates every document for every query.

For a small collection, this is reasonable because it keeps the implementation straightforward and returns a score for every document.

For a larger system, the index could be reused across queries, and scoring could focus on documents containing query terms. If the function must still return every document, zero-score documents would need to be included in the final results.

---

## 13. Connection to Retrieval-Augmented Generation (RAG)

A simplified RAG pipeline looks like this:

```text
User Query
    |
    v
Query Processing
    |
    v
Document Retrieval
    |
    v
BM25 Ranking
    |
    v
Context Selection
    |
    v
Language Model
    |
    v
Generated Answer
```

BM25 helps determine which documents should be considered relevant enough to provide as context to a language model.

It is useful when exact terminology matters, such as searching technical manuals, equipment identifiers, code documentation, and error messages.

However, BM25 relies on lexical matching and statistical term weighting. It does not inherently understand synonyms or paraphrases.

For example, a query about preventing structural failure might be relevant to a document discussing proactive maintenance, even if the two expressions share few terms.

Dense vector retrieval can help identify semantically related passages. Combining lexical and dense retrieval produces hybrid search, while a reranker can further evaluate the relevance of retrieved candidates.

BM25 therefore provides a useful, interpretable baseline for understanding the retrieval component of a RAG system.

---

## 14. Further Experiments

To deepen my understanding, I would explore the following:

1. **Term-frequency saturation:** Compare documents with different TF values and observe how `k1` affects their scores.
2. **Document-length normalization:** Keep TF constant while changing document length and varying `b`.
3. **Document-frequency effects:** Add documents containing a query term and observe how its IDF changes.
4. **Repeated query terms:** Compare `"python"` with `"python python"` and decide whether repeated query terms should influence the score.
5. **Index reuse:** Compare rebuilding the index for every query against reusing a precomputed index.
6. **Hybrid retrieval:** Compare BM25 with dense retrieval for queries involving synonyms and paraphrases.

These experiments would help evaluate both retrieval quality and the computational trade-offs involved in building a search system.

---

## 15. Final Reflection

This exercise helped me connect Python fundamentals with the foundations of information retrieval.

I implemented a tokenizer, built an inverted index, calculated TF and DF, used IDF to weight matching terms, translated the BM25 formula into Python, and established deterministic ranking rules.

The most important conceptual lesson is that **relevance is not determined by term frequency alone**. BM25 considers how frequently a term occurs in a document, how widely it is distributed across the collection, and how long the document is relative to the average.

The most important engineering lesson is that a working algorithm is only one part of a reliable system. Edge cases, clear input contracts, reusable computations, maintainable code, and meaningful tests all matter.

My next step is to strengthen this baseline by handling empty inputs, formalizing repeated-query-term behavior, and separating collection indexing from query-time ranking. These improvements would provide a stronger foundation for exploring hybrid search and more advanced RAG architectures.

**Technologies and concepts:** Python · Tokenization · Dictionaries · Inverted Index · Term Frequency (TF) · Document Frequency (DF) · Inverse Document Frequency (IDF) · BM25 · Sorting · Complexity Analysis · Information Retrieval · RAG
