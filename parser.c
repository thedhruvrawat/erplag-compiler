/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "Set.h"
#include "colorCodes.h"
#include "stackADT.h"

Trie *grammarTrie;

#define TOTAL_RULES 200     //total number of rules in grammar
#define NULL_RULES 50       //expected number of nullable rules

ProductionTable *pdtable;           //stores all rules

int parseTable[TOTAL_RULES][TOTAL_RULES]; 

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
static bool synCorrPrint = true;
ProductionTable *initializeProductionTable(ProductionTable *pdtable, int maxRules) {
    pdtable = malloc(sizeof(ProductionTable));
    pdtable->maxRules = maxRules;
    pdtable->ruleCount = 0;
    pdtable->grammarrules = malloc(maxRules * (sizeof(ProductionRule*)));
    return pdtable;
}

void insertRuleInProductionTable(ProductionTable *pdtable, ProductionRule *p) {
    if(pdtable->ruleCount == pdtable->maxRules - 1) {
        printf(RED BOLD "Sorry, table is full\n" RESET);
        return;
    }
    pdtable->grammarrules[pdtable->ruleCount] = p;
    pdtable->ruleCount++;
}

void printParseError(int p_errno, stackNode * top ,TOKEN* tok){
    synCorrPrint = false;
    switch(p_errno){
        case 1:
            {
                // printf("Stream has ended but the stack is non-empty\n\n");
                printf(RED BOLD "[Parser] Line: %d Error in the input as expected token is %s \n" RESET, tok->linenum, top->GE->lexeme);
                break;
            }
        case 2:
        {
            // printf("Top of Stack is Terminal: %s, Token is not!\n\n", topStack->GE->lexeme);
            printf(RED BOLD "[Parser] Line: %d Error in the input as expected token is %s \n" RESET, tok->linenum, top->GE->lexeme);
            break;
        }
        case 3:
        {
            // printf("Top of Stack is Terminal: %s, Token is not!\n\n", topStack->GE->lexeme);
            printf(RED BOLD "[Parser] Stream has ended but the stack is non-empty\n" RESET);
            break;
        }
    }
}


void printProductionTable(ProductionTable *pdtable) {
    printf("Printing Production Table\n");
    int sz = pdtable->ruleCount;
    for (int i = 1; i <= sz; i++) {
        printf("[%d]\t(%d)%s -> ", pdtable->grammarrules[i-1]->productionID, pdtable->grammarrules[i - 1]->LHS->tokenID, pdtable->grammarrules[i - 1]->LHS->lexeme);
        grammarElement *ptr = pdtable->grammarrules[i - 1]->RHSHead;
        while(ptr!=NULL) {
            printf("(%d)%s", ptr->tokenID, ptr->lexeme);
            // if(ptr->isTerminal)
            //     printf("*"); // Print a star, if the element is a terminal
            printf(" ");
            ptr = ptr->next;
        }
        printf("\n");
        printf(CYAN BOLD "FIRST SET = ");
        for (int j = 0; j < base; ++j) {
            if (pdtable->grammarrules[i-1]->firstSet->contains[j]) {
                printf("%s, ", elements[j]);
            }
        }
        printf("\n" RESET);
        if(pdtable->grammarrules[i-1]->firstSet->contains[EPSILON]) {
            printf(YELLOW BOLD "FOLLOW SET = ");
            // int currNT = pdtable->grammarrules[i-1]->LHS->tokenID;
            // printf("FOLLOW(LHS(%d) = %s): ", currNT, pdtable->grammarrules[i]->LHS->lexeme);
            for (int j = 0; j < base; ++j) {
                if (pdtable->grammarrules[i-1]->followSet->contains[j]) {
                    printf("%s, ", elements[j]);
                }
            }
            printf("\n" RESET);
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
                firstSetsRules[head->productionID]->contains[EPSILON] = false;
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
                printf(RED BOLD "LL(1) violated\n" RESET);
                printf(RED BOLD "Rule Number %d with token %s\n" RESET, head->productionID, elements[tokenID + base]);
                exit(1);
            }
        } else {
            bool flag = unionSet(firstSets[tokenID], firstSetsRules[head->productionID]);
            if (flag) { 
                printf(RED BOLD "LL(1) violated\n" RESET);
                printf(RED BOLD "Rule Number %d with token %s\n" RESET, head->productionID, elements[tokenID + base]);
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
                // if (parseTable[LHS][j] != -1) { printf("ERROR1\n"); exit(1); }
                parseTable[LHS][j] = i;
            }
        }
        if (pdtable->grammarrules[i]->firstSet->contains[EPSILON]) {
            // If EPSILON is in FIRST(RHS), add A -> RHS to parseTable[A][a]
            for (int j = 0; j < EPSILON; ++j) {
                if (pdtable->grammarrules[i]->followSet->contains[j]) {
                // if (parseTable[LHS][j] != -1) { printf("ERROR2\n"); exit(1); }
                    parseTable[LHS][j] = i;
                }
            }
            // If EPSILON is in FIRST(RHS), and $ is in FOLLOW(A), add A -> RHS to parseTable[A][$]
            if (pdtable->grammarrules[i]->followSet->contains[DOLLAR]) {
                // if (parseTable[LHS][DOLLAR] != -1) { printf("ERROR3\n"); exit(1); }
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
            }
        }
        printf("\n");
    } */

    return;
}


