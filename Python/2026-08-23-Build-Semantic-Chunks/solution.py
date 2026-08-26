def split_semantic_chunks(tokens: list[str], boundary_scores: list[float], threshold: float, overlap: int) -> list[list[str]]:
    if not tokens:
        return []
    result = [0]
    for index, score in enumerate(boundary_scores):
        if score >= threshold:
            result.append(index+1)
            
    outcome = []
    index = 0

    while index <= len(result)-1:
        if index == 0 and index == len(result)-1:
            outcome.append(tokens[result[0]:])
            index += 1
            continue

            
        if index == 0:
            outcome.append(tokens[result[0]:result[index+1]])
            index += 1
        
        elif index == len(result)-1:
            outcome.append(tokens[max(0, result[index]-overlap):])
            index += 1
        else:
            outcome.append(tokens[max(0, result[index]-overlap) : result[index+1]])
            index += 1
            
        
    return outcome
    
            
        


def attach_embeddings(chunks: list[str], embeddings: list[float]) -> list[dict[str, object]]:
    if not chunks or not embeddings:
        return []
    final_result = []

    for index, chunk in enumerate(chunks):
        content = {}
        content["tokens"] = chunks[index]
        content["embedding"] = embeddings[index]
        final_result.append(content)

    return final_result