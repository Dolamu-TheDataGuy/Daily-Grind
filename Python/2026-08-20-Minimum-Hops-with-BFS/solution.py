def minimum_hops(graph, start, target):
    if start == target:
        return 0

    queue = [(start, 0)]
    visited = {start,}

    while queue:
        current_node, current_distance = queue.pop(0)

        if current_node not in graph:
            continue

        for n in sorted(graph[current_node]):
            if n == target:
                return current_distance + 1
            if n not in visited:
                queue.append((n, current_distance+1))
                visited.add(n)
    return -1