// PARSE TREE SECTION

ParseTree* parseTree;
const int ROOT = 618;

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))

void initRootNode() {
    parseTree->sz++;
    parseTree->treeDepth++;
    TreeNode* root = malloc(sizeof(TreeNode));
    root->tok = NULL;
    root->depth = 1;
    root->tokenID = pdtable->grammarrules[0]->LHS->tokenID;
    root->productionID = -1;
    root->tokenDerivedFrom = -1;
    root->isLeaf = false;
    root->next = NULL;
    root->child = NULL;
    parseTree->root = root;
    return;
}

void initParseTree() {
    parseTree = malloc(sizeof(ParseTree));
    parseTree->sz = 0;
    parseTree->treeDepth = 0;
    initRootNode();
    return;
}

void insertRuleInParseTree(TreeNode* parent, int productionID, TOKEN* tok, stack *st) {
    // if (parent->tokenID != pdtable->grammarrules[productionID]->LHS->tokenID) {
    //     printf("ERROR: Invalid Rule Insertion\n");
    //     exit(1);
    // }

    TreeNode* head = NULL;
    TreeNode* currNode;

    grammarElement* g = pdtable->grammarrules[productionID]->RHSTail;

    while (g != NULL) {
        currNode = malloc(sizeof(TreeNode));
        parseTree->sz++;
        currNode->depth = parent->depth + 1;
        currNode->tok = NULL;
        currNode->tokenID = g->tokenID;
        currNode->productionID = productionID;
        currNode->tokenDerivedFrom = parent->tokenID;
        currNode->isLeaf = g->isTerminal;
        currNode->next = head;
        currNode->child = NULL;
        head = currNode;
        pushStackGE(st, g, currNode);
        g = g->prev;
    }

    parseTree->treeDepth = MAX(parseTree->treeDepth, parent->depth + 1);
    parent->child = head;
    return;
}

void printParseTreeRec(TreeNode* node, FILE* fp, bool firstChild) {
    if (node == NULL) { 
        return;
    }

    printParseTreeRec(node->child, fp, true);

    if (node->isLeaf && node->tok != NULL) {
        fprintf(fp, "%-25s%-10d%-15s", node->tok->lexeme, node->tok->linenum, elements[node->tok->tok]);
    } else if (node->isLeaf) {
        fprintf(fp, "%-25s%-10s%-15s", "(null)", "(null)", "(null)");
    } else {
        fprintf(fp, "%-25s%-10s%-15s", "--------------------", "---", "----------");
    }

    if (node->isLeaf && node->tok == NULL) {
        fprintf(fp, "%-20s", "(null)");
    } else if (node->isLeaf && node->tok->tok == NUM) {
        fprintf(fp, "%-20d", node->tok->num);
    } else if (node->isLeaf && node->tok->tok == RNUM) {
        fprintf(fp, "%-20lf", node->tok->rnum);
    } else {
        fprintf(fp, "%-20s", "---------------");
    }

    if (node->tokenDerivedFrom >= 0) {
        fprintf(fp, "%-40s", elements[node->tokenDerivedFrom]);
    } else {
        fprintf(fp, "%-40s", "ROOT");
    }
    fprintf(fp, "%-5s", (node->isLeaf ? "Yes" : "No"));
    
    if (!node->isLeaf) {
        fprintf(fp, "%-20s", elements[node->tokenID]);
    }

    fprintf(fp, "\n");

    if (node->child != NULL) {
        printParseTreeRec(node->child->next, fp, false);
    }

    if (!firstChild) {
        printParseTreeRec(node->next, fp, false);
    }

    return;
}

