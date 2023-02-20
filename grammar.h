#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

typedef struct grammarElement {
    bool isTerminal;
    int tokenID;
    char lexeme[100];
    struct grammarElement *next, *prev;
} grammarElement;

typedef struct ProductionRule {
    int productionID;
    int RHScount;
    bool isEpsilon;
    grammarElement *LHS; // pointer to LHS of a rule
    grammarElement *RHSHead, *RHSTail; // pointer to first element in RHS
} ProductionRule;

typedef struct ProductionTable {
    int maxRules, ruleCount;
    ProductionRule **grammarrules; 
} ProductionTable;