from solution import build_inverted_index, get_tfidf_scores

run_cases = [
    (
        "apple",
        {
            "doc1": "red apple apple",
            "doc2": "green apple",
            "doc3": "blue berry",
        },
        {
            "red": ["doc1"],
            "apple": ["doc1", "doc2"],
            "green": ["doc2"],
            "blue": ["doc3"],
            "berry": ["doc3"],
        },
        {"doc1": 1.0, "doc2": 0.75, "doc3": 0.0},
    ),
    (
        "DOG",
        {
            "a": "dog cat",
            "b": "Dog dog fish",
            "c": "bird",
        },
        {
            "dog": ["a", "b"],
            "cat": ["a"],
            "fish": ["b"],
            "bird": ["c"],
        },
        {"a": 0.75, "b": 1.0, "c": 0.0},
    ),
]

submit_cases = run_cases + [
    (
        "apple",
        {},
        {},
        {},
    ),
    (
        "kiwi",
        {
            "doc1": "apple pear",
            "doc2": "pear banana",
            "doc3": "banana apple",
        },
        {
            "apple": ["doc1", "doc3"],
            "pear": ["doc1", "doc2"],
            "banana": ["doc2", "doc3"],
        },
        {"doc1": 0.0, "doc2": 0.0, "doc3": 0.0},
    ),
    (
        "code",
        {
            "guide": "python code python data",
            "notes": "data pipelines and code",
            "story": "music and rain",
        },
        {
            "python": ["guide"],
            "code": ["guide", "notes"],
            "data": ["guide", "notes"],
            "pipelines": ["notes"],
            "and": ["notes", "story"],
            "music": ["story"],
            "rain": ["story"],
        },
        {"guide": 0.38, "notes": 0.38, "story": 0.0},
    ),
]


def format_documents(documents):
    if len(documents) == 0:
        return "  (empty)"

    lines = []
    for name in documents:
        lines.append(f'  {name}: "{documents[name]}"')
    return "\n".join(lines)


def test(term, documents, expected_index, expected_scores):
    print("---------------------------------")
    print(f'Term: "{term}"')
    print("Documents:")
    print(format_documents(documents))
    print("")

    actual_index = build_inverted_index(documents)
    actual_scores = get_tfidf_scores(term, documents)

    print(f"Expected index:  {expected_index}")
    print(f"Actual index:    {actual_index}")
    print("")
    print(f"Expected scores: {expected_scores}")
    print(f"Actual scores:   {actual_scores}")

    if actual_index == expected_index and actual_scores == expected_scores:
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
