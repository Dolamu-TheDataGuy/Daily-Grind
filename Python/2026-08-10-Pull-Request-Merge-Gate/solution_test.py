from solution import evaluate_merge, get_latest_statuses

run_cases = [
    (
        ["lint", "tests"],
        [("lint", "success"), ("tests", "success")],
        {"lint": "success", "tests": "success"},
        (True, []),
    ),
    (
        ["lint", "tests", "tests"],
        [
            ("lint", "failure"),
            ("tests", "pending"),
            ("lint", "success"),
            ("tests", "success"),
        ],
        {"lint": "success", "tests": "success"},
        (True, []),
    ),
]

submit_cases = run_cases + [
    (
        ["lint", "tests"],
        [],
        {},
        (
            False,
            ["Missing required check: lint", "Missing required check: tests"],
        ),
    ),
    (
        ["lint", "tests", "build", "security"],
        [
            ("lint", "success"),
            ("tests", "failure"),
            ("build", "pending"),
            ("security", "cancelled"),
            ("docs", "failure"),
        ],
        {
            "lint": "success",
            "tests": "failure",
            "build": "pending",
            "security": "cancelled",
            "docs": "failure",
        },
        (
            False,
            [
                "Required check failed: tests",
                "Required check pending: build",
                "Required check has unknown status 'cancelled': security",
            ],
        ),
    ),
    (
        ["lint", "unit-tests", "integration-tests", "security"],
        [
            ("lint", "failure"),
            ("unit-tests", "pending"),
            ("optional-docs", "failure"),
            ("integration-tests", "success"),
            ("security", "success"),
            ("lint", "success"),
            ("unit-tests", "success"),
        ],
        {
            "lint": "success",
            "unit-tests": "success",
            "optional-docs": "failure",
            "integration-tests": "success",
            "security": "success",
        },
        (True, []),
    ),
]


def test(required_checks, check_results, expected_statuses, expected_evaluation):
    print("---------------------------------")
    print(f"Required checks: {required_checks}")
    print("CI results (oldest to newest):")
    if len(check_results) == 0:
        print("  (none)")
    else:
        for check_name, status in check_results:
            print(f"  * {check_name}: {status}")
    print("")

    actual_statuses = get_latest_statuses(check_results)
    actual_evaluation = evaluate_merge(required_checks, check_results)

    print(f"Expected latest statuses: {expected_statuses}")
    print(f"Actual latest statuses:   {actual_statuses}")
    print(f"Expected evaluation:      {expected_evaluation}")
    print(f"Actual evaluation:        {actual_evaluation}")

    if (
        actual_statuses == expected_statuses
        and actual_evaluation == expected_evaluation
    ):
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
