# Course Enrollment Coverage — Picking the Right JOIN

**Date:** 2026-08-12
**Language:** SQL
**Source:** boot.dev
**Concepts:** `INNER JOIN` · `LEFT JOIN` · `RIGHT JOIN` · `FULL JOIN` · `GROUP BY` · `COUNT` · `ORDER BY`

---

## 🎯 The Problem (one sentence)

Build a report showing **every course** and its student enrollment count — including courses with zero enrollments — sorted by count descending, then title ascending.

---

## 🧩 My First Approach

The query was almost fully written. I only had to replace `__JOIN_TYPE__` with the correct keyword. My first instinct was `FULL JOIN` — I reasoned that "I want everything from both tables," and full felt like the most complete option. It passed the given test cases and I moved on.

It was only when I compared against the boot.dev solution that I saw the correct answer was `LEFT JOIN` — one word different, and the reasoning behind that difference is the entire lesson.

---

## 💥 What Broke / Where I Got Stuck

My `FULL JOIN` solution wasn't wrong on the surface — it produced the correct output on the seeded data. But it was wrong in the general case, and that's a dangerous kind of wrong because it hides until production.

**The bug:** `FULL JOIN` returns all rows from *both* tables. That means it would also return enrollments that have no matching course — orphan records pointing at deleted or invalid course IDs. The seeded test data had none of those, so the extra rows never appeared. But in a real system they can exist, and the query would silently return garbage rows nobody asked for.

The requirement was *"every course, even with no enrollments"* — not *"everything from both tables."* That distinction is the difference between `LEFT JOIN` and `FULL JOIN`.

---

## 💡 The Eureka Moment

The question that unlocked it: **"Which table is my source of truth?"**

Courses were. Every course had to appear in the report, regardless of whether it had enrollments. Enrollments were supplementary data — attached to courses where they existed, ignored where they didn't.

That maps exactly to `LEFT JOIN`: keep everything from the left table (courses), attach right-table data (enrollments) where a match exists, and fill with `NULL` where it doesn't. `COUNT(e.id)` then counts non-NULL values only — so a course with no enrollments naturally gets a count of `0`.

---

## ✅ My Solution vs The Correct One

```sql
-- ✗ My solution — works on clean data, wrong in the general case
SELECT
  c.title AS course_title,
  COUNT(e.id) AS enrollment_count
FROM courses c
FULL JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC, course_title ASC;

-- ✓ Correct solution
SELECT
  c.title AS course_title,
  COUNT(e.id) AS enrollment_count
FROM courses c
LEFT JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC, course_title ASC;
```

One word. The rest of the query is identical. The difference is entirely in what each JOIN type includes.

---

## 📐 All Four SQL JOINs Explained

Using the same two tables from this problem as examples throughout.

**The tables:**

`courses`

| id | title | difficulty |
|----|-------|------------|
| 1 | SQL for Data Analysis | easy |
| 2 | Intro to Databases | easy |
| 3 | Advanced Indexing Strategies | medium |
| 4 | Normalization Deep Dive | medium |
| 5 | ACID Transactions Explained | hard |
| 6 | Distributed Systems and Sharding | hard |

`enrollments`

| id | course_id | student_name |
|----|-----------|--------------|
| 1 | 1 | Alice |
| 2 | 1 | Bob |
| 3 | 1 | Carol |
| 4 | 2 | Alice |
| 5 | 2 | Dave |
| 6 | 3 | Bob |
| 7 | 4 | Eve |

Courses 5 and 6 have no enrollments. That's the edge case everything hinges on.

---

### INNER JOIN — Only matching rows from both sides

```
courses        enrollments
  ┌───┐            ┌───┐
  │   │ ██████████ │   │
  │   │ ██ match ██│   │
  └───┘            └───┘
```

Returns only rows where a match exists in **both** tables. No match on either side means the row is excluded entirely.

```sql
SELECT c.title, COUNT(e.id) AS enrollment_count
FROM courses c
INNER JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC;
```

**Result:**

| course_title | enrollment_count |
|---|---|
| SQL for Data Analysis | 3 |
| Intro to Databases | 2 |
| Advanced Indexing Strategies | 1 |
| Normalization Deep Dive | 1 |

> ⚠️ ACID Transactions Explained and Distributed Systems and Sharding are **silently dropped** — they have no enrollments so they produce no match. For this problem, INNER JOIN gives the wrong answer.

**When to use it:** You only care about rows that have a counterpart in both tables. Orders with their customer details. Employees with their department records. If the match doesn't exist, the row is irrelevant to your query.

---

### LEFT JOIN — All rows from the left, matches from the right

```
courses        enrollments
  ┌───┐            ┌───┐
  │███│ ██████████ │   │
  │███│ ██ match ██│   │
  └───┘            └───┘
```

Returns **every row from the left table**, with right-table columns populated where a match exists and `NULL` where it doesn't. This is the correct JOIN for this problem.

```sql
SELECT c.title, COUNT(e.id) AS enrollment_count
FROM courses c
LEFT JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC, c.title ASC;
```

**Result:**

| course_title | enrollment_count |
|---|---|
| SQL for Data Analysis | 3 |
| Intro to Databases | 2 |
| Advanced Indexing Strategies | 1 |
| Normalization Deep Dive | 1 |
| ACID Transactions Explained | **0** |
| Distributed Systems and Sharding | **0** |

> ✅ Every course appears. Zero-enrollment courses show `0` because `COUNT(e.id)` counts non-NULL values only — and when there's no matching enrollment, `e.id` is `NULL`.

