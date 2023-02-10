#include "trie.h"

Trie* setupTrie() {
    Trie* trie = createTrieNode();
    insertWord(trie, "integer", INTEGER);
    insertWord(trie, "real", REAL);
    insertWord(trie, "boolean", BOOLEAN);
    insertWord(trie, "of", OF);
    insertWord(trie, "array", ARRAY);
    insertWord(trie, "start", START);
    insertWord(trie, "end", END);
    insertWord(trie, "declare", DECLARE);
    insertWord(trie, "module", MODULE);
    insertWord(trie, "driver", DRIVER);
    // "program" specified in the language specifications but "Program" in the test cases
    insertWord(trie, "Program", PROGRAM); 
    insertWord(trie, "get_value", GET_VALUE);
    insertWord(trie, "print", PRINT);
    insertWord(trie, "use", USE);
    insertWord(trie, "with", WITH);
    insertWord(trie, "parameters", PARAMETERS);
    insertWord(trie, "takes", TAKES);
    insertWord(trie, "input", INPUT);
    insertWord(trie, "returns", RETURNS);
    insertWord(trie, "for", FOR);
    insertWord(trie, "in", IN);
    insertWord(trie, "switch", SWITCH);
    insertWord(trie, "case", CASE);
    insertWord(trie, "break", BREAK);
    insertWord(trie, "default", DEFAULT);
    insertWord(trie, "while", WHILE);
    insertWord(trie, "AND", AND);
    insertWord(trie, "OR", OR);
    insertWord(trie, "true", TRUE);
    insertWord(trie, "false", FALSE);
    return trie;
}

Trie* createTrieNode() {
    Trie* res = malloc(sizeof(Trie));
    res->tok = ID;
    res->end = false;
    memset(res->next, 0, sizeof(res->next));

    return res;
}

void insertWord(Trie* tr, char *word, TOK_TYPE tok) {
    int len = strlen(word);
    for (int i = 0; i < len; ++i) {
        if (tr->next[word[i]] == NULL) {
            tr->next[word[i]] = createTrieNode();
        }
        tr = tr->next[word[i]];
    }

    tr->end = true;
    tr->tok = tok;
    return;
}

TOK_TYPE searchWord(Trie* tr, char* word) {
    int len = strlen(word);
    for (int i = 0; i < len; ++i) {
        if (tr->next[word[i]] == NULL) {
            return ID;
        }
        tr = tr->next[word[i]];
    }

    return tr->tok;
}