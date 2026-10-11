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
├── C/
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
| 2026-09-03 | [Fix Player Positions (Pointers)](C/2026-09-03-Fix-Player-Position-Pointers/) | C | pointers, pointer arithmetic, dereferencing (`*ptr`), address-of (`&`), `NULL` as "no valid address", in-place array mutation, C99 vs C89 for-loop declarations |
| 2026-09-04 | [Fix snek_add](C/2026-09-04-Fix/) | C | struct, tagged unions, `enum`-based type discrimination, heap allocation with `malloc`/`free`, manual buffer-length arithmetic, `memcpy` vs `strcpy`, defensive NULL checks, AddressSanitizer / UndefinedBehaviorSanitizer |
| 2026-09-05 | [Pointer Practice: Arrays and Structs](C/2026-09-05-Pointer-Practise-Array-Structs/) | C | pointer traversal (`p++`, `*p`), half-open ranges `[begin, end)`, pointer-as-iterator vs array indexing, pointer subtraction (`end - begin`), struct pointers and `->`, sentinel-return pattern, AddressSanitizer / UndefinedBehaviorSanitizer |
| 2026-09-06 | [Split Words with Pointers](C/2026-09-06-Split-Words-With-Pointers/) | C | `char **` (array of C strings), two-pass string scanning, pointer subtraction for length, jagged heap arrays, manual null-termination, defensive bounds against pass disagreement |
| 2026-09-07 | [Fix Inventory Pointers](C/2026-09-07-Fix-Inventory-Pointers/) | C | nested heap allocation, struct owning a heap array, bounded string copying, stack-to-heap struct copy, never returning pointers to stack memory, cleanup on partial failure, `free` |
| 2026-09-08 | [Fix Stat Buff Pointers](C/2026-09-08-Fix-Staff-Buff-Pointers/) | C | pointer-returning search (`&stats[i]`), two-pass count-then-allocate, output-parameter resets on every return path, `malloc(count * sizeof(int))` |
| 2026-09-09 | [Dynamic Int Array](C/2026-09-09-Dynamic-Int-Array/) | C | `malloc`/`realloc`/`free`, length vs. capacity, geometric (doubling) growth, shifting elements on insert, failure-safe `realloc` |
| 2026-09-10 | [Traverse Arrays with Pointers](C/2026-09-10-Traverse-Arrays-with-Pointers/) | C | array decay to pointer, `size_t` for lengths, signed/unsigned comparison pitfalls (`-Wsign-compare`), null-terminator traversal, indexing vs. pointer-walking |
| 2026-09-11 | [Analyze Sensor Readings with Pointers](C/2026-09-11-Analyze-Sensor-Readings-with-Pointers/) | C | combined index-and-pointer loops, half-open range `[start, end)` with two pointers, in-place mutation through a pointer, pure-pointer traversal |
| 2026-09-12 | [Fix Quest Pointers](C/2026-09-12-Fix-Quest-Pointers/) | C | direct heap writes (no stack intermediate), shallow pointer storage vs. deep string copy, consistent `NULL` guards, `const` read-only accessors |
| 2026-09-13 | [Filter With Pointers](C/2026-09-13-Filter-With-Pointers/) | C | read/write cursor (slow-fast) pattern, in-place array and string compaction, order-preserving filtering, manual null-termination |
| 2026-09-14 | [Implement a Pointer-Based Playlist](C/2026-09-14-Implement-a-Pointer-Based-Playlist/) | C | singly linked lists, pointer-to-pointer (`Song **`), heap ownership, pointer rewiring without a "previous" pointer, list cleanup |
| 2026-09-15 | [Compact Spaces (Pointer Arithmetic)](C/2026-09-15-Compact-Spaces-Pointer-Arithmetic/) | C | pointer arithmetic, read/write cursors, in-place string mutation, array decay, stack buffers, null termination |
| 2026-09-16 | [Pointer Workshop: Array Stats and Clamping](C/2026-09-16-Pointer-Workshop-ArrayStats-Clamping/) | C | pointer arithmetic over arrays, writing through a struct pointer, `const`-correctness, signed/unsigned comparison |

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

**C language**
`pointers` · `pointer arithmetic` · `dereferencing` · `address-of (&)` · `structs` · `tagged unions` · `enum`-based type discrimination · `malloc`/`free` heap allocation · `memcpy` vs `strcpy` · `NULL` checks · AddressSanitizer / UndefinedBehaviorSanitizer · `pointer-as-iterator` · half-open ranges `[begin, end)` · pointer subtraction · struct pointers & `->` · sentinel-return pattern · `char **` arrays of strings · two-pass scan-then-allocate · nested heap ownership · cleanup on partial allocation failure · returning pointers into arrays (`&a[i]`) · output parameters · `realloc` & failure-safe growth · length vs. capacity · geometric growth · array decay · `size_t` & signed/unsigned pitfalls (`-Wsign-compare`) · read/write cursor (slow-fast) pattern · in-place array/string compaction · shallow vs. deep string copy · `const`-correctness · singly linked lists · pointer-to-pointer (`**`) · pointer rewiring

**Software practices**
`error handling` · `input validation` · `guard clauses` · `defensive programming` · `shallow vs deep copy` · `input-mutation prevention` · `immutability` · `safe out-of-bounds handling` · `path-traversal security` · `separation of concerns` · `self-documenting code`

---

*New entries added daily. The best write-ups get republished to my blog.*