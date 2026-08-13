# Pull Request Merge Gate

**Date:** 2026-08-10
**Language:** Python
**Source:** boot.dev
**Concepts:** dictionaries, dictionary key overwrite behaviour, list comprehension, `zip()`, `enumerate()`, `match` statement, input validation

---

## 🎯 The Problem (one sentence)

Given a list of CI check results (ordered oldest to newest) and a list of required check names, determine whether a pull request can be merged — returning a boolean and a list of blocking reasons.

---

## 🧩 My First Approach

I split the problem into two functions as instructed. `get_latest_statuses` came first and was straightforward — iterate through the results, assign each check's status to a dictionary, and return it. I didn't overthink it.

`evaluate_merge` was where I went wrong early. I misread the requirements on the first pass and built my solution around the raw `check_results` list — using `zip()` to unpack it into separate `names` and `status` tuples, then building an `index` list via list comprehension inside the loop to find every position where a name appeared. I was essentially doing manually what `get_latest_statuses` already does for me automatically. I had six variables declared before any logic ran:

```python
def get_latest_statuses(check_results):
    values = {}
    for result in check_results:
        values[result[0]] = result[1]
    return values


def evaluate_merge(
    required_checks: list[str], check_results: list[tuple[str], ...]
) -> tuple[bool, list[str]]:
    keep_record = {}
    reasons = []
    can_merge = False
    names = ()
    status = ()
    latest_status = get_latest_statuses(check_results)

    if len(check_results) > 0:
        names, status = zip(*check_results)

    for item in required_checks:
        if (
            (item in keep_record)
            and (item in names)
            and status[names.index(item)] != "success"
        ):
            continue
        if item not in names:
            reasons.append(f"Missing required check: {item}")

        index = [num for num, value in enumerate(names) if value == item]

        if len(index) >= 1:
            for num in index:
                if status[num] == "success":
                    keep_record[item] = status[num]
                    can_merge = True
                    break

    for status in latest_status:
        if (
            (status not in keep_record)
            and (status in required_checks)
            and (latest_status[status] != "success")
        ):
            match latest_status[status]:
                case "failure":
                    reasons.append(f"Required check failed: {status}")
                case "pending":
                    reasons.append(f"Required check pending: {status}")
                case _:
                    reasons.append(
                        f"Required check has unknown status '{latest_status[status]}': {status}"
                    )

            can_merge = False

    return (can_merge, reasons)
```

And I still wasn't using `latest_status` properly — I was cross-referencing between the raw list and the processed dictionary simultaneously, which created unnecessary overhead on every loop iteration.

---

## 💥 What Broke / Where I Got Stuck

Two things went wrong, and together they cost me two days.

**First: I didn't read the requirements carefully enough.** The problem says *"results are ordered from oldest to newest, so a later duplicate replaces an earlier result"* — that's the entire specification for `get_latest_statuses`. But I didn't trust that the dictionary I built was sufficient. I kept going back to the raw `check_results` list in `evaluate_merge`, essentially solving the "latest status" problem twice. That's what produced all the unnecessary variables.

**Second: I overcomplicated the lookup.** Inside the `required_checks` loop I built an index list like this:

```python
index = [num for num, value in enumerate(names) if value == item]
```

This scans the entire `names` tuple on every iteration looking for every position a check name appears — an O(n) scan per required check, per iteration. Then I looped over those indices checking statuses. All of that work was already done and sitting in `latest_status`. I just needed `latest_status.get(item)`.

The code passed all test cases because the logic was *eventually* correct — but it arrived at the right answer through a longer, more fragile route than necessary.

---

## 💡 The Eureka Moment

Two separate insights unlocked this problem.

**Insight 1 — the dictionary key overwrite.** I didn't know that if you assign to the same key in a dictionary more than once, Python silently overwrites the earlier value with the later one. No error, no warning — the key just holds the most recent value:

```python
d = {}
d["tests"] = "failure"
d["tests"] = "success"
print(d["tests"])  # "success" — the earlier value is gone
```

