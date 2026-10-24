# Daily-Grind: Building a BM25 Search Ranker in Python

## 1. Project Overview

This project implements a keyword-based document ranking system using **BM25 (Best Matching 25)**, a probabilistic information retrieval algorithm used to estimate the relevance of documents to a search query.

The objective was to implement a basic search ranker from first principles using Python's built-in data structures, loops, and mathematical operations.

Rather than relying on an external search library, I implemented the following components:

- A tokenizer to convert text into searchable terms.
- An inverted index to map terms to documents and record their frequencies.
- A BM25 scoring function to calculate document relevance.
- A sorting mechanism to rank documents by their calculated scores.

This exercise also provides a foundation for understanding Retrieval-Augmented Generation (RAG), where relevant information is retrieved from a document collection before being supplied to a language model.

### Learning objectives

By completing this task, I aimed to understand how to:

- Tokenize queries and documents consistently.
- Build an inverted index using dictionaries and loops.
- Distinguish term frequency (TF), document frequency (DF), and inverse document frequency (IDF).
- Translate the BM25 mathematical formula into Python.
- Understand term-frequency saturation and document-length normalization.
- Sort documents by relevance using deterministic tie-breaking.
- Identify correctness issues, refactoring opportunities, and scalability considerations.

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

The function works in five steps:

1. Initializes an empty list called `tokens` and a string called `current`.
2. Converts the input to lowercase and iterates through each character.
3. Appends alphanumeric characters to the current token.
4. When a separator appears, appends the completed token and resets `current`.
5. Appends the final token after the loop if one remains.

The final step is necessary because the text may end with a letter or digit rather than punctuation.

The tokenizer ensures that `Python`, `PYTHON`, and `python` are treated as the same term. It also splits punctuation-separated expressions, such as `data-driven`, into `data` and `driven`.

**Key lesson:** Tokenization defines what the retrieval system considers a term. Since both queries and documents use the same tokenizer, they follow consistent normalization rules.

The implementation is intentionally simple. It does not perform stemming, lemmatization, or stop-word removal, and it treats punctuation as a token boundary. These are design choices rather than necessarily incorrect behavior.

## 4. Understanding the Inverted Index

An inverted index maps each unique term to the documents containing it and the frequency of that term in each document.

Consider this collection:

```python
docs_tokens = [
    ["python", "python", "search"],
    ["python", "search", "engine"],
    ["search", "engine"]
]
```

The resulting index is:

```python
{
    "python": {0: 2, 1: 1},
    "search": {0: 1, 1: 1, 2: 1},
    "engine": {1: 1, 2: 1}
}
```

The structure can be interpreted as follows:

- The outer dictionary key is a term.
- The inner dictionary key is a document ID.
- The inner dictionary value is the number of occurrences of the term in that document.

For example:

```python
"python": {0: 2, 1: 1}
```

means that `python` occurs twice in document 0 and once in document 1.

### How my index-building function works

The function first iterates through each document and creates a temporary dictionary called `counts`.

```python
counts = {}

for term in tokens:
    if term in counts:
        counts[term] += 1
    else:
        counts[term] = 1
```

This calculates the frequency of each unique term in the current document.

It then transfers those counts into the global index:

```python
for key in counts:
    if key not in index:
        index[key] = {}
    index[key][doc_id] = counts[key]
```

The two-stage process separates counting terms within a document from storing their frequencies across the collection.

### Why this structure matters

The inverted index provides the information required to calculate TF and DF.

- TF is retrieved from the inner dictionary using a document ID.
- DF is calculated by counting the number of document entries for a term.

This is a central design decision in the solution: the data structure is organized around terms because the ranking algorithm needs to determine which documents contain each query term.

## 5. TF, DF, and IDF: The Three Concepts Behind BM25

These three measurements are fundamental to understanding the scoring formula.

Although they are related, they answer different questions.

<text> </text>

### 5.1 Term Frequency (TF)

**Term frequency measures how many times a particular term occurs within a particular document.**

In your implementation:

```python
tf = index[term].get(doc_id, 0)
```

Suppose the query is `python` and the document collection is:

```python
docs = [
    "python python search",
    "python search engine",
    "search engine"
]
```

