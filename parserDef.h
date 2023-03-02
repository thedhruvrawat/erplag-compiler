/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef PARSER_DEF
#define PARSER_DEF

#include "Set.h"
#include "lexerDef.h"

typedef struct Tuple {
    int enumID;
    char* token;
} Tuple;

typedef struct listElement {
    int productionID;
    struct listElement* next;
} listElement;

typedef struct grammarElement {
    bool isTerminal;
    int tokenID;
    char lexeme[100];
    struct grammarElement *next, *prev;
} grammarElement;

typedef struct ProductionRule {
    int productionID;
    int RHScount;
    grammarElement* LHS; // pointer to LHS of a rule
    grammarElement *RHSHead, *RHSTail; // pointer to first element in RHS
    Set *firstSet, *followSet;
} ProductionRule;

typedef struct ProductionTable {
    int maxRules, ruleCount;
    ProductionRule** grammarrules;
} ProductionTable;

typedef struct TreeNode {
    TOKEN* tok;
    int depth;
    int tokenID;
    int productionID;
    int tokenDerivedFrom;
    bool isLeaf;
    struct TreeNode* next;
    struct TreeNode* child;
} TreeNode;

typedef struct ParseTree {
    TreeNode* root;
    int sz;
    int treeDepth;
} ParseTree;

#endif