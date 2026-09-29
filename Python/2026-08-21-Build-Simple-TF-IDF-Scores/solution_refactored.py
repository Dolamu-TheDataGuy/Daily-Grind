def build_inverted_index(documents: dict[str, str]) -> dict[str, list[str]]:
    if len(documents) == 0:
        return {}
    docmap: dict[str, list[str]] = {}
    for document, text in documents.items():
        seen_words: set[str] = set()
        for word in text.lower().split():
            if word in seen_words:
                continue
            seen_words.add(word)
            if word not in docmap:
                docmap[word] = [document]
            else:
                docmap[word].append(document)
    return docmap


def get_tfidf_scores(term: str, documents:dict[str, str]) -> dict[str, float]:
    if len(documents) == 0:
        return {}
    
    total_document: int = len(documents)
    term_lower: str = term.lower()
    number_of_document_term = 0

    for document in documents:
        if term_lower in documents[document].lower():
            number_of_document_term += 1
    
    if number_of_document_term == 0:
        idf = 0
    else:
        idf = total_document / number_of_document_term

    doc_scores = {}
    doc_list = documents[document].split()

    for document in documents:
        total_word_document = len(doc_list)
        term_count_document = 0
        for word in doc_list:
            if word.lower() == term_lower:
                term_count_document += 1
        doc_scores[document] = round((term_count_document / total_word_document) * idf, 2)

    return doc_scores