#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef struct Trie {
    bool end;
    TOK_TYPE tok;
    struct Trie* next[128];
} Trie;

Trie* setupTrie();
Trie* createTrieNode();
void insertWord(Trie* tr, char *word, TOK_TYPE tok);
TOK_TYPE searchWord(Trie* tr, char* word);