The term frequencies are:

| Document | Occurrences of `python` | TF |
|---|---|---:|
| Document 0 | `python python` | 2 |
| Document 1 | `python` | 1 |
| Document 2 | None | 0 |

TF is document-specific. It tells the ranking function how often the query term appears in the document being scored.

#### Why TF matters

If a document mentions a query term several times, it may be more relevant to that query than a document mentioning it only once.

However, repetition alone is not a reliable measure of relevance. A document should not necessarily rank dramatically higher simply because it repeats the same word many times.

This is why BM25 incorporates TF into a saturation formula rather than using the raw count as the entire relevance score.

**In my code, TF determines the strength of the term's contribution to the current document's score.**

### 5.2 Document Frequency (DF)

**Document frequency measures the number of distinct documents in the collection that contain a particular term.**

In your implementation:

```python
df = len(index[term])
```

Using the same document collection:

```python
docs = [
    "python python search",
    "python search engine",
    "search engine"
]
```

The document frequencies are:

| Term | Documents containing the term | DF |
|---|---|---:|
| `python` | 0, 1 | 2 |
| `search` | 0, 1, 2 | 3 |
| `engine` | 1, 2 | 2 |

Notice the distinction between TF and DF.

The term `python` occurs twice in document 0, but that does not mean its DF is 2 because it occurs twice. Its DF is 2 because the term appears in two distinct documents.

Similarly, `search` appears once in each of three documents, so its DF is 3.

#### Why DF matters

DF tells us how widespread a term is across the collection.

A term appearing in almost every document may be less useful for distinguishing one document from another. A term appearing in only a small number of documents may provide a stronger signal about which documents are relevant.

DF is therefore not a direct measure of the relevance of an individual document. Instead, it supplies a collection-level statistic used to calculate IDF.

**In my code, DF determines how informative a query term is relative to the entire document collection.**

### 5.3 Inverse Document Frequency (IDF)

**Inverse document frequency uses document frequency to estimate how informative a term is across the collection.**

In simple terms, it helps distinguish common terms from relatively rare terms.

Your implementation calculates IDF using:

```python
idf = math.log(
    1 + (N - df + 0.5) / (df + 0.5)
)
```

The formula is:

\[
\operatorname{IDF}(t)=
\ln\left(
1+\frac{N-df(t)+0.5}{df(t)+0.5}
\right)
\]

where:

- \(N\) is the total number of documents.
- \(df(t)\) is the number of documents containing term \(t\).
- \(\ln\) is the natural logarithm.

The constants \(0.5\) are part of this smoothed BM25 IDF variant. The additional \(1\) inside the logarithm keeps the IDF non-negative for terms found in the collection.

#### Working through the calculation

Suppose there are three documents:

\[
N=3
\]

For the term `python`, which appears in two documents:

\[
df(\text{python})=2
\]

Substituting these values:

\[
\operatorname{IDF}(\text{python})
=
\ln\left(
1+\frac{3-2+0.5}{2+0.5}
\right)
\]

\[
=\ln(1.6)\approx 0.4700
\]

For `search`, which appears in all three documents:

\[
\operatorname{IDF}(\text{search})
=
\ln\left(
1+\frac{3-3+0.5}{3+0.5}
\right)
\]

\[
=\ln(1.142857)\approx 0.1335
\]

The IDF of `python` is higher than that of `search` because `python` appears in fewer documents.

This does not mean `python` is always more important than `search`. Its relative importance depends on the collection's document distribution.

#### Why IDF matters

Imagine searching a large collection of technical documents for the word `system`. If the word appears in almost every document, finding it provides little information about which document is most relevant.

Now consider a more specialized term such as `galvanization`. If that term occurs in only a small proportion of documents, its presence may help identify a document about corrosion protection.

IDF helps the ranking function account for this difference.

**In my code, IDF weights the contribution of each matching term according to how widely it is distributed across the collection.**

### 5.4 TF vs DF vs IDF

| Measurement | Question answered | Scope | Role in BM25 |
|---|---|---|---|
| TF | How often does the term occur in this document? | Individual document | Determines the term's frequency contribution |
| DF | How many documents contain the term? | Entire collection | Supplies the statistic used to calculate IDF |
| IDF | How informative is the term across the collection? | Entire collection | Weights the term's contribution to relevance |

