def add_label(record, prefix):
    result = record.copy()
    name = result.get("name", "Unknown")
    result["label"] = f"{prefix}: {name}"
    return result


def meets_min_score(record, minimum):
    if not isinstance(record, dict):
        return False
    try:
        score = record["score"]
        if isinstance(score, bool):
            return False

        if isinstance(score, int | float):
            return score >= minimum
    except:
        return False


class TransformProcessor:
    def __init__(self, transform):
        self.transform = transform

    def process(self, records):
        output = []
        for record in records:
            output.append(self.transform(record))
        return output


class FilterProcessor:
    def __init__(self, predicate):
        self.predicate = predicate

    def process(self, records):
        output = []
        for record in records:
            if self.predicate(record):
                output.append(record)
        return output


class Pipeline:
    def __init__(self, processors):
        self.processors = processors

    def process(self, records):
        if len(self.processors) == 0:
            return records

        filtered_record = []
        for record in records:
            if record:
                if isinstance(record.get("id"), str) and record.get("id") != "":
                    filtered_record.append(record)

        for processor in self.processors:
            current_records = processor.process(filtered_record)
            filtered_record = current_records

        return current_records
