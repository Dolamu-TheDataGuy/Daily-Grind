def get_latest_statuses(check_results):
    latest_statuses = {}
    for check_name, status in check_results:
        latest_statuses[check_name] = status
    return latest_statuses


def evaluate_merge(required_checks, check_results):
    latest_statuses = get_latest_statuses(check_results)
    evaluated = set()
    reasons = []

    for check_name in required_checks:
        if check_name in evaluated:
            continue
        evaluated.add(check_name)

        if check_name not in latest_statuses:
            reasons.append(f"Missing required check: {check_name}")
            continue

        status = latest_statuses[check_name]
        if status == "failure":
            reasons.append(f"Required check failed: {check_name}")
        elif status == "pending":
            reasons.append(f"Required check pending: {check_name}")
        elif status != "success":
            reasons.append(
                f"Required check has unknown status '{status}': {check_name}"
            )

    return len(reasons) == 0, reasons
