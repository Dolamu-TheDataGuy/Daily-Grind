from solution import *

run_cases = [
    ("/srv/app", "assets/css/site.css", True),
    ("/srv/app", "pages/../index.html", True),
]

submit_cases = run_cases + [
    ("/srv/app", "../../etc/passwd", False),
    ("/srv/app", "/srv/application/secrets.txt", False),
    ("/srv/app", "/srv/app/posts/2025/welcome.html", True),
]


def test(project_dir, requested_path, expected):
    print("---------------------------------")
    print(f"Project directory: {project_dir}")
    print(f"Requested path:    {requested_path}")
    print("")
    result = is_path_allowed(project_dir, requested_path)
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