void printParseTree(ParseTree* parseTree, char* outFile) {
    FILE* fp = fopen(outFile, "w");

    if (fp == NULL) {
        printf(RED BOLD "Unable to open the file to write parseTree.\n" RESET);
        exit(1);
    }

    printParseTreeRec(parseTree->root, fp, false);
    fclose(fp);
    return;
}
//############################################

// Setting Up STACK for the Parsing of given Input File
// Pushes Dollar, Followed by <program> into the Stack
void initParseStack(stack * st){
    grammarElement *dollar = (grammarElement*) malloc(sizeof(grammarElement));
    dollar->isTerminal = true;
    dollar->tokenID = DOLLAR;
    strcpy(dollar->lexeme, "EOF");
    dollar->next = NULL; dollar->prev = NULL;

    pushStackGE(st, dollar, NULL);

    // push <program> into stack
    grammarElement* S = pdtable->grammarrules[0]->LHS;

    pushStackGE(st, S, parseTree->root);
}

// void pushRule(stack *S, ProductionRule *rule)
// {
//     // Insert the Rule RHS from right to left
//     grammarElement *curr = rule->RHSTail;
//     while (curr != NULL)
//     {
//         pushStackGE(S, curr, );
//         curr = curr->prev;
//     }
//     return;
// }

// void pushRuleViaID(stack *S, int ruleID)
// {
//     // Error Incase pdtable is not accessible
//     if ((pdtable == NULL) || (pdtable->grammarrules == NULL)){
//         printf("Production Table or GrammarRules is empty!\n");
//         return;
//     }
//     pushRule(S, pdtable->grammarrules[ruleID]);
//     return;
// }

/*Parsing Check*/
// Input Buffer contains string to be parsed followed by $ (ENDMARKER) 
// Program -> Considers X=>stack(top), a=>current(inputSymbol)

TOKEN* createTokenCopy(TOKEN* curTok) {
    TOKEN* temp = malloc(sizeof(TOKEN));
    if(curTok == NULL){
        free(temp);
        printf(GREEN BOLD "End of Stream of Tokens\n" RESET);
        return curTok;
    }
    memcpy(temp, curTok, sizeof(TOKEN));
    curTok = temp;
    return curTok;
}

Set* initSynchronizingSet(grammarElement* g) {
    Set* res = initSet(base);
    unionSet(res, firstSets[g->tokenID - base]);
    if (followSets[g->tokenID - base] != NULL) {
        unionSet(res, followSets[g->tokenID - base]);
    }
    res->contains[START] = true;
    res->contains[END] = true;
    res->contains[SEMICOL] = true;
    res->contains[DECLARE] = true;
    res->contains[DRIVERDEF] = true;
    res->contains[DRIVERENDDEF] = true;
    res->contains[DEF] = true;
    res->contains[ENDDEF] = true;
    res->contains[FOR] = true;
    res->contains[SWITCH] = true;
    res->contains[WHILE] = true;

    return res;
}

