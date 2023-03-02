/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
#ifndef TRIE
#define TRIE

#include "stackADT.h"

typedef struct Trie {
    int count;
    bool end;
    TOK_TYPE tok;
    struct Trie* next[128];
} Trie;

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

#endif
