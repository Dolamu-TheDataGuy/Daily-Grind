def chunk_tokens(tokens: list[str], chunk_size: int, overlap: int) -> list[list[str]]:
    if chunk_size <= 0 or overlap < 0 or overlap >= chunk_size:
        raise ValueError("Cannot be processed.")

    if len(tokens) == 0:
        return []

    chunk = []

    n = 0

    while (n + overlap) < len(tokens):
        token = tokens[n : n + chunk_size]
        chunk.append(token)
        n += chunk_size - overlap

    return chunk
