def add_label(record, prefix):
    result = record.copy()
    name = result.get("name", "Unknown")
    result["label"] = f"{prefix}: {name}"
    return result


def meets_min_score(record, minimum):
    score = record.get("score")
    if isinstance(score, bool) or not isinstance(score, (int, float)):
        return False
    return score >= minimum


class TransformProcessor:
    def __init__(self, transform):
        self.transform = transform

    def process(self, records):
        return [self.transform(record) for record in records]


class FilterProcessor:
    def __init__(self, predicate):
        self.predicate = predicate

    def process(self, records):
        return [record for record in records if self.predicate(record)]


class Pipeline:
    def __init__(self, processors):
        self.processors = processors

    def process(self, records):
        output = [
            record.copy()  # Avoid mutation of input record
            for record in records
            if isinstance(record, dict)
            and isinstance(record.get("id"), str)
            and record["id"] != ""
        ]

        for processor in self.processors:
            output = processor.process(output)

        return output