// Parse Table Check
void parse(){
    printf(UNDERLINE BOLD "Into Parser\n" RESET);

    stack * st = initStack();
    initParseStack(st);
    
    // Checking the Current Token and Top of the Stack
    TOKEN* curTok = getNextToken();
    curTok = createTokenCopy(curTok);

    if (curTok->tok == DOLLAR) { 
        printParseError(3,st->top,curTok);

        destroyStack(st);
        if(synCorrPrint) {printf(GREEN BOLD "Input source code is syntactically correct\n" RESET); synCorrPrint = true;}
        return;
    }

    stackNode * topStack = peekStack(st);

    while(!(isEmpty(st))){
        if (topStack->GE->tokenID == EPSILON) {
            popStack(st);
            topStack = peekStack(st);
            continue;
        }

        // top of the stack is terminal
        if(topStack->GE->isTerminal){
            
            if (topStack->GE->tokenID == curTok->tok) { // Match
                // Pop Stack, Update topStack variable
                // printf("terminal\t");
                // printf(GREEN BOLD "Top Stack: %-30s" RESET, topStack->GE->lexeme);
                // printf(GREEN BOLD "Current Token: %-20s\t" RESET, curTok->lexeme);
                // printf(GREEN BOLD "MATCHED\n" RESET);

                if (curTok->tok == DOLLAR) { 
                    break; 
                }
                topStack->nodeAddr->tok = curTok;

                popStack(st);
                topStack = peekStack(st);

                // Move Ahead i.e. Fetch another Token
                curTok = getNextToken();
                curTok = createTokenCopy(curTok);

                // if (curTok->tok == DOLLAR && st->size > 1) { 
                //     printParseError(3,st->top,curTok);

                //     destroyStack(st);
                //     return;
                // }
            } else {
                printParseError(2,st->top,curTok);

                // TODO: REPORT ERROR
                // reportError();
                // popStack(st);
                // topStack = peekStack(st);
                // curTok = getNextToken();
                // curTok = createTokenCopy(curTok);

                // if (curTok == NULL) { 
                //     printParseError(1,st->top,curTok);
                //     return;
                // }

                while (curTok->tok != SEMICOL && curTok->tok != START && curTok->tok != END) {
                    free(curTok);
                    curTok = getNextToken();
                    curTok = createTokenCopy(curTok);
                    if (curTok->tok == DOLLAR) { 
                        break;
                    }
                }

                while ((topStack = peekStack(st))) {
                    if (topStack->GE->isTerminal && topStack->GE->tokenID == curTok->tok) {
                        break;
                    } else if (!topStack->GE->isTerminal && parseTable[topStack->GE->tokenID - base][curTok->tok] != -1) {
                        break;
                    }
                    popStack(st);
                }
            }

        } else {
            // printf("nonTerminal\t");
            // printf("Top Stack: %-30s", topStack->GE->lexeme);
            // printf("Current Token: %-20s\n", curTok->lexeme);
            // Use curToken, parseTable to pop current Element and Push Rule
            int nonTerminalID = topStack->GE->tokenID;
            int terminalID = curTok->tok;
            int ruleID = parseTable[nonTerminalID - base][terminalID];
            // printf("(%d)%s, (%d)%s, %d\n", nonTerminalID - base, elements[nonTerminalID], terminalID, elements[terminalID], ruleID);
            
            if(ruleID == -1){
                synCorrPrint = false;
                printf(RED BOLD "[Parser] Line: %d Error in the input as no entry found in parseTable[%s][%s]\n" RESET, curTok->linenum, topStack->GE->lexeme, elements[curTok->tok]);
                
                // TODO: REPORT ERROR
                // reportError()
                Set* synchronizingSet = initSynchronizingSet(topStack->GE);

                bool once = true;
                while (synchronizingSet->contains[curTok->tok] == false) {
                    free(curTok);
                    curTok = getNextToken();
                    curTok = createTokenCopy(curTok);
                    if (curTok->tok == DOLLAR && st->size > 1) { 
                        if (once) {
                            once = false;
                            continue;
                        }
                        printParseError(3,st->top,curTok);

                        destroyStack(st);
                        destroySet(synchronizingSet);
                        if(synCorrPrint) {printf(GREEN BOLD "Input source code is syntactically correct\n" RESET); synCorrPrint = true;}
                        return;
                    }
                } 

                destroySet(synchronizingSet);
                popStack(st);
                topStack = peekStack(st);
                free(curTok);
                curTok = getNextToken();
                curTok = createTokenCopy(curTok); 
            } else {
                // Pop current nonTerminal, push Rule, update topStack
                TreeNode* topStackAddr = topStack->nodeAddr;
                popStack(st);
                insertRuleInParseTree(topStackAddr, ruleID, curTok, st);
                topStack = peekStack(st);
            }
        }

    }

    if (curTok->tok != EOF_SYMBOL) {
        printf(RED BOLD "The stack is empty but the stream has not ended.\n" RESET);
    }

    while(curTok->tok != EOF_SYMBOL){
        curTok = getNextToken();
        
    }

    free(curTok);
    destroyStack(st);
    if(synCorrPrint) {printf(GREEN BOLD "Input source code is syntactically correct\n" RESET); synCorrPrint = true;}
    return;
}

void freePDTable(ProductionTable* pdtable) {
    int cnt = pdtable->ruleCount;
    for (int i = 0; i < cnt; ++i) {
        free(pdtable->grammarrules[i]->LHS);
        grammarElement* head = pdtable->grammarrules[i]->RHSHead;
        while (head != NULL) {
            grammarElement* temp = head->next;
            free(head);
            head = temp;
        }
        destroySet(pdtable->grammarrules[i]->firstSet);
        free(pdtable->grammarrules[i]);
    }
    free(pdtable->grammarrules);
    free(firstSetsRules);
    free(pdtable);
    return;
}

