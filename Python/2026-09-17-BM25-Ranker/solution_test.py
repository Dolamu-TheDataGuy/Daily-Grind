from solution import bm25_rank


def fmt_results(results, places=3):
    out = []
    for doc_id, score in results:
        out.append((doc_id, round(score, places)))
    return out


def test_case(name, query, docs, expected_order, expected_rounded_scores):
    print("---------------------------------")
    print(name)
    print("Input query:")
    print(f"  {query}")
    print("Input docs:")
    i = 0
    for d in docs:
        print(f"  {i}: {d}")
        i += 1
    print("")

    results = bm25_rank(query, docs)
    got_order = []
    for doc_id, _ in results:
        got_order.append(doc_id)

    got_scores = fmt_results(results)

    print(f"Expected order: {expected_order}")
    print(f"Actual order:   {got_order}")
    print("")
    print(f"Expected (rounded): {expected_rounded_scores}")
    print(f"Actual (rounded):   {got_scores}")

    if got_order != expected_order:
        return False
    if got_scores != expected_rounded_scores:
        return False
    return True


run_cases = [
    (
        "Run: 'brown dog' ranks doc 0 highest",
        "brown dog",
        [
            "the brown fox jumped over the brown dog",
            "the lazy dog slept in the sun",
            "brown dog and brown bear",
        ],
        [2, 0, 1],
        [(2, 0.881), (0, 0.753), (1, 0.131)],
    ),
    (
        "Run: single-term query 'sun'",
        "sun",
        [
            "the brown fox jumped over the brown dog",
            "the lazy dog slept in the sun",
            "brown dog and brown bear",
        ],
        [1, 0, 2],
        [(1, 0.959), (0, 0.0), (2, 0.0)],
    ),
]

submit_cases = run_cases + [
    (
        "Submit: repeated query terms count multiple times",
        "brown brown",
        [
            "the brown fox jumped over the brown dog",
            "the lazy dog slept in the sun",
            "brown dog and brown bear",
        ],
        [2, 0, 1],
        [(2, 1.46), (0, 1.262), (1, 0.0)],
    ),
    (
        "Submit: unknown term => all zeros, tie-break by doc_id",
        "unicorn",
        [
            "the brown fox jumped over the brown dog",
            "the lazy dog slept in the sun",
            "brown dog and brown bear",
        ],
        [0, 1, 2],
        [(0, 0.0), (1, 0.0), (2, 0.0)],
    ),
    (
        "Submit: 'lazy dog' ranks doc 1 highest",
        "lazy dog",
        [
            "the brown fox jumped over the brown dog",
            "the lazy dog slept in the sun",
            "brown dog and brown bear",
        ],
        [1, 2, 0],
        [(1, 1.09), (2, 0.15), (0, 0.123)],
    ),
]


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)

    for case in test_cases:
        ok = test_case(*case)
        if ok:
            passed += 1
            print("Pass")
        else:
            failed += 1
            print("Fail")

    if failed == 0:
        print("============= PASS ==============")
    else:
        print("============= FAIL ==============")

    if skipped > 0:
        print(f"{passed} passed, {failed} failed, {skipped} skipped")
    else:
        print(f"{passed} passed, {failed} failed")


test_cases = submit_cases
if "__RUN__" in globals():
    test_cases = run_cases

main()
