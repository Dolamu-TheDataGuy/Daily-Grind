from solution import minimum_hops

run_cases = [
    (
        {
            "A": ["B"],
            "B": ["C"],
            "C": []
        },
        "A",
        "C",
        2,
    ),
    (
        {
            "A": ["B", "C"],
            "B": ["D"],
            "C": ["E"],
            "D": [],
            "E": []
        },
        "A",
        "E",
        2,
    ),
    (
        {
            "Home": ["Store", "Park"],
            "Store": ["School"],
            "Park": [],
            "School": []
        },
        "Home",
        "School",
        2,
    ),
]

submit_cases = run_cases + [
    (
        {
            "A": ["B"],
            "B": ["C"],
            "C": []
        },
        "B",
        "B",
        0,
    ),
    (
        {
            "A": ["B"],
            "B": [],
            "C": ["D"],
            "D": []
        },
        "A",
        "D",
        -1,
    ),
    (
        {
            "Gate": ["Hall", "Stairs"],
            "Hall": ["Kitchen", "Library"],
            "Stairs": ["Attic"],
            "Kitchen": ["Pantry"],
            "Library": [],
            "Attic": [],
            "Pantry": []
        },
        "Gate",
        "Pantry",
        3,
    ),
]


def format_graph(graph):
    lines = []
    for node in graph:
        lines.append("  " + str(node) + " -> " + str(graph[node]))
    return "\n".join(lines)


def test(graph, start, target, expected_output):
    print("---------------------------------")
    print("Input graph:")
    print(format_graph(graph))
    print(f"Start:  {start}")
    print(f"Target: {target}")
    print("")
    result = minimum_hops(graph, start, target)
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
