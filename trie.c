/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdlib.h>
#include <string.h>
#include "trie.h"

/**
 * @brief Creates a new trie
 *
 * @return Trie*
 */
Trie* setupTrie()
{
    Trie* trie = createTrieNode();
    return trie;
}

/**
 * @brief Populates the given trie with terminals for keyword matching for lexer
 *
 * @param trie
 */
void populateTerminalTrie(Trie* trie)
{
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
    insertWord(trie, "program", PROGRAM);
    insertWord(trie, "get_value", GET_VALUE);
    insertWord(trie, "print", PRINT);
    insertWord(trie, "printoffset", PRINTOFFSET);
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
}

/**
 * @brief Populates the given terminals with the token names of all the terminals for the parser
 *
 * @param trie
 */
void populateGrammarTrie(Trie* trie)
{
    insertWord(trie, "INTEGER", INTEGER);
    insertWord(trie, "REAL", REAL);
    insertWord(trie, "BOOLEAN", BOOLEAN);
    insertWord(trie, "OF", OF);
    insertWord(trie, "ARRAY", ARRAY);
    insertWord(trie, "START", START);
    insertWord(trie, "END", END);
    insertWord(trie, "DECLARE", DECLARE);
    insertWord(trie, "MODULE", MODULE);
    insertWord(trie, "DRIVER", DRIVER);
    insertWord(trie, "PROGRAM", PROGRAM);
    insertWord(trie, "GET_VALUE", GET_VALUE);
    insertWord(trie, "PRINT", PRINT);
    insertWord(trie, "PRINTOFFSET", PRINTOFFSET);
    insertWord(trie, "USE", USE);
    insertWord(trie, "WITH", WITH);
    insertWord(trie, "PARAMETERS", PARAMETERS);
    insertWord(trie, "TAKES", TAKES);
    insertWord(trie, "INPUT", INPUT);
    insertWord(trie, "RETURNS", RETURNS);
    insertWord(trie, "FOR", FOR);
    insertWord(trie, "IN", IN);
    insertWord(trie, "SWITCH", SWITCH);
    insertWord(trie, "CASE", CASE);
    insertWord(trie, "BREAK", BREAK);
    insertWord(trie, "DEFAULT", DEFAULT);
    insertWord(trie, "WHILE", WHILE);
    insertWord(trie, "ID", ID);
    insertWord(trie, "NUM", NUM);
    insertWord(trie, "RNUM", RNUM);
    insertWord(trie, "PLUS", PLUS);
    insertWord(trie, "MINUS", MINUS);
    insertWord(trie, "MUL", MUL);
    insertWord(trie, "DIV", DIV);
    insertWord(trie, "LT", LT);
    insertWord(trie, "LE", LE);
    insertWord(trie, "GE", GE);
    insertWord(trie, "GT", GT);
    insertWord(trie, "EQ", EQ);
    insertWord(trie, "NE", NE);
    insertWord(trie, "DEF", DEF);
    insertWord(trie, "ENDDEF", ENDDEF);
    insertWord(trie, "DRIVERDEF", DRIVERDEF);
    insertWord(trie, "DRIVERENDDEF", DRIVERENDDEF);
    insertWord(trie, "COLON", COLON);
    insertWord(trie, "RANGEOP", RANGEOP);
    insertWord(trie, "SEMICOL", SEMICOL);
    insertWord(trie, "COMMA", COMMA);
    insertWord(trie, "ASSIGNOP", ASSIGNOP);
    insertWord(trie, "SQBO", SQBO);
    insertWord(trie, "SQBC", SQBC);
    insertWord(trie, "BO", BO);
    insertWord(trie, "BC", BC);
    insertWord(trie, "COMMENTMARK", COMMENTMARK); // Not really needed
    insertWord(trie, "TRUE", TRUE);
    insertWord(trie, "FALSE", FALSE);
    insertWord(trie, "AND", AND);
    insertWord(trie, "OR", OR);
}

/**
 * @brief Create a new trie node and initializes it
 *
 * @return Trie*
 */
Trie* createTrieNode()
{
    Trie* res = malloc(sizeof(Trie));
    res->count = 0;
    res->tok = ID;
    res->end = false;
    memset(res->next, 0, sizeof(res->next));

    return res;
}

/**
 * @brief Inserts a given string into the trie and associates that trie with the given tok
 *
 * @param tr
 * @param word
 * @param tok
 * @return int
 */
int insertWord(Trie* tr, char* word, int tok)
{
    Trie* head = tr;
    int len = strlen(word);
    for (int i = 0; i < len; ++i) {
        if (tr->next[word[i]] == NULL) {
            tr->next[word[i]] = createTrieNode();
        }
        tr = tr->next[word[i]];
    }

    if (tr->end) {
        return tok;
    }
    head->count++;
    tr->end = true;
    tr->tok = tok;
    return tok + 1;
}

/**
 * @brief Search the trie for the given string and returns tok associated with it if present, else ID
 *
 * @param tr
 * @param word
 * @return int
 */
int searchWord(Trie* tr, char* word)
{
    int len = strlen(word);
    for (int i = 0; i < len; ++i) {
        if (tr->next[word[i]] == NULL) {
            return ID;
        }
        tr = tr->next[word[i]];
    }
    return tr->tok;
}

/**
 * @brief Search the trie for the given string and returns tok associated with it if present, else -1
 *
 * @param tr
 * @param word
 * @return int
 */
int searchGrammar(Trie* tr, char* word)
{
    int len = strlen(word);
    for (int i = 0; i < len; ++i) {
        if (tr->next[word[i]] == NULL) {
            return -1;
        }
        tr = tr->next[word[i]];
    }
    return tr->tok;
}

/**
 * @brief Recursive function invoked by the getElement() function to form the elements array
 *
 * @param tr
 * @param tok
 * @param pos
 * @param elements
 */
void getElement(Trie* tr, char* tok, int pos, char** elements)
{
    if (tr->end) {
        tok[pos] = 0;
        elements[tr->tok] = malloc(sizeof(char) * (strlen(tok) + 1));
        strcpy(elements[tr->tok], tok);
    }
    for (int i = 0; i < 128; ++i) {
        if (tr->next[i] == NULL) {
            continue;
        }
        tok[pos] = (char)i;
        getElement(tr->next[i], tok, pos + 1, elements);
    }
    return;
}

/**
 * @brief Constructs the elements array which stores all the elements of the trie at their corresponding
 * index given by the tok
 *
 * @param tr
 * @return char**
 */
char** getElements(Trie* tr)
{
    int sz = tr->count;
    char** elements = malloc(sz * sizeof(char*));
    char tok[100];
    getElement(tr, tok, 0, elements);

    return elements;
}

/**
 * @brief Frees the memory allocated for the trie
 *
 * @param tr
 */
void freeTrie(Trie* tr)
{
    if (tr == NULL) {
        return;
    }
    for (int i = 0; i < 128; ++i) {
        if (tr->next[i] == NULL) {
            continue;
        }
        freeTrie(tr->next[i]);
    }
    free(tr);
    return;
}