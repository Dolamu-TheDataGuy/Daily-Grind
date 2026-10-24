import math


def tokenize(text):
    tokens = []
    current = ""
    for ch in text.lower():
        if ch.isalnum():
            current += ch
        else:
            if current != "":
                tokens.append(current)
                current = ""
    if current != "":
        tokens.append(current)
    return tokens


def build_inverted_index(docs_tokens):
    index = {}
    for doc_id in range(len(docs_tokens)):
        tokens = docs_tokens[doc_id]
        counts = {}
        for term in tokens:
            if term in counts:
                counts[term] += 1
            else:
                counts[term] = 1

        for key in counts:
            if key not in index:
                index[key] = {}
            index[key][doc_id] = counts[key]

    return index


def bm25_rank(query, docs, k1=1.5, b=0.75):
    query_token = tokenize(query)

    docs_tokens = []
    doc_length = []
    for doc in docs:
        tokens = tokenize(doc)
        docs_tokens.append(tokens)
        doc_length.append(len(tokens))

    N = len(docs_tokens)
    avgdl = sum(doc_length) / len(doc_length)
    index = build_inverted_index(docs_tokens)

    result = []
    for doc_id in range(N):
        score = 0
        for term in query_token:
            if term in index:
                tf = index[term].get(doc_id, 0)
                df = len(index[term])
                idf = math.log(1 + (N - df + 0.5) / (df + 0.5))
                dl = doc_length[doc_id]
                score += idf * (tf * (k1 + 1)) / (tf + k1 * (1 - b + b * dl / avgdl))

        result.append((doc_id, score))

    result.sort(key=lambda content: (-content[1], content[0]))

    return result
