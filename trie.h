#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

Trie* setupTrie();
void populateTerminalTrie(Trie* trie);
Trie *createTrieNode();
int insertWord(Trie* tr, char *word, int tok);
int searchWord(Trie* tr, char* word);