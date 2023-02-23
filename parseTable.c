
#include "grammar.h"
#include "set.h"

Set** firstSets = NULL;
Set** firstSetsRules = NULL;
Set** followSets = NULL;
char** elements;
bool *computed;
listElement** LHSLoc;
listElement** RHSLoc;
int base;
int EPSILON;

bool findFirst(int tokenID) {
    // If the token is a terminal, just return
    if (tokenID < 0) { return false; }

    // If already computed return
    if (firstSets[tokenID] != NULL) {
        return (firstSets[tokenID]->contains[EPSILON]);
    }

    // Traversing LHSLoc
    firstSets[tokenID] = initSet(base);
    listElement* head = LHSLoc[tokenID];
    while (head != NULL) {
        grammarElement* RHS = pdtable->grammarrules[head->productionID]->RHSHead;
        if (!computed[head->productionID]) {
            if (firstSetsRules[head->productionID] == NULL) {
                firstSetsRules[head->productionID] = initSet(base);
            }
            // loop if epsilon
            bool isEpsilon = false;
            while (isEpsilon = findFirst(RHS->tokenID - base)) {
                unionSet(firstSetsRules[head->productionID], firstSets[RHS->tokenID - base]);
                RHS = RHS->next;
                if (RHS == NULL) { break; }
                firstSets[tokenID]->contains[EPSILON] = false;
            }

            // In case the token is a terminal
            if (RHS != NULL && RHS->tokenID - base < 0) { 
                firstSetsRules[head->productionID]->contains[RHS->tokenID] = true;
            } else if (RHS != NULL) { // Union in the case no epsilon in FIRST(RHS)
                unionSet(firstSetsRules[head->productionID], firstSets[RHS->tokenID - base]);
            }

            computed[head->productionID] = true;
            bool flag = unionSet(firstSets[tokenID], firstSetsRules[head->productionID]);
            if (flag) {
                for (int i = 33; i < 38; ++i) {
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
                printf("LL(1) violated\n");
                printf("Rule Number %d with token %s\n", head->productionID, elements[tokenID + base]);
                exit(1);
            }
        } else {
            bool flag = unionSet(firstSets[tokenID], firstSetsRules[head->productionID]);
            if (flag) { 
                printf("LL(1) violated\n");
                printf("Rule Number %d with token %s\n", head->productionID, elements[tokenID + base]);
                exit(1);
            }
        }
        head = head->next;
    }

    return (firstSets[tokenID]->contains[EPSILON]);
}

void computeFirstSet(int nonTerminalLen, int terminalLen) {
    int ruleCount = pdtable->ruleCount;
    base = terminalLen + 2;
    EPSILON = terminalLen;
    elements = getElements(grammarTrie);
    printf("%d\t%d\t%d\n", ruleCount, nonTerminalLen, terminalLen);

    // Array of linked lists to store the id of production rules of that non-terminal
    LHSLoc = malloc(nonTerminalLen * sizeof(listElement*));
    memset(LHSLoc, 0, nonTerminalLen * sizeof(listElement*));   

    for (int i = 0; i < ruleCount; ++i) {
        int nt = pdtable->grammarrules[i]->LHS->tokenID;
        nt -= (base); // epsilon and $ after terminals
        listElement* newNode = malloc(sizeof(listElement));
        newNode->productionID = i;
        newNode->next = LHSLoc[nt];
        LHSLoc[nt] = newNode;
    }

    /* // Printing to check 
    for (int i = 0; i < nonTerminalLen; ++i) {
        listElement* head = LHSLoc[i];
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
    firstSets = malloc(nonTerminalLen * sizeof(Set*));
    memset(firstSets, 0, nonTerminalLen * sizeof(Set*));
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
        if (firstSets[i] == NULL) {
            printf("first set not formed for %d\n", i);
            continue;
        }
        for (int j = 0; j < base; ++j) {
            if (firstSets[i]->contains[j]) {
                printf("%d %s, ", j, elements[j]);
            }
        }
        printf("\n");
    }

    return;
}

bool findFollow(int tokenID) {
    // If the token is a terminal, return false
    if (tokenID < 0) {
        return false;
    }

    // Checking if the Follow Set for the given tokenID has already been computed or not
    if (followSets[tokenID] != NULL) {
        return followSets[tokenID]->contains[EPSILON];
    }

    followSets[tokenID] = initSet(base);
    listElement* head = RHSLoc[tokenID];
    while (head != NULL) {
        ProductionRule *rule = pdtable->grammarrules[head->productionID];
        grammarElement* RHS = rule->RHSHead;
        while (RHS->tokenID != (tokenID + base)) {
            RHS = RHS->next;
        }
        RHS = RHS->next;
        while (RHS != NULL) {
            if (RHS->isTerminal) {
                followSets[tokenID]->contains[RHS->tokenID] = true;
                break;
            } else {
                unionSet(followSets[tokenID], firstSets[RHS->tokenID - base]);
                if (followSets[tokenID]->contains[EPSILON]) {
                    RHS = RHS->next;
                    followSets[tokenID]->contains[EPSILON] = false;
                } else {
                    break;
                }
            }
        }

        if (RHS == NULL) {
            int LHStokenID = rule->LHS->tokenID - base;
            if (LHStokenID == tokenID) {
                head = head->next;
                continue;
            }
            findFollow(LHStokenID);
            unionSet(followSets[tokenID], followSets[LHStokenID]);
        }

        head = head->next;
    }


    return false;
}

void computeFollowSet(int nonTerminalLen, int terminalLen) {
    int ruleCount = pdtable->ruleCount;

    // Allocating space for Follow Sets and setting to NULL
    followSets = malloc(nonTerminalLen * sizeof(Set*));
    memset(followSets, 0, nonTerminalLen * sizeof(Set*));

    // Creating an array to store the location of the nonterminals in the RHS
    RHSLoc = malloc(nonTerminalLen * sizeof(listElement*));
    memset(RHSLoc, 0, nonTerminalLen * sizeof(listElement*));

    // Adding $ to <program> 
    followSets[0] = initSet(base);
    followSets[0]->contains[terminalLen + 1] = true;
    
    for (int i = 0; i < ruleCount; ++i) {
        grammarElement *RHS = pdtable->grammarrules[i]->RHSHead;

        while (RHS != NULL) {
            if (RHS->tokenID - base >= 0) {
                listElement* newNode = malloc(sizeof(listElement));
                newNode->productionID = i;
                newNode->next = RHSLoc[RHS->tokenID - base];
                RHSLoc[RHS->tokenID - base] = newNode;
            }
            RHS = RHS->next;
        }
    }

    /* // Printing to check
    for (int i = 0; i < nonTerminalLen; ++i) {
        listElement* head = RHSLoc[i];
        printf("%s: ", elements[i + base]);
        while (head != NULL) {
            printf("%d ", head->productionID);
            head = head->next;
        }
        printf("\n");
    }
    exit(0); */

    // Iterating over First Sets and compute Follow for the ones that are nullable
    // base is terminalLen + 2, which is the length of firstSets
    // epsilon at terminalLen
    for (int i = 0; i < nonTerminalLen; ++i) {
        if (firstSets[i]->contains[EPSILON]) {
            findFollow(i);
        }   
    }

    // Printing FOLLOW Sets
    for (int i = 0; i < nonTerminalLen; ++i) {
        if (firstSets[i]->contains[EPSILON]) {
            printf("FOLLOW(%s): ", elements[i + base]);
            for (int j = 0; j < base; ++j) {
                if (followSets[i]->contains[j]) {
                    printf("%s, ", elements[j]);
                }
            }
            printf("\n");
        } 
    }

    return;
}

ParseTable* computeParseTable() {
    // TODO 
    ParseTable* res = malloc(sizeof(ParseTable));
    return res;
}