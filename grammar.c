#include "grammar.h"

Trie *grammarTrie;

#define TOTAL_RULES 200     //total number of rules in grammar
#define NULL_RULES 50       //expected number of nullable rules

ProductionTable *pdtable;           //stores all rules
ProductionTable *nullpdtable;       //stores only NULLABLE rules

int parseTable[TOTAL_RULES][TOTAL_RULES];

ProductionTable *initializeProductionTable(ProductionTable *pdtable, int maxRules) {
    pdtable = malloc(sizeof(ProductionTable));
    pdtable->maxRules = maxRules;
    pdtable->ruleCount = 0;
    pdtable->grammarrules = malloc(maxRules * (sizeof(ProductionRule)));
    return pdtable;
}

char** findNullableTerminals(ProductionTable *nullpdtable) {
    int sz = nullpdtable->ruleCount;
    char **str = (char**)malloc(sz*sizeof(char*));
    for (int i = 0; i < sz; i++) {
        str[i] = (char *)malloc(100 * sizeof(char));
        strcpy(str[i], nullpdtable->grammarrules[i]->LHS->lexeme);
    }
    return str;
}

void insertRuleInProductionTable(ProductionTable *pdtable, ProductionRule *p) {
    if(pdtable->ruleCount == pdtable->maxRules - 1) {
        printf("Sorry, table is full\n");
        return;
    }
    pdtable->grammarrules[pdtable->ruleCount] = p;
    pdtable->ruleCount++;
}

Set** firstSets = NULL;
Set** firstSetsRules = NULL;
Set** followSets = NULL;
char** elements;
bool *computed;
listElement** LHSLoc;
listElement** RHSLoc;
int base;
int EPSILON;
int DOLLAR;

Set* initSet(int sz) {
    Set* s = malloc(sizeof(Set));
    s->sz = sz;
    s->contains = malloc(sz * sizeof(bool));
    memset(s->contains, false, sz * sizeof(bool));

    return s;
}

bool unionSet(Set* a, Set* b) {
    int sz = a->sz;
    // Flag for checking LL(1)
    bool flag = false;
    for (int i = 0; i < sz; ++i) {
        if (a->contains[i] && b->contains[i]) {
            flag = true;
        }
        a->contains[i] = a->contains[i] || b->contains[i];
    }

    return flag;
}

void intersectionSet(Set* a, Set* b) {
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        a->contains[i] = a->contains[i] && b->contains[i];
    }

    return;
}

void differenceSet(Set* a, Set* b) {
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        if (b->contains[i]) {
            a->contains[i] = false;
        }
    }

    return;
}


void printProductionTable(ProductionTable *pdtable) {
    printf("Printing Production Table\n");
    int sz = pdtable->ruleCount;
    for (int i = 1; i <= sz; i++) {
        printf("[%d]\t(%d)%s -> ", pdtable->grammarrules[i-1]->productionID, pdtable->grammarrules[i - 1]->LHS->tokenID, pdtable->grammarrules[i - 1]->LHS->lexeme);
        grammarElement *ptr = pdtable->grammarrules[i - 1]->RHSHead;
        while(ptr!=NULL) {
            printf("%s", ptr->lexeme);
            // if(ptr->isTerminal)
            //     printf("*"); // Print a star, if the element is a terminal
            printf(" ");
            ptr = ptr->next;
        }
        printf("\n");
        printf("FIRST SET = ");
        for (int j = 0; j < base; ++j) {
            if (pdtable->grammarrules[i-1]->firstSet->contains[j]) {
                printf("%s, ", elements[j]);
            }
        }
        printf("\n");
        if(pdtable->grammarrules[i-1]->firstSet->contains[EPSILON]) {
            printf("FOLLOW SET = ");
            // int currNT = pdtable->grammarrules[i-1]->LHS->tokenID;
            // printf("FOLLOW(LHS(%d) = %s): ", currNT, pdtable->grammarrules[i]->LHS->lexeme);
            for (int j = 0; j < base; ++j) {
                if (pdtable->grammarrules[i-1]->followSet->contains[j]) {
                    printf("%s, ", elements[j]);
                }
            }
            printf("\n");
        }
    }
    return;
}



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
            while ((isEpsilon = findFirst(RHS->tokenID - base))) {
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
    DOLLAR = terminalLen + 1;
    elements = getElements(grammarTrie);

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
        pdtable->grammarrules[i]->firstSet = firstSetsRules[i]; //attaching first set
        /* printf("FIRST(RHS(%d)): ", i);
        if (firstSetsRules[i] == NULL) {
            printf("first set not formed for %d\n", i);
            continue;   
        }
        for (int j = 0; j < base; ++j) {
            if (firstSetsRules[i]->contains[j]) {
                printf("%s, ", elements[j]);
            }
        }
        printf("\n"); */
    }

    /* // Non-Terminals
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
    } */

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

        bool computing = false;
        while (RHS != NULL) {
            while (RHS != NULL && RHS->tokenID != (tokenID + base)) {
                RHS = RHS->next;
            }

            if (RHS == NULL) { break; }
            computing = true;
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

            if (RHS != NULL) {
                computing = false;
            }
        }
        if (computing) {
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

    /* // Printing FOLLOW Sets
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
    } */

    return;
}

