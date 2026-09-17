def format_item(item_name: str, quantity: int) -> str:
    return item_name + ": " + str(quantity)


def add_stock(current_count: int, delivery_count: int) -> int:
    return current_count + delivery_count


def total_inventory(counts: list[int]) -> int:
    total = 0
    for count in counts:
        total += count
    return total
