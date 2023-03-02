/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
#ifndef PARSER
#define PARSER
#include "parserDef.h"
#include "trie.h"

extern ProductionTable* pdtable;
extern Trie* grammarTrie;


void computeFirstSet(int nonTerminalLen, int terminalLen);
bool findFirst(int tokenID);
void computeFollowSet(int nonTerminalLen, int terminalLen);
bool findFollow(int tokenID);

void parserMain(char *userSourceCode, char *parseTreeOutput);
void cleanParser();

#endif