void freeFirstAndFollowSets(int nonTerminalLen) {
    for (int i = 0; i < nonTerminalLen; ++i) {
        destroySet(firstSets[i]);
        if (followSets[i] != NULL) {
            destroySet(followSets[i]);
        } 
    }

    free(firstSets);
    free(followSets);
}

void freeElements(char** elements, int count) {
    for (int i = 0; i < count; ++i) {
        free(elements[i]);
    }

    free(elements);
    return;
}

void freeRuleLocs(int nonTerminalLen) {
    for (int i = 0; i < nonTerminalLen; ++i) {
        listElement* LHSHead = LHSLoc[i];
        while (LHSHead != NULL) {
            listElement* temp = LHSHead->next;
            free(LHSHead);
            LHSHead = temp;
        }
        listElement* RHSHead = RHSLoc[i];
        while (RHSHead != NULL) {
            listElement* temp = RHSHead->next;
            free(RHSHead);
            RHSHead = temp;
        }
    }

    free(LHSLoc);
    free(RHSLoc);
}

void freeParseTreeRec(TreeNode* node) {
    if (node == NULL) { return; }

    freeParseTreeRec(node->child);
    if (node->tok != NULL) {
        free(node->tok);
    }

    freeParseTreeRec(node->next);
    free(node);
    return;
}

void freeParseTree(ParseTree* parseTree) {
    freeParseTreeRec(parseTree->root);
    free(parseTree);
    return;
}

void cleanParser() {
    synCorrPrint = true;

    int nonTerminalLen = grammarTrie->count - base;
    int trSize = grammarTrie->count;
    freePDTable(pdtable);
    pdtable = NULL;
    firstSetsRules = NULL;

    freeTrie(grammarTrie);
    grammarTrie = NULL;

    freeFirstAndFollowSets(nonTerminalLen);
    firstSets = NULL;
    followSets = NULL;

    freeElements(elements, trSize);
    elements = NULL;

    free(computed);
    computed = NULL;

    freeRuleLocs(nonTerminalLen);
    LHSLoc = NULL;
    RHSLoc = NULL;

    freeParseTree(parseTree);
    base = 0;
    return;
}


//##########################################

void parserMain(char *userSourceCode, char* parseTreeOutput) {
    // Setup 
    // if (argc != 2) {
    //     printf(YELLOW BOLD "Usage: ./a.out <filename>\n" RESET);
    //     return 1;
    // }

    // FILE* fp = fopen(argv[1], "r");
    FILE* fp = fopen(userSourceCode, "r");
    if (fp == NULL) {
        printf(RED BOLD "File Not Found.\n" RESET);
        exit(1);
    }

    setupLexer(fp);

    grammarTrie = setupTrie();
    populateGrammarTrie(grammarTrie);
    int terminalTrieLen = grammarTrie->count; // it only contains terminals right now
    pdtable = initializeProductionTable(pdtable, TOTAL_RULES);
    char *grammarFile = "grammar.txt";
    FILE *f = fopen(grammarFile, "r");
    if(f==NULL)
        printf(RED BOLD "File error: Unable to open Grammar File\n" RESET);
    char *line = NULL;
    size_t len = 0;
    size_t read = 0; // number of bytes read in a line
    int nonTerminalID = grammarTrie->count;
    // Adding epsilon and $ to allow easier indexing in first and follow sets;
    nonTerminalID = insertWord(grammarTrie, "e", nonTerminalID);
    nonTerminalID = insertWord(grammarTrie, "$", nonTerminalID);
    int productionRuleID = 0;
    while ((read = getline(&line, &len, f)) != -1) {
        if(line[read-1]=='\n') line[read-1] = '\0';
        // printf("%s\n", line);
        ProductionRule *p = (ProductionRule*)malloc(sizeof(ProductionRule));
        char temp[read];
        strcpy(temp, line);
        char *tok = strtok(temp, " ");

        //Left side of Production Rule
        p->LHS = (grammarElement*)malloc(sizeof(grammarElement));
        p->LHS->isTerminal = false;
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
    }

    free(line);
    computeFirstSet(grammarTrie->count - terminalTrieLen - 2, terminalTrieLen);
    computeFollowSet(grammarTrie->count - terminalTrieLen - 2, terminalTrieLen);
    attachFollowToRule();
    
    // printProductionTable(pdtable);

    computeParseTable();

    initParseTree();
    parse();
    printParseTree(parseTree, parseTreeOutput);
    
    fclose(f);
    fclose(fp);
    return;
}
