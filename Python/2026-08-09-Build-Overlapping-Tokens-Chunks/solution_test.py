from solution import chunk_tokens

run_cases = [
    (["a", "b", "c", "d", "e"], 3, 1, [["a", "b", "c"], ["c", "d", "e"]]),
    (
        ["one", "two", "three", "four", "five"],
        4,
        1,
        [["one", "two", "three", "four"], ["four", "five"]],
    ),
]

submit_cases = run_cases + [
    ([], 3, 1, []),
    (["a", "b"], 0, 0, ValueError),
    (["a", "b", "c"], 3, [-1, 3, 4], ValueError),
    (
        ["t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"],
        4,
        2,
        [
            ["t0", "t1", "t2", "t3"],
            ["t2", "t3", "t4", "t5"],
            ["t4", "t5", "t6", "t7"],
        ],
    ),
]


def test(tokens, chunk_size, overlap, expected):
    print("---------------------------------")
    print(f"Tokens:     {tokens}")
    print(f"Chunk size: {chunk_size}")
    print(f"Overlap:    {overlap}")
    print("")

    overlaps = overlap if isinstance(overlap, list) else [overlap]

    if expected is ValueError:
        for current_overlap in overlaps:
            try:
                result = chunk_tokens(tokens, chunk_size, current_overlap)
                print(f"Overlap {current_overlap}: expected ValueError")
                print(f"Actual: {result}")
                return False
            except ValueError as error:
                print(f"Overlap {current_overlap}: ValueError: {error}")
        return True

    result = chunk_tokens(tokens, chunk_size, overlap)
    print(f"Expected: {expected}")
    print(f"Actual:   {result}")
    return result == expected


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)

    for test_case in test_cases:
        try:
            correct = test(*test_case)
        except Exception as error:
            print(f"Error: {error}")
            correct = False

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


if __name__ == "__main__":
    test_cases = run_cases + submit_cases
    main()