void attachFollowToRule() {
    for (int i = 0; i < pdtable->ruleCount; ++i) {
        if(pdtable->grammarrules[i]->firstSet->contains[EPSILON]) {
            int currNT = pdtable->grammarrules[i]->LHS->tokenID;
            /* printf("FOLLOW(LHS(%d) = %s): ", currNT, pdtable->grammarrules[i]->LHS->lexeme);
            for (int j = 0; j < base; ++j) {
                if (followSets[currNT - base]->contains[j]) {
                    printf("%s, ", elements[j]);
                }
            }
            printf("\n"); */
            pdtable->grammarrules[i]->followSet = followSets[currNT - base];
        }
    }
}

void computeParseTable() {

    // Initializing to -1
    memset(parseTable, -1, sizeof(parseTable));

    // Iterate through the production table
    for (int i = 0; i < pdtable->ruleCount; ++i) {
        int LHS = pdtable->grammarrules[i]->LHS->tokenID - base;
        // For each terminal 'a' in FIRST(RHS), add A -> RHS to parseTable[A][a]
        for (int j = 0; j < EPSILON; ++j) {
            if (pdtable->grammarrules[i]->firstSet->contains[j]) {
                parseTable[LHS][j] = i;
            }
        }
        if (pdtable->grammarrules[i]->firstSet->contains[EPSILON]) {
            // If EPSILON is in FIRST(RHS), add A -> RHS to parseTable[A][a]
            for (int j = 0; j < EPSILON; ++j) {
                if (pdtable->grammarrules[i]->followSet->contains[j]) {
                    parseTable[LHS][j] = i;
                }
            }
            // If EPSILON is in FIRST(RHS), and $ is in FOLLOW(A), add A -> RHS to parseTable[A][$]
            if (pdtable->grammarrules[i]->followSet->contains[DOLLAR]) {
                parseTable[LHS][DOLLAR] = i;
            }
        }
    }

    /* // Printing parse table to check
    for (int i = 0; i < grammarTrie->count - base; ++i) {
        for (int j = 0; j < base; ++j) {
            if (parseTable[i][j] == -1) {
                printf("%s,", "e");
            } else {
                printf("%d,", parseTable[i][j]);
            }.
        }
        printf("\n");
    } */

    return;
}

