from solution import *


def make_pipeline(minimum, prefix):
    return Pipeline(
        [
            FilterProcessor(lambda record: meets_min_score(record, minimum)),
            TransformProcessor(lambda record: add_label(record, prefix)),
        ]
    )


run_cases = [
    (
        [
            {"id": "a1", "name": "Ada", "score": 90},
            {"id": "b2", "name": "Bo", "score": 40},
        ],
        50,
        "Student",
        [{"id": "a1", "name": "Ada", "score": 90, "label": "Student: Ada"}],
    ),
    (
        [
            {"id": "p1", "name": "Pia", "score": 70},
            {"id": "q2", "score": 70},
        ],
        70,
        "Qualified",
        [
            {"id": "p1", "name": "Pia", "score": 70, "label": "Qualified: Pia"},
            {"id": "q2", "score": 70, "label": "Qualified: Unknown"},
        ],
    ),
]

submit_cases = run_cases + [
    (
        [None, [], {"name": "No ID", "score": 100}, {"id": "", "score": 100}],
        0,
        "Valid",
        [],
    ),
    (
        [
            {"id": "x1", "name": "Missing"},
            {"id": "x2", "name": "Text", "score": "99"},
            {"id": "x3", "name": "Flag", "score": True},
        ],
        1,
        "Accepted",
        [],
    ),
    (
        [
            {"id": "r1", "name": "Rin", "score": 82, "team": "red"},
            {"id": "s2", "name": "Sol", "score": 95, "team": "blue"},
            {"id": "t3", "name": "Tao", "score": 81, "team": "gold"},
        ],
        82,
        "Finalist",
        [
            {
                "id": "r1",
                "name": "Rin",
                "score": 82,
                "team": "red",
                "label": "Finalist: Rin",
            },
            {
                "id": "s2",
                "name": "Sol",
                "score": 95,
                "team": "blue",
                "label": "Finalist: Sol",
            },
        ],
    ),
]


def test(records, minimum, prefix, expected):
    print("---------------------------------")
    print("Input records:")
    for record in records:
        print(f"  {record}")
    print(f"Minimum score: {minimum}")
    print(f"Label prefix:  {prefix}")

    original_list = records.copy()
    original_dicts = []
    for record in records:
        if isinstance(record, dict):
            original_dicts.append((record, record.copy()))

    result = make_pipeline(minimum, prefix).process(records)

    preserved = records == original_list
    for record, snapshot in original_dicts:
        if record != snapshot:
            preserved = False

    print(f"Expected: {expected}")
    print(f"Actual:   {result}")
    print(f"Input preserved: {preserved}")

    if result == expected and preserved:
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
