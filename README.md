# Daily Grind — Problem Solving Log

A daily log of coding problems solved across **Go, Python, C, and SQL**, featuring technical write-ups that explain solutions, challenges encountered, and lessons learned.

> "The goal isn't just solving problems — it's *understanding* them well enough to explain them."

## Why this repo exists

- 📈 **Consistency** — one problem daily, documented systematically.
- ✍️ **Understanding** — documentation serves as verification of comprehension.
- 🐛 **Debugging skill** — write-ups capture failures and root causes, not just successes.
- 🗂️ **Portfolio** — a growing, verifiable record of problem-solving across multiple languages.

## Structure

Each problem occupies its own folder organized by language and date:

```
daily-grind/
├── Go/
│   └── 2026-06-12-concurrent-task-runner/
│       ├── solution.go
│       ├── solution_test.go
│       └── README.md      ← the write-up
├── Python/
├── SQL/
├── c/                     ← planned
└── TEMPLATE.md            ← write-up template for every entry
```

## Index

| Date | Problem | Language | Key Concepts |
|------|---------|----------|--------------|
| 2026-06-12 | [Concurrent Task Runner](Go/2026-06-12-concurrent-task-runner/) | Go | goroutines, channels, worker pools, mutex, generics |
| 2026-08-09 | [Build Overlapping Token Chunks](Python/2026-08-09-Build-Overlapping-Tokens-Chunks/) | Python | while loops, list slicing, type hints, error handling, input validation, iteration, overlapping windows, boundary conditions |
| 2026-08-10 | [Pull Request Merge Gate](Python/2026-08-10-Pull-Request-Merge-Gate/) | Python | dictionary, list, set, for loops, match-case, enumerate, indexing |
| 2026-08-12 | [Course Enrollment Coverage](SQL/2026-08-12-Course-Enrollment/) | SQL | tables, JOINs, GROUP BY, ORDER BY |
| 2026-08-13 | [Fix the Record Processing Pipeline](Python/2026-08-13-Fix-the-Record-Processing-Pipeline/) | Python | dictionaries, loops, data types, list comprehensions |
| 2026-08-14 | [Mailroom Receivers](Go/2026-08-14-Mailroom-Receivers/) | Go | value receivers, pointer receivers, mutability, nil-pointer safety, automatic referencing & dereferencing, method sets, struct copy cost |
| 2026-08-15 | [Fix Nested Type Hints](Python/2026-08-15-Fix-Nested-Type-Hints/) | Python | nested container types, type annotations, union & optional types, dict comprehensions, mypy static checking, type-inference limits |
| 2026-08-16 | [Fix List Slice Helpers](Python/2026-08-16-Fix-List-Slice-Helpers/) | Python | list slicing, `[start:stop:step]` syntax, negative indexing, sequence operations, immutability, safe out-of-bounds handling |
| 2026-08-17 | [Fix BM25 Score](Python/2026-08-17-Fix-BM25-Score/) | Python | information-retrieval algorithms, term-frequency saturation, IDF, length normalization, tokenization, guard clauses, corpus statistics |
| 2026-08-18 | [Build Your Own Map / Filter / Reduce](Python/2026-08-18-Build-Your-Own-Map-Filter-Reduce/) | Python | higher-order functions, first-class functions, closures, generic type variables, callable type hints, accumulator pattern, function composition |
| 2026-08-20 | [Minimum Hops with BFS](Python/2026-08-20-Minimum-Hops-with-BFS/) | Python | breadth-first search, adjacency lists, graph traversal, `collections.deque`, visited-set management, shortest-path |
| 2026-08-21 | [Build Simple TF-IDF Scores](Python/2026-08-21-Build-Simple-TF-IDF-Scores/) | Python | dictionary operations, set deduplication, case-insensitive text processing, inverted indexing, corpus statistics, single-pass iteration, type hints |
| 2026-08-22 | [Annotate Inventory Tools](Python/2026-08-22-Annotate-Inventory-Tools/) | Python | type hints, parameter & return annotations, generic types (`list[int]`), static type checking, self-documenting code |
| 2026-08-23 | [Build Semantic Chunks](Python/2026-08-23-Build-Semantic-Chunks/) | Python | list slicing, token positions, overlap windows, `enumerate()`, range vs while loops, list comprehension, `zip()`, boundary computation, separation of concerns |
| 2026-08-24 | [Fix the Inventory Summary](Python/2026-08-24-Fix-The-Inventory-Summary/) | Python | type hints, `Optional[T]`, `None`-handling, shallow vs deep copy, input-mutation prevention, nested dictionaries, `dict.get()` defaults, defensive programming |
| 2026-08-25 | [Validate Project Path](Python/2026-08-25-Validate-Project-Path/) | Python | `os.path`, path normalization (`normpath`/`abspath`/`join`), `commonpath` prefix checking, directory containment, relative vs absolute paths, security pitfalls, exception handling |

## Topics covered so far

**Python language & typing**
`type hints` · `parameter & return annotations` · `nested type hints` · `Optional[T]` · `union types` · `generics (TypeVar)` · `callable type hints` · `mypy static checking` · `match-case` · `list comprehensions` · `dict comprehensions` · `enumerate` · `zip` · `list slicing [start:stop:step]` · `negative indexing` · `while vs range loops` · `closures` · `higher-order & first-class functions`

**Data structures**
`lists` · `dictionaries` · `sets` · `nested dictionaries` · `adjacency lists` · `collections.deque`

**Algorithms & problem-solving**
`breadth-first search (BFS)` · `graph traversal` · `shortest-path` · `overlapping windows` · `boundary conditions` · `accumulator pattern` · `map / filter / reduce` · `function composition` · `single-pass iteration`

**Information retrieval / RAG**
`BM25` · `TF-IDF` · `term-frequency saturation` · `IDF` · `length normalization` · `tokenization` · `inverted indexing` · `semantic chunking` · `corpus statistics`

**Concurrency (Go)**
`goroutines` · `channels` · `worker pools` · `sync.Mutex` · `sync.WaitGroup` · `generics`

**Go language**
`value vs pointer receivers` · `method sets` · `nil-pointer safety` · `mutability` · `automatic referencing & dereferencing` · `struct copy cost`

**SQL**
`tables` · `INNER JOIN` · `LEFT JOIN` · `RIGHT JOIN` · `FULL JOIN` · `GROUP BY` · `COUNT` · `ORDER BY`

**Software practices**
`error handling` · `input validation` · `guard clauses` · `defensive programming` · `shallow vs deep copy` · `input-mutation prevention` · `immutability` · `safe out-of-bounds handling` · `path-traversal security` · `separation of concerns` · `self-documenting code`

---

*New entries added daily. The best write-ups get republished to my blog.*