int main() {
    grammarTrie = setupTrie();
    populateGrammarTrie(grammarTrie);
    int terminalTrieLen = grammarTrie->count; // it only contains terminals right now
    pdtable = initializeProductionTable(pdtable, TOTAL_RULES);
    nullpdtable = initializeProductionTable(nullpdtable, NULL_RULES);
    char *grammarFile = "grammar.txt";
    FILE *f = fopen(grammarFile, "r");
    if(f==NULL)
        printf("File error\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0; // number of bytes read in a line
    int nonTerminalID = grammarTrie->count;
    // Adding epsilon and $ to allow easier indexing in first and follow sets;
    nonTerminalID = insertWord(grammarTrie, "e", nonTerminalID);
    nonTerminalID = insertWord(grammarTrie, "$", nonTerminalID);
    int productionRuleID = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        if(line[read-1]=='\n') line[read-1] = '\0';
        // printf("%s\n", line);
        ProductionRule *p = (ProductionRule*)malloc(sizeof(ProductionRule));
        p->isEpsilon = false;
        char temp[read];
        strcpy(temp, line);
        char *tok = strtok(temp, " ");

        //Left side of Production Rule
        p->LHS = (grammarElement*)malloc(sizeof(grammarElement));
        strcpy(p->LHS->lexeme, tok);
        p->LHS->next = NULL;
        int nonTerminalCheck = searchGrammar(grammarTrie, tok); //Check if non-terminal already exists in NT Trie
        if(nonTerminalCheck == -1) {
            nonTerminalID = insertWord(grammarTrie, tok, nonTerminalID); //It doesn't exist, so add it
            p->LHS->tokenID = nonTerminalID - 1; //since it returns value incremented by 1
        }
        else
            p->LHS->tokenID = nonTerminalCheck; //It already exists, so assign ID

        //Right side of Production Rule
        grammarElement *RHSHead = NULL;
        grammarElement *RHSCurrent = RHSHead;

        int count = 0; //to count number of elements in RHS
        
        while(tok) {
            count++;
            if(count>2) {
                if(strcmp(tok, "e")==0)
                    p->isEpsilon = true;
                grammarElement *newElement = (grammarElement *)malloc(sizeof(grammarElement)); //allocate memory for a new element
                newElement->next = NULL;
                newElement->prev = NULL;
                strcpy(newElement->lexeme, tok); //copy the token string to the new element
                int terminalCheck = searchGrammar(grammarTrie, tok); //check if it is a terminal
                if (terminalCheck!= -1 && terminalCheck < terminalTrieLen) {
                    newElement->isTerminal = true;
                    newElement->tokenID = terminalCheck; //set tokenID to its unique enum from terminalTrie
                } else {
                    newElement->isTerminal = false; // not found in terminal trie, this means it is a non-terminal
                    int nonTerminalCheck = searchGrammar(grammarTrie, tok); // check if it exists in non-terminal trie
                    if(nonTerminalCheck == -1) {
                        nonTerminalID = insertWord(grammarTrie, tok, nonTerminalID); //it doesn not exist in NT trie, so insert
                        newElement->tokenID = nonTerminalID - 1; //since it returns value incremented by 1
                    } else {
                        newElement->tokenID = nonTerminalCheck; // it already exists so assign value
                    }
                }
                if (RHSCurrent == NULL) { //First element of our doubly linked list
                    newElement->prev = NULL;
                    RHSCurrent = newElement;
                    RHSHead = RHSCurrent;
                    tok = strtok(NULL, " ");
                    continue;
                }
                RHSCurrent->next = newElement;
                newElement->prev = RHSCurrent;
                RHSCurrent = RHSCurrent->next;
            }
            tok = strtok(NULL, " ");
        }
        p->RHScount = count - 2;
        p->RHSHead = RHSHead;
        p->RHSTail = RHSCurrent;
        p->productionID = productionRuleID; // set unique ID to each rule
        insertRuleInProductionTable(pdtable, p);
        productionRuleID++;
        if (p->isEpsilon)
            insertRuleInProductionTable(nullpdtable, p);
        // printf("Done rule\n");
    }
    // printProductionTable(pdtable);
    // printProductionTable(nullpdtable);

    computeFirstSet(grammarTrie->count - terminalTrieLen - 2, terminalTrieLen);
    computeFollowSet(grammarTrie->count - terminalTrieLen - 2, terminalTrieLen);
    
    attachFollowToRule();

    // printProductionTable(pdtable);

    computeParseTable();

    // //Listing all non-terminals for which FOLLOW needs to be calculated
    // char **listNullables = findNullableTerminals(nullpdtable);
    // printf("{");
    // for (int i = 0; i < nullpdtable->ruleCount; i++) {        
    //     printf("%s,", listNullables[i]);        
    // }
    // printf("}\n");

    // Testing trie
    // printf("%d\t%d\n", terminalTrieLen, searchWord(grammarTrie, "e"));
    // printf("%s\t%d\n", "<moduleDef>", searchWord(grammarTrie, "<moduleDef>"));

    /* // Printing Trie
    int grammarTrieLen = grammarTrie->count;
    printf("grammarTrie Len: %d\n", grammarTrieLen);
    Tuple* elements = getElements(grammarTrie);
    for (int i = 0; i < grammarTrieLen; ++i) {
        printf("%d\t%s\n", elements[i].enumID, elements[i].token);
    } */

    freeTrie(grammarTrie);
    
    fclose(f);
    return 0;
}