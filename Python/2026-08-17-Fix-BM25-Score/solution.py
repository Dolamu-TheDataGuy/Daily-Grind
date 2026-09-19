import math


def tokenize(text: str) -> list[str]:
    if text == "":
        return []
    return text.lower().split()


K1: float = 1.5
B: float = 0.75


def bm25_score(query: str, document: str, corpus: list[str]) -> float:
    if len(corpus) == 0:
        return 0.0

    query_terms: list[str] = tokenize(query)
    document_terms: list[str] = tokenize(document)
    corpus_terms: list[list[str]] = []
    for corpus_document in corpus:
        corpus_terms.append(tokenize(corpus_document))

    doc_len: int = len(document_terms)

    total_length: int = 0
    for terms in corpus_terms:
        total_length += len(terms)
    avg_doc_len: float = total_length / len(corpus_terms)
    if avg_doc_len == 0:
        return 0.0

    score: float = 0.0
    for term in query_terms:
        freq: int = 0
        for doc_term in document_terms:
            if doc_term == term:
                freq += 1
        if freq == 0:
            continue

        df: int = 0
        for terms in corpus_terms:
            if term in terms:
                df += 1

        idf: float = math.log(1 + (len(corpus_terms) - df + 0.5) / (df + 0.5))
        denominator: float = freq + K1 * (1 - B + B * (doc_len / avg_doc_len))
        score += idf * (freq * (K1 + 1)) / denominator

    return round(score, 4)
