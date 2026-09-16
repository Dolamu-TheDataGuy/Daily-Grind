from typing import Optional


def summarize_inventory(
    inventory: dict[str, dict[str, Optional[int]]],
    warehouse: Optional[str],
) -> dict[str, int] :
    if warehouse is None:
        selected_inventory = inventory.copy()
    else:
        selected_inventory = {
            warehouse: inventory.get(warehouse, {})
        }

    summary = {}
    for items in selected_inventory.values():
        for item, quantity in items.items():
            if quantity is not None and quantity >= 0:
                summary[item] = summary.get(item, 0) + quantity

    return summary
