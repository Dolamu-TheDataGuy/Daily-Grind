from solution import *

run_cases = [
    (["map", "key", "torch", "rope", "coin"], 1, 4),
    ([10, 20, 30, 40, 50, 60], 0, 5),
]

submit_cases = run_cases + [
    ([], 0, 3),
    ([1, 2, 3], 1, 20),
    (["red", "blue", "green", "gold", "black", "white", "silver"], 2, 6),
]


def test(items, start, end):
    expected_range = items[start:end]
    expected_reverse = items[::-1]
    expected_alternating = items[start::2]

    print("---------------------------------")
    print(f"Input list: {items}")
    print(f"Start: {start}")
    print(f"End:   {end}")
    print("")

    actual_range = extract_range(items, start, end)
    actual_reverse = reverse_items(items)
    actual_alternating = take_every_other(items, start)

    print("extract_range:")
    print(f"Expected: {expected_range}")
    print(f"Actual:   {actual_range}")
    print("")
    print("reverse_items:")
    print(f"Expected: {expected_reverse}")
    print(f"Actual:   {actual_reverse}")
    print("")
    print("take_every_other:")
    print(f"Expected: {expected_alternating}")
    print(f"Actual:   {actual_alternating}")

    return (
        actual_range == expected_range
        and actual_reverse == expected_reverse
        and actual_alternating == expected_alternating
    )


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)

    for test_case in test_cases:
        if test(*test_case):
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