These measurements work together.

TF considers the term's frequency in the current document. DF tells us how common the term is across the collection. IDF converts that collection-level information into a weight used by BM25.

A crucial distinction is that **DF is not itself the relevance weight**. It is an input to IDF, which then contributes to the final score.

### 5.5 How my inverted index supports all three concepts

Consider:

```python
index = {
    "python": {0: 2, 1: 1},
    "search": {0: 1, 1: 1, 2: 1}
}
```

For the term `python` in document 0:

```python
tf = index["python"].get(0, 0)
# 2

df = len(index["python"])
# 2
```

The total document count is:

```python
N = 3
```

The IDF can then be calculated from `N` and `df`.

This shows why the inverted index is more than a storage structure. It is the foundation from which the ranking function obtains the statistics it needs.

## 6. Understanding the BM25 Formula

BM25 combines IDF, TF, and document-length normalization.

Your implementation uses the following contribution for each query term:

\[
\operatorname{Score}(t,d)=
\operatorname{IDF}(t)
\frac{tf(t,d)(k_1+1)}
{tf(t,d)+k_1\left(1-b+b\frac{dl}{avgdl}\right)}
\]

The final score is accumulated across the query terms processed by the implementation:

\[
\operatorname{BM25}(q,d)
=
\sum_{t\in q}\operatorname{Score}(t,d)
\]

Here:

- \(t\) is a query term.
- \(d\) is the current document.
- \(tf(t,d)\) is the term frequency.
- \(\operatorname{IDF}(t)\) is the inverse document frequency.
- \(dl\) is the current document's token count.
- \(avgdl\) is the average document length.
- \(k_1\) controls TF saturation.
- \(b\) controls document-length normalization.

### 6.1 The role of `k1`

Your default is:

```python
k1 = 1.5
```

The parameter controls how quickly the effect of TF saturates.

Increasing TF increases the term's contribution, but the benefit of additional occurrences diminishes as TF grows.

This prevents the ranking function from treating every additional repetition as equally valuable.

### 6.2 The role of `b`

Your default is:

```python
b = 0.75
```

The parameter controls the strength of document-length normalization.

- When `b = 0`, the document-length adjustment disappears from the TF normalization factor.
- When `b = 1`, the full length adjustment is applied.
- At `b = 0.75`, the formula applies a substantial length adjustment.

Document-length normalization matters because longer documents have more opportunities to contain a query term simply because they contain more words.

### 6.3 How TF, DF, and IDF interact with the other variables

The final score reflects several influences at once:

1. A higher TF generally increases the contribution, with diminishing returns.
2. A higher IDF increases the weight of a matching term.
3. Document length affects the normalized TF contribution.
4. The contributions of the query terms are accumulated to calculate the document's score.

A document is therefore not ranked solely by how often a word occurs. Its score also depends on how common the word is across the collection and how long the document is.

## 7. A Complete BM25 Example

Consider:

```python
docs = [
    "python python search",
    "python search engine",
    "search engine"
]

query = "python"
```

The collection statistics are:

\[
N=3,\qquad avgdl=\frac{8}{3}
\]

The document lengths are:

| Document ID | Length | TF for `python` | DF for `python` |
|---|---:|---:|---:|
| 0 | 3 | 2 | 2 |
| 1 | 3 | 1 | 2 |
| 2 | 2 | 0 | 2 |

The document frequency is the same for all three rows because it describes the term's occurrence across the collection, not within the individual document.

The IDF is approximately:

\[
\operatorname{IDF}(\text{python})=0.4700
\]

For document 0, the term occurs twice:

\[
\operatorname{Score}(\text{python},d_0)
\approx 0.6463
\]

For document 1, the term occurs once:

\[
\operatorname{Score}(\text{python},d_1)
\approx 0.4700
\]

Document 2 does not contain the term, so its score is zero.

The expected ranking, with scores rounded for presentation, is:

```python
[
    (0, 0.6463),
    (1, 0.4700),
    (2, 0.0)
]
```

