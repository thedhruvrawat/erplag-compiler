#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

Trie* setupTrie();
void populateTerminalTrie(Trie* trie);
void populateGrammarTrie(Trie *trie);
Trie *createTrieNode();
int insertWord(Trie* tr, char *word, int tok);
int searchWord(Trie* tr, char* word);
int searchGrammar(Trie *tr, char *word);
int compareTuple(const void *a, const void *b);
void getElement(Trie* tr, char* tok, int pos, Tuple* elements);
Tuple* getElements(Trie* tr);
void freeTrie(Trie* tr);