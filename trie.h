#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef struct Trie {
    bool end;
    int tok;
    struct Trie* next[128];
} Trie;

Trie* setupTrie();
void populateTerminalTrie(Trie* trie);
Trie *createTrieNode();
int insertWord(Trie* tr, char *word, int tok);
int searchWord(Trie* tr, char* word);