This example demonstrates how TF affects the ranking while IDF remains constant for the same term across the collection.

The third document remains in the output because the task requires the function to return all documents, including those with zero scores.

## 8. Explaining the `bm25_rank()` Function

The ranking function brings together the components described above.

### Step 1: Tokenize the query

```python
query_token = tokenize(query)
```

The query is converted into a list of normalized terms.

### Step 2: Tokenize the documents and record their lengths

```python
docs_tokens = []
doc_length = []

for doc in docs:
    tokens = tokenize(doc)
    docs_tokens.append(tokens)
    doc_length.append(len(tokens))
```

The function stores the tokenized documents and calculates the length of each document.

### Step 3: Calculate collection statistics

```python
N = len(docs_tokens)
avgdl = sum(doc_length)/len(doc_length)
index = build_inverted_index(docs_tokens)
```

These lines calculate the number of documents, average document length, and inverted index.

The empty-collection case needs to be handled before calculating `avgdl`; otherwise, division by zero occurs.

### Step 4: Score every document

```python
result = []

for doc_id in range(N):
    score = 0
```

The outer loop visits every document and initializes its score to zero.

### Step 5: Process each query term

```python
for term in query_token:
    if term in index:
```

Only terms present in the index enter the scoring calculation.

### Step 6: Retrieve TF, DF, and IDF

```python
tf = index[term].get(doc_id, 0)
df = len(index[term])
idf = math.log(1 + (N - df + 0.5) / (df + 0.5))
```

These lines implement the central statistical part of the algorithm.

TF is specific to the current document, whereas DF and IDF depend on the collection.

### Step 7: Apply document-length normalization

```python
dl = doc_length[doc_id]

score += idf * (tf * (k1 + 1)) / (
    tf + k1 * (1 - b + b * dl / avgdl)
)
```

The score accumulates the BM25 contribution of the current query term.

### Step 8: Store and sort the results

```python
result.append((doc_id, score))

result.sort(key=lambda content: (-content[1], content[0]))

return result
```

The function records every document's score, sorts the results by descending score, and uses ascending document ID as a tie-breaker.

## 9. Why I Chose `.sort()` Instead of `sorted()`

Python provides two common approaches to sorting: the list method `.sort()` and the built-in function `sorted()`.

My implementation uses `.sort()` because I was comfortable mutating the initial list of document scores.

After calculating the scores, I did not need to preserve the order in which documents were processed. The list existed specifically to hold the final ranking.

### `.sort()` — modifies the existing list

```python
result.sort(key=lambda item: (-item[1], item[0]))
```

The list is reordered in place, and the method returns `None`.

### `sorted()` — returns a new list

```python
ranked_result = sorted(
    result,
    key=lambda item: (-item[1], item[0])
)
```

The original list remains unchanged, and a separate sorted list is returned.

| Feature | `.sort()` | `sorted()` |
|---|---|---|
| Modifies the original list | Yes | No |
| Returns | `None` | New sorted list |
| Accepts arbitrary iterables | No, it is a list method | Yes |
| Appropriate for this task | Yes | Also valid |

### Why `.sort()` is justified here

The result list is created locally inside `bm25_rank()`. Nothing else in the function depends on its original ordering after the scoring loop finishes.

Using `.sort()` therefore avoids creating a separate result list when the original list can safely be reordered.

The key:

```python
(-item[1], item[0])
```

also establishes the required ranking contract:

- Negative scores sort higher-scoring documents first.
- Document IDs sort in ascending order when scores are equal.

The decision is based on the ownership and intended use of the list, not on the assumption that `.sort()` is always faster or better than `sorted()`.

## 10. Lessons Learned

### Lesson 1: Data structures influence system behavior

The inverted index makes it possible to retrieve term-level information efficiently.

Its design directly supports TF, DF, and IDF calculations. This reinforced the importance of choosing data structures based on the operations the system needs to perform.

### Lesson 2: TF and DF must not be confused

TF measures occurrences within an individual document. DF counts the number of distinct documents containing a term.

The distinction is essential because a word can occur many times in one document but still have a low document frequency across the collection.

### Lesson 3: IDF provides collection-level context

A term's importance depends partly on how widely it is distributed across the document collection.

