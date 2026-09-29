def build_inverted_index(documents):
    if len(documents) == 0:
        return {}
    docmap = {}
    for document in documents:
        docstring = documents[document].lower()
        doclist = set(docstring.split())
        for word in doclist:
            if word not in docmap:
                docmap[word] = [document]
            else:
                docmap[word].append(document)
    return docmap


def get_tfidf_scores(term, documents):
    if len(documents) == 0:
        return {}
    
    total_document = len(documents)
    number_of_document_term = 0

    for document in documents:
        if term.lower() in documents[document].lower():
            number_of_document_term += 1
    try:
        idf = total_document / number_of_document_term
    except ZeroDivisionError:
        idf = 0

    doc_scores = {}

    for document in documents:
        total_word_document = len(documents[document].split())
        term_count_document = 0
        for word in documents[document].split():
            if word.lower() == term.lower():
                term_count_document += 1
        doc_scores[document] = round((term_count_document / total_word_document) * idf, 2)

    return doc_scores