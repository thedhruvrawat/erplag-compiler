#ifndef PARSER_DEF
#define PARSER_DEF

#include "lexerDef.h"
#include "Set.h"

typedef struct Tuple {
    int enumID;
    char *token;
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
    grammarElement *LHS; // pointer to LHS of a rule
    grammarElement *RHSHead, *RHSTail; // pointer to first element in RHS
    Set *firstSet, *followSet;
} ProductionRule;

typedef struct ProductionTable {
    int maxRules, ruleCount;
    ProductionRule **grammarrules; 
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