IDF provides that context, helping BM25 distinguish common terms from more discriminative terms.

### Lesson 4: Relevance is a combination of signals

BM25 combines term frequency, inverse document frequency, and document-length normalization.

This is more informative than ranking documents based only on the number of matching words.

### Lesson 5: Mathematical correctness is only one part of software correctness

The formula can be implemented correctly while the function still fails on an empty document collection.

Input validation, edge cases, and clear contracts are therefore important parts of implementing an algorithm reliably.

### Lesson 6: Sorting rules should be explicit

Using a composite key ensures that the ranking is deterministic when scores are equal.

This is valuable for predictable test results and reproducible behavior.

### Lesson 7: Mutation is a design decision

Using `.sort()` is appropriate because the result list does not need to preserve its initial ordering.

The broader lesson is that modifying an object is acceptable when its ownership and subsequent use make that behavior safe.

### Lesson 8: Index construction and ranking have different responsibilities

Index construction depends on the document collection, while query-time ranking depends on the query and collection statistics.

Separating these responsibilities can prevent unnecessary work when the same collection is queried repeatedly.

## 11. Refactoring Opportunities and System Justification

The objective of refactoring is not simply to shorten the code. Improvements should address correctness, maintainability, or measurable system requirements.

### Priority 1: Handle an empty document collection

**Current code:**

```python
avgdl = sum(doc_length)/len(doc_length)
```

When `docs` is empty, this raises `ZeroDivisionError`.

A reasonable policy for this function is to return an empty list:

```python
if not docs:
    return []
```

This check should occur before calculating the average document length.

Alternatively, the application could explicitly reject empty collections with `ValueError`. The important point is to define the contract.

### Priority 2: Define repeated-query-term behavior

The current loop processes every occurrence of a query term:

```python
for term in query_token:
```

As a result, `"python python"` adds the contribution of `python` twice.

This may be intentional in a model that uses query-term frequency, but it should not happen accidentally.

If each unique query term should contribute only once, an ordered deduplication approach is:

```python
for term in dict.fromkeys(query_token):
```

This changes scoring behavior, so the expected task semantics and tests should determine whether it is appropriate.

### Priority 3: Reuse the inverted index for multiple queries

The current implementation tokenizes all documents and builds the inverted index every time `bm25_rank()` is called.

That is reasonable for a small exercise. For a service handling many queries against a stable collection, it repeats expensive collection-dependent work.

A more scalable design would separate index construction from ranking and reuse the index and document statistics across queries.

This is the most significant architectural improvement if the system is expected to serve multiple queries.

### Priority 4: Simplify frequency counting

The manual frequency-counting logic is correct, but Python's `Counter` can make its intention more explicit:

```python
from collections import Counter

counts = Counter(tokens)
```

This is a readability and maintainability improvement, not a necessary correctness fix.

### Priority 5: Use `enumerate()` and clearer names

Instead of:

```python
for doc_id in range(len(docs_tokens)):
    tokens = docs_tokens[doc_id]
```

use:

```python
for doc_id, tokens in enumerate(docs_tokens):
```

Likewise, `term` and `frequency` are more descriptive than `key` and `counts[key]` when iterating through the counts dictionary.

These changes improve readability without changing the algorithm.

### Priority 6: Extract intermediate scoring variables

The current BM25 expression is mathematically valid, but its components could be made easier to inspect:

```python
length_norm = 1 - b + b * dl / avgdl

tf_weight = (
    tf * (k1 + 1)
) / (
    tf + k1 * length_norm
)

score += idf * tf_weight
```

This makes the implementation easier to compare with the formula and simplifies debugging.

### Priority 7: Validate BM25 parameters

A reusable component should reject invalid parameter values rather than silently produce unexpected scores.

For conventional BM25 usage, `k1` should be non-negative and `b` should lie between 0 and 1. An implementation could validate these conditions and reject non-finite parameter values.

### Priority 8: Precompute IDF values

For a fixed document collection, a term's DF and IDF do not change between documents.

The current implementation recalculates IDF inside the nested scoring loop. Precomputing the IDF for each distinct query term would avoid repeated calculations.

