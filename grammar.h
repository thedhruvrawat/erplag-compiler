#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "lexer.h"

extern ProductionTable* pdtable;
extern Trie* grammarTrie;


Set* initSet(int sz);
bool unionSet(Set* a, Set* b);

void computeFirstSet(int nonTerminalLen, int terminalLen);
bool findFirst(int tokenID);
void computeFollowSet(int nonTerminalLen, int terminalLen);
bool findFollow(int tokenID);
