from solution import apply_to_all, keep_if, fold


def half_price(price):
    return price // 2


def is_passing(score):
    return score >= 60


def add(a, b):
    return a + b


def concat_strings(a, b):
    return a + b


def is_high_damage(dmg):
    return dmg >= 15


def increase_by_three(x):
    return x + 3


run_cases = [
    (
        "apply_to_all - 50% sale on shop prices",
        "apply_to_all",
        [100, 50, 80],
        half_price,
        None,
        [50, 25, 40],
    ),
    (
        "keep_if - passing quest scores",
        "keep_if",
        [55, 80, 92, 40, 100],
        is_passing,
        None,
        [80, 92, 100],
    ),
]

submit_cases = run_cases + [
    (
        "fold - total gold from loot",
        "fold",
        [10, 25, 5],
        add,
        0,
        40,
    ),
    (
        "apply_to_all and keep_if - boosted damage values",
        "combo",
        [5, 10, 15, 20],
        increase_by_three,
        None,
        [18, 23],
    ),
]


def test(label, which_function, values, func, initial, expected_output):
    print("---------------------------------")
    print(f"Test: {label}")
    print("")
    print(f"Input values: {values}")
    print(f"Function type: {which_function}")

    if which_function == "apply_to_all":
        result = apply_to_all(values, func)
    elif which_function == "keep_if":
        result = keep_if(values, func)
    elif which_function == "fold":
        print(f"Initial accumulator: {initial}")
        result = fold(values, func, initial)
    elif which_function == "combo":
        boosted = apply_to_all(values, func)
        print(f"After apply_to_all (boosted): {boosted}")
        result = keep_if(boosted, is_high_damage)
    else:
        print("Unknown function type")
        return False

    print(f"Expected: {expected_output}")
    print(f"Actual:   {result}")
    if result == expected_output:
        print("Pass")
        return True
    print("Fail")
    return False


def main():
    passed = 0
    failed = 0
    skipped = len(submit_cases) - len(test_cases)
    for test_case in test_cases:
        correct = test(*test_case)
        if correct:
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