This is a worthwhile optimization when the collection or query workload grows, but it is not necessary to understand or validate the core algorithm.

### Priority 9: Add tests for edge cases

I would test:

- An empty document collection.
- An empty query.
- Query terms absent from the index.
- Documents containing no tokens.
- Equal scores and document-ID tie-breaking.
- Repeated query terms.
- Repeated occurrences of a query term within a document.
- Invalid parameter values, if validation is implemented.

Tests should capture the intended behavior before refactoring. This makes it possible to improve the implementation without accidentally changing its contract.

## 12. Complexity Analysis

Let:

- \(N\) be the number of documents.
- \(T\) be the total number of tokens in the document collection.
- \(Q\) be the number of query tokens.
- \(C\) be the total number of characters in the input documents.

Ignoring the extra cost of repeated immutable-string concatenation, tokenization is linear in the input character count, or \(O(C)\).

Building the inverted index takes expected \(O(T)\) time because the implementation processes each token and then visits the unique terms in each document.

The scoring loop examines every document against every query token:

\[
O(NQ)
\]

Sorting the results takes:

\[
O(N\log N)
\]

Therefore, a simplified expected time-complexity bound is:

\[
O(C+T+NQ+N\log N)
\]

This excludes query-tokenization cost, which is proportional to the query's character count, and assumes expected constant-time dictionary operations.

### What this means for the system

The inverted index supports efficient term lookups, but the implementation still evaluates every document for every query.

For a small collection, this is a reasonable design because it is straightforward and returns a score for every document.

For a larger system, the index could be reused across queries and the scoring process could focus on documents containing query terms. If the interface still requires every document to be returned, zero-score documents would need to be included in the final results.

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

BM25 helps determine which documents should be considered relevant enough to provide to a language model.

It is particularly useful when exact terminology matters, such as searching technical manuals, equipment identifiers, code documentation, and error messages.

However, BM25 relies on lexical matching and statistical term weighting. It does not inherently understand synonyms or paraphrases.

A query about preventing structural failure may be relevant to a document discussing proactive maintenance even when the two expressions share few terms.

Dense vector retrieval can help identify semantically related passages. Combining lexical and dense retrieval produces hybrid search, while a reranker can further assess the relevance of retrieved candidates.

BM25 therefore provides a useful, interpretable baseline for understanding the retrieval component of a RAG system.

## 14. Further Experiments

To deepen my understanding, I would explore the following experiments:

1. **Term-frequency saturation:** Compare documents with different TF values and observe how `k1` affects their scores.
2. **Document-length normalization:** Keep TF constant while changing document length and varying `b`.
3. **Document-frequency effects:** Add documents containing a query term and observe how its IDF changes.
4. **Repeated query terms:** Compare `"python"` with `"python python"` and decide whether repeated query terms should influence the score.
5. **Index reuse:** Compare rebuilding the index for each query against reusing a precomputed index.
6. **Hybrid retrieval:** Compare BM25 results with dense retrieval results for queries involving synonyms and paraphrases.

These experiments would help evaluate both retrieval quality and the trade-offs involved in implementing a search system.

## 15. Final Reflection

This exercise helped me connect Python fundamentals with the foundations of information retrieval.

I implemented a tokenizer, built an inverted index, calculated TF and DF, used IDF to weight matching terms, translated the BM25 formula into Python, and established deterministic ranking rules.

The most important conceptual lesson is that **relevance is not determined by term frequency alone**. BM25 considers how frequently a term occurs in a document, how widely it is distributed across the collection, and how long the document is relative to the average.

The most important engineering lesson is that a working algorithm is only one part of a reliable system. Edge cases, clear input contracts, reusable computations, maintainable code, and meaningful tests all matter.

My next step is to strengthen this baseline by handling empty inputs, formalizing repeated-query-term behavior, and separating collection indexing from query-time ranking. These improvements would provide a stronger foundation for exploring hybrid search and more advanced RAG architectures.

**Technologies and concepts:** Python · Tokenization · Dictionaries · Inverted Index · Term Frequency (TF) · Document Frequency (DF) · Inverse Document Frequency (IDF) · BM25 · Sorting · Complexity Analysis · Information Retrieval · RAG
