
#include "grammar.h"
#include "set.h"

Set** firstSetsNonTerminals = NULL;
Set** firstSetsRules = NULL;
char** elements;
bool *computed;
listElement** ruleLoc;
int base;

bool findFirst(int tokenID) {
    // If the token is a terminal, just return
    if (tokenID < 0) { return false; }

    // If already computed return
    if (firstSetsNonTerminals[tokenID] != NULL) {
        return (firstSetsNonTerminals[tokenID]->contains[base - 2]);
    }

    // Traversing ruleLoc
    firstSetsNonTerminals[tokenID] = initSet(base);
    listElement* head = ruleLoc[tokenID];
    while (head != NULL) {
        grammarElement* RHS = pdtable->grammarrules[head->productionID]->RHSHead;
        if (!computed[head->productionID]) {
            if (firstSetsRules[head->productionID] == NULL) {
                firstSetsRules[head->productionID] = initSet(base);
            }
            // loop if epsilon
            bool isEpsilon = false;
            while (isEpsilon = findFirst(RHS->tokenID - base)) {
                unionSet(firstSetsRules[head->productionID], firstSetsNonTerminals[RHS->tokenID - base]);
                RHS = RHS->next;
                if (RHS == NULL) { break; }
                firstSetsNonTerminals[tokenID]->contains[base - 2] = false;
            }

            // In case the token is a terminal
            if (RHS != NULL && RHS->tokenID - base < 0) { 
                firstSetsNonTerminals[tokenID]->contains[RHS->tokenID] = true;
                firstSetsRules[head->productionID]->contains[RHS->tokenID] = true;
            } else if (RHS != NULL) { // Union in the case no epsilon in FIRST(RHS)
                unionSet(firstSetsRules[head->productionID], firstSetsNonTerminals[RHS->tokenID - base]);
            }

            computed[head->productionID] = true;
            unionSet(firstSetsNonTerminals[tokenID], firstSetsRules[head->productionID]);
        } else {
            unionSet(firstSetsNonTerminals[tokenID], firstSetsRules[head->productionID]);
        }
        head = head->next;
    }

    return (firstSetsNonTerminals[tokenID]->contains[base - 2]);
}

void computeFirstSet(int nonTerminalLen, int terminalLen) {
    int ruleCount = pdtable->ruleCount;
    base = terminalLen + 2;
    elements = getElements(grammarTrie);
    printf("%d\t%d\t%d\n", ruleCount, nonTerminalLen, terminalLen);

    // Array of linked lists to store the id of production rules of that non-terminal
    ruleLoc = malloc(nonTerminalLen * sizeof(listElement*));
    memset(ruleLoc, 0, nonTerminalLen * sizeof(listElement*));   

    for (int i = 0; i < ruleCount; ++i) {
        int nt = pdtable->grammarrules[i]->LHS->tokenID;
        nt -= (terminalLen + 2); // epsilon and $ after terminals
        listElement* newNode = malloc(sizeof(listElement));
        newNode->productionID = i;
        newNode->next = ruleLoc[nt];
        ruleLoc[nt] = newNode;
    }

    /* // Printing to check 
    for (int i = 0; i < nonTerminalLen; ++i) {
        listElement* head = ruleLoc[i];
        printf("%s: ", elements[i + base]);
        while (head != NULL) {
            printf("%d ", head->productionID);
            head = head->next;
        }
        printf("\n");
    }
    exit(0); */

    // To track whether FIRST of that rule has already been calculated or not
    computed = malloc(ruleCount * sizeof(bool));
    memset(computed, 0, ruleCount * sizeof(bool));

    firstSetsRules = malloc(ruleCount * sizeof(Set*));
    memset(firstSetsRules, 0, ruleCount * sizeof(Set*));
    firstSetsNonTerminals = malloc(nonTerminalLen * sizeof(Set*));
    memset(firstSetsNonTerminals, 0, nonTerminalLen * sizeof(Set*));
    bool isEpsilon = false; 

    for (int i = 0; i < nonTerminalLen; ++i) {
        findFirst(i);
    }

    // Printing the FIRST SETS
    // Rules
    for (int i = 0; i < ruleCount; ++i) {
        printf("FIRST(RHS(%d)): ", i);
        if (firstSetsRules[i] == NULL) {
            printf("first set not formed for %d\n", i);
            continue;   
        }
        for (int j = 0; j < base; ++j) {
            if (firstSetsRules[i]->contains[j]) {
                printf("%s, ", elements[j]);
            }
        }
        printf("\n");
    }

    // Non-Terminals
    for (int i = 0; i < nonTerminalLen; ++i) {
        printf("FIRST(%s): ", elements[i + base]);
        if (firstSetsNonTerminals[i] == NULL) {
            printf("first set not formed for %d\n", i);
            continue;
        }
        for (int j = 0; j < base; ++j) {
            if (firstSetsNonTerminals[i]->contains[j]) {
                printf("%d %s, ", j, elements[j]);
            }
        }
        printf("\n");
    }

    return;
}

ParseTable* computeParseTable() {
    // TODO 
    ParseTable* res = malloc(sizeof(ParseTable));
    return res;
}