**When to use it:** The left table is your source of truth and must appear in full. Attach right-table data where available. The classic scenario: "show me all X, with Y data where it exists."

---

### RIGHT JOIN — All rows from the right, matches from the left

```
courses        enrollments
  ┌───┐            ┌───┐
  │   │ ██████████ │███│
  │   │ ██ match ██│███│
  └───┘            └───┘
```

Mirror image of LEFT JOIN. Returns **every row from the right table**, with left-table columns populated where a match exists and `NULL` where it doesn't.

```sql
SELECT c.title, COUNT(e.id) AS enrollment_count
FROM courses c
RIGHT JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC;
```

**Result:**

| course_title | enrollment_count |
|---|---|
| SQL for Data Analysis | 3 |
| Intro to Databases | 2 |
| Advanced Indexing Strategies | 1 |
| Normalization Deep Dive | 1 |

> ⚠️ Courses with no enrollments vanish again — because the right table (enrollments) is the source of truth here, not courses. If an orphan enrollment existed pointing at a deleted course, it would appear with `NULL` in the course columns.

**When to use it:** Rarely written directly. Any RIGHT JOIN can be rewritten as a LEFT JOIN by swapping the table order, which reads more naturally. Most SQL writers do exactly that.

---

### FULL JOIN — Everything from both sides

```
courses        enrollments
  ┌───┐            ┌───┐
  │███│ ██████████ │███│
  │███│ ██ match ██│███│
  └───┘            └───┘
```

Returns **all rows from both tables**. Where a match exists, both sides are populated. Where there's no match on either side, the missing columns are `NULL`. The most inclusive JOIN — and the most dangerous to reach for by default.

```sql
SELECT c.title, COUNT(e.id) AS enrollment_count
FROM courses c
FULL JOIN enrollments e ON c.id = e.course_id
GROUP BY c.id
ORDER BY enrollment_count DESC;
```

**Result (if an orphan enrollment exists):**

| course_title | enrollment_count | note |
|---|---|---|
| SQL for Data Analysis | 3 | matched |
| Intro to Databases | 2 | matched |
| Advanced Indexing Strategies | 1 | matched |
| Normalization Deep Dive | 1 | matched |
| ACID Transactions Explained | 0 | left-only ✓ |
| Distributed Systems and Sharding | 0 | left-only ✓ |
| NULL | 1 | ⚠️ orphan enrollment |

> ⚠️ The last row is the bug. An enrollment with no matching course sneaks in. The seeded data had no orphan enrollments — which is why my FULL JOIN passed all test cases. In production, with real messy data, this row would silently appear and corrupt the report.

**When to use it:** Data reconciliation — finding mismatches between two systems. "Show me everything in system A with no match in system B, and everything in system B with no match in system A." That's a FULL JOIN with a `WHERE` filter.

---

## 🗺️ Quick Reference — Which JOIN to Pick

| JOIN | Keeps left-only rows | Keeps right-only rows | Use when… |
|------|:-------------------:|:--------------------:|-----------|
| `INNER JOIN` | ✗ | ✗ | You only want matched rows from both tables |
| `LEFT JOIN` | ✓ | ✗ | Left table is source of truth; attach right data where available |
| `RIGHT JOIN` | ✗ | ✓ | Right table is source of truth (swap tables and use LEFT instead) |
| `FULL JOIN` | ✓ | ✓ | You need everything from both sides, including unmatched rows |

**The mental shortcut:** Ask — *"which table must appear in full?"* Put that table on the LEFT. Use `LEFT JOIN` if you need all of it regardless of matches. Use `INNER JOIN` if unmatched rows are irrelevant.

---

## ⚠️ The COUNT(*) vs COUNT(column) Trap

This problem uses `COUNT(e.id)`, not `COUNT(*)`. That distinction is load-bearing with LEFT JOINs:

```sql
-- COUNT(*) counts every row, including NULL-filled rows from the LEFT JOIN
-- A course with no enrollments produces one NULL row → COUNT(*) = 1
-- WRONG: unenrolled courses show 1, not 0

-- COUNT(e.id) only counts non-NULL values
-- A course with no enrollments has e.id = NULL → COUNT(e.id) = 0
-- CORRECT: unenrolled courses show 0
```

Whenever you LEFT JOIN and aggregate, count the joined table's column — not `*`.

---

## 📚 Lessons Learnt

- **Name the source of truth before choosing a JOIN.** The table that must appear in full goes on the left. Everything else is supplementary.
- **"Most inclusive" is not always most correct.** FULL JOIN feels safe because it keeps everything — but it brings in data you didn't ask for. More rows isn't always the right answer.
- **Test with edge-case data, not just happy-path data.** My FULL JOIN passed because the seed data was clean. One orphan enrollment row would have caught the bug immediately.
- **`COUNT(column)` and `COUNT(*)` behave differently on NULLs.** Always count the joined column when you want a true match count from a LEFT JOIN.
- **RIGHT JOIN is almost never written directly.** Swap the table order and write a LEFT JOIN instead — it reads more naturally.

---

## 🔗 Further Reading

- [PostgreSQL docs — JOIN types](https://www.postgresql.org/docs/current/queries-table-expressions.html#QUERIES-JOIN)
- [SQLite docs — SELECT and JOINs](https://www.sqlite.org/lang_select.html)
- Next to explore: `LEFT JOIN` with a `WHERE right.id IS NULL` to find rows in the left table with *no* match at all — the "anti-join" pattern.