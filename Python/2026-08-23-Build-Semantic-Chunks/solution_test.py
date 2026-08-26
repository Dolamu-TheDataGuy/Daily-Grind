from solution import *

run_cases = [
    (
        ["Cats", "purr", "Dogs", "bark"],
        [0.1, 0.9, 0.2],
        0.8,
        1,
        [[0.2, 0.8], [0.7, 0.3]],
        [
            {"tokens": ["Cats", "purr"], "embedding": [0.2, 0.8]},
            {"tokens": ["purr", "Dogs", "bark"], "embedding": [0.7, 0.3]},
        ],
    ),
    (
        ["one", "continuous", "topic"],
        [0.2, 0.3],
        0.7,
        2,
        [[1.0, 0.0]],
        [
            {
                "tokens": ["one", "continuous", "topic"],
                "embedding": [1.0, 0.0],
            }
        ],
    ),
]

submit_cases = run_cases + [
    ([], [], 0.5, 2, [], []),
    (
        ["a", "b", "c", "d"],
        [0.9, 0.9, 0.9],
        0.9,
        3,
        [[1], [2], [3], [4]],
        [
            {"tokens": ["a"], "embedding": [1]},
            {"tokens": ["a", "b"], "embedding": [2]},
            {"tokens": ["a", "b", "c"], "embedding": [3]},
            {"tokens": ["a", "b", "c", "d"], "embedding": [4]},
        ],
    ),
    (
        ["solar", "panels", "make", "power", "batteries", "store", "it"],
        [0.1, 0.2, 0.85, 0.1, 0.95, 0.2],
        0.8,
        2,
        [[0.9, 0.1], [0.5, 0.5], [0.1, 0.9]],
        [
            {"tokens": ["solar", "panels", "make"], "embedding": [0.9, 0.1]},
            {
                "tokens": ["panels", "make", "power", "batteries"],
                "embedding": [0.5, 0.5],
            },
            {
                "tokens": ["power", "batteries", "store", "it"],
                "embedding": [0.1, 0.9],
            },
        ],
    ),
]


def test(tokens, scores, threshold, overlap, embeddings, expected):
    print("---------------------------------")
    print(f"Tokens:          {tokens}")
    print(f"Boundary scores: {scores}")
    print(f"Threshold:       {threshold}")
    print(f"Overlap:         {overlap}")
    print(f"Embeddings:      {embeddings}")
    print("")

    chunks = split_semantic_chunks(tokens, scores, threshold, overlap)
    result = attach_embeddings(chunks, embeddings)

    print(f"Expected: {expected}")
    print(f"Actual:   {result}")
    if result == expected:
        print("Pass")
        return True
    print("Fail")
    return False


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)

    for test_case in test_cases:
        if test(*test_case):
            passed += 1
        else:
            failed += 1

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
