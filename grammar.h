#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

typedef struct grammarElement {
    char lexeme[100];
    struct grammarElement *next;
} grammarElement;

typedef struct ProductionRule {
    int productionID;
    int RHScount;
    bool isEpsilon;
    grammarElement *LHS; // pointer to LHS of a rule
    grammarElement *RHS; // pointer to first element in RHS
} ProductionRule;

typedef struct ProductionTable {
    int maxRules, ruleCount;
    ProductionRule **grammarrules; 
} ProductionTable;