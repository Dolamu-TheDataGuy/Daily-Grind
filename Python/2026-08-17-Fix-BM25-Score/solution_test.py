from main import *

run_cases = [
    (
        "red fish",
        "red fish red",
        ["red fish red", "blue fish", "red bird"],
        1.0314,
    ),
    (
        "python search",
        "python search engine python",
        [
            "python search engine python",
            "search engine basics",
            "python code examples",
        ],
        1.0621,
    ),
]

submit_cases = run_cases + [
    (
        "apple apple",
        "apple banana apple",
        ["apple banana apple", "banana fruit", "apple pie"],
        1.2299,
    ),
    (
        "orange",
        "apple banana",
        ["apple banana", "orange orange", "banana orange"],
        0.0,
    ),
    (
        "green turtle",
        "green turtle swims green turtle",
        [
            "green turtle swims green turtle",
            "green fish swims",
            "turtle shell green",
            "blue ocean turtle",
        ],
        0.8957,
    ),
]


def test(query, document, corpus, expected_output):
    print("---------------------------------")
    print(f"Query:    {query}")
    print(f"Document: {document}")
    print("Corpus:")
    for item in corpus:
        print(f"  - {item}")
    print("")
    result = bm25_score(query, document, corpus)
    print(f"Expected: {expected_output}")
    print(f"Actual:   {result}")
    if result == expected_output:
        return True
    return False


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)
    for test_case in test_cases:
        correct = test(*test_case)
        if correct:
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