This is exactly why `get_latest_statuses` works in three lines. You don't need to check whether the key already exists, you don't need to compare timestamps — you just iterate from oldest to newest and let the dictionary do the overwriting for you. The *order of the input list* is doing the deduplication work.

**Insight 2 — trust the helper function.** Once `get_latest_statuses` gives you a clean `{name: latest_status}` dictionary, `evaluate_merge` should use *only* that. The raw `check_results` list is irrelevant at that point. Every required check is either in the dictionary (with its latest status) or it isn't (missing). That's the whole decision tree.

---

## ✅ How the Final Solution Works

```python
def get_latest_statuses(check_results):
    values = {}
    for result in check_results:
        values[result[0]] = result[1]
    return values
```

Iterate oldest to newest. Each assignment overwrites the previous value for that key. By the time the loop ends, every key holds its latest status — no conditionals needed.

```python
def evaluate_merge(
    required_checks: list[str],
    check_results: list[tuple[str, ...]],
) -> tuple[bool, list[str]]:
    latest_status = get_latest_statuses(check_results)
    seen = set()
    reasons = []

    for check in required_checks:
        if check in seen:          # requirement: evaluate repeated names only once
            continue
        seen.add(check)

        if check not in latest_status:
            reasons.append(f"Missing required check: {check}")
            continue

        status = latest_status[check]
        match status:
            case "success":
                pass
            case "failure":
                reasons.append(f"Required check failed: {check}")
            case "pending":
                reasons.append(f"Required check pending: {check}")
            case _:
                reasons.append(f"Required check has unknown status '{status}': {check}")

    can_merge = len(reasons) == 0
    return (can_merge, reasons)
```

Step by step:

1. **`get_latest_statuses` first** — produce one clean dictionary. Everything after this point reads from that dictionary only, never from the raw list again.
2. **`seen` set for deduplication** — the requirement says "if a required check name is repeated, evaluate it only once." A set lookup is O(1) and is the right tool here. My original solution used a dictionary (`keep_record`) for this, which works but carries more weight than a set needs to.
3. **`latest_status.get(check)` is O(1)** — a single dictionary lookup per required check. No scanning, no index lists, no `enumerate()`.
4. **`match` for the status branches** — Python 3.10+ structural pattern matching is the cleanest way to handle the four exact string cases the problem specifies. My original solution had `match` too, which was a good instinct.
5. **`can_merge` derived at the end** — rather than maintaining a `can_merge` boolean and flipping it back and forth throughout the loop (which my original code did), derive it once from the final state: if `reasons` is empty, the PR can merge. One source of truth, no mutation bugs.

---

## 📚 Lessons Learnt

**Dictionary key overwrite is a feature, not a bug.** When you assign to an existing key, Python replaces the old value silently. Iterating a time-ordered list and assigning into a dictionary is a clean, idiomatic way to keep only the latest value — no timestamps, no conditionals, no extra variables.

**Read the requirements twice before writing a line.** Both days of confusion came from solving the problem I *imagined* rather than the problem that was *specified*. The requirements explicitly said results go oldest-to-newest and later duplicates replace earlier ones — that sentence was the implementation of `get_latest_statuses`. I missed it.

**Trust your helper functions.** If you've already processed the data into a clean structure, use that structure. Going back to raw input inside a second function is a sign that the first function's output isn't being trusted or fully used.

**Variable count is a code smell indicator.** Six variables declared before any logic runs is a signal worth pausing on. Ask: which of these are actually needed, and which exist because I'm solving the same sub-problem twice?

**`can_merge` should be derived, not maintained.** Flipping a boolean flag inside a loop — setting it True here, False there — creates hidden state that's hard to reason about. Deriving it once from `len(reasons) == 0` after the loop is done makes the logic obvious and removes a whole class of bugs.

---

## 🔗 Related Problems / Further Reading

- [Python docs — Dictionaries](https://docs.python.org/3/tutorial/datastructures.html#dictionaries)
- [Python docs — `match` statement (3.10+)](https://docs.python.org/3/reference/compound_stmts.html#the-match-statement)
- Next variation to try: extend `evaluate_merge` to support a *warning* status that doesn't block the merge but still gets reported — how does that change the `can_merge` derivation?