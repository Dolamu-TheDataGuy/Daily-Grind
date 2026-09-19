def apply_to_all(values, func):
    map_list = []

    for value in values:
        map_list.append(func(value))
    return map_list


def keep_if(values, predicate):
    filter_list = []

    for value in values:
        if predicate(value):
            filter_list.append(value)
    return filter_list


def fold(values, combine, initial):
    sum = initial

    for value in values:
        sum = combine(sum, value)

    return sum
