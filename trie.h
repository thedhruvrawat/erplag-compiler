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
void getElement(Trie* tr, char* tok, int pos, char** elements);
char** getElements(Trie* tr);
void freeTrie(Trie* tr);