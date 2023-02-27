#include "grammar.h"

Trie *grammarTrie;

#define TOTAL_RULES 200     //total number of rules in grammar
#define NULL_RULES 50       //expected number of nullable rules

ProductionTable *pdtable;           //stores all rules
ProductionTable *nullpdtable;       //stores only NULLABLE rules

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

void printParseError(int p_errno, stackNode * top ,TOKEN* tok){
    switch(p_errno){
        case 1:
            {
                // printf("Stream has ended but the stack is non-empty\n\n");
                printf("Parse Error: Expected %s \n\n",top->GE->lexeme);
                break;
            }
        case 2:
        {
            // printf("Top of Stack is Terminal: %s, Token is not!\n\n", topStack->GE->lexeme);
            printf("Parse error: Expected %s \n\n",top->GE->lexeme);
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
        currNode->tok = tok;
        currNode->tokenID = g->tokenID;
        currNode->productionID = productionID;
        currNode->tokenDerivedFrom = parent->tokenID;
        currNode->isLeaf = g->isTerminal;
        currNode->next = head;
        head = currNode;
        pushStackGE(st, g, currNode);
        g = g->prev;
    }

    parseTree->treeDepth = MAX(parseTree->treeDepth, parent->depth + 1);
    parent->child = head;
    return;
}

void printParseTreeRec(TreeNode* node, FILE* fp) {
    if (node == NULL) { 
        return;
    }

    printParseTreeRec(node->child, fp);

    if (node->isLeaf) {
        fprintf(fp, "%-25s%-10d%-15s", node->tok->lexeme, node->tok->linenum, elements[node->tok->tok]);
    } else {
        fprintf(fp, "%-25s%-10s%-15s", "--------------------", "---", "----------");
    }

    if (node->isLeaf && node->tok->tok == NUM) {
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

    printParseTreeRec(node->next, fp);
    return;
}

void printParseTree(ParseTree* parseTree, char* outFile) {
    FILE* fp = fopen(outFile, "w");

    if (fp == NULL) {
        printf("Unable to open the file to write parseTree.\n");
        exit(1);
    }

    printParseTreeRec(parseTree->root, fp);
    return;
}
//############################################

// Setting Up STACK for the Parsing of given Input File
// Pushes Dollar, Followed by <program> into the Stack
void initParseStack(stack * st){
    grammarElement *dollar = (grammarElement*) malloc(sizeof(grammarElement));
    dollar->isTerminal = true;
    dollar->tokenID = DOLLAR;
    strcpy(dollar->lexeme, "$");
    dollar->next = NULL; dollar->prev = NULL;

    pushStackGE(st, dollar, NULL);

    // push <program> into stack
    grammarElement * S = (grammarElement*) malloc(sizeof(grammarElement));
    S->isTerminal = false;
    S->tokenID = searchWord(grammarTrie, "<program>");
    strcpy(S->lexeme, "<program>");
    S->next = NULL; S->prev = NULL;

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
        printf("End of Stream of Tokens\n");
        return curTok;
    }
    memcpy(temp, curTok, sizeof(TOKEN));
    curTok = temp;
    return curTok;
}

Set* initSynchronizingSet(grammarElement* g) {
    Set* res = initSet(base);
    int SEMCOL = searchGrammar(grammarTrie, "SEMICOL");
    unionSet(res, firstSets[g->tokenID - base]);
    if (followSets[g->tokenID - base] != NULL) {
        unionSet(res, followSets[g->tokenID - base]);
    }
    res->contains[SEMCOL] = true;

    return res;
}

// Parse Table Check
void parse(){

    printf("Into Parser\n");

    stack * st = initStack();
    initParseStack(st);
    
    // Checking the Current Token and Top of the Stack
    TOKEN* curTok = getNextToken();
    curTok = createTokenCopy(curTok);

    if (curTok == NULL) { 
        printParseError(1,st->top,curTok);
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
                printf("terminal\t");
                printf("Top Stack: %-30s", topStack->GE->lexeme);
                printf("Current Token: %-20s\t", curTok->lexeme);
                printf("MATCHED\n");

                topStack->nodeAddr->tok = curTok;

                popStack(st);
                topStack = peekStack(st);
                // Need to add the relation of GrammarElement to TOKEN
                // Add to ParseTree
                // #####################

                // Move Ahead i.e. Fetch another Token
                curTok = getNextToken();
                curTok = createTokenCopy(curTok);

                if (curTok == NULL) { 
                    printParseError(1,st->top,curTok);
                    return;
                }
            } else {
                // printf("Top of Stack is Terminal: %s, Token is not!\n\n", topStack->GE->lexeme);
                printParseError(2,st->top,curTok);

                // TODO: REPORT ERROR
                // reportError();
                popStack(st);
                topStack = peekStack(st);
            }

        } else {
            printf("nonTerminal\t");
            printf("Top Stack: %-30s", topStack->GE->lexeme);
            printf("Current Token: %-20s\n", curTok->lexeme);
            // Use curToken, parseTable to pop current Element and Push Rule
            int nonTerminalID = topStack->GE->tokenID;
            int terminalID = curTok->tok;
            int ruleID = parseTable[nonTerminalID - base][terminalID];
            printf("(%d)%s, (%d)%s, %d\n", nonTerminalID - base, elements[nonTerminalID], terminalID, elements[terminalID], ruleID);
            
            if(ruleID == -1){
                printf("No entry in parseTable[%s][%s]\n\n", topStack->GE->lexeme, elements[curTok->tok]);
                
                // TODO: REPORT ERROR
                // reportError()
                Set* synchronizingSet = initSynchronizingSet(topStack->GE);

                while (synchronizingSet->contains[curTok->tok] == false) {
                    free(curTok);
                    curTok = getNextToken();
                    curTok = createTokenCopy(curTok);
                    if (curTok == NULL) { 
                        printf("Stream has ended but the stack is non-empty\n");
                        return;
                    }
                } 

                popStack(st);
                topStack = peekStack(st);
                curTok = getNextToken();
                curTok = createTokenCopy(curTok); 

                if (curTok == NULL) { 
                    // printf("Stream has ended but the stack is non-empty\n");
                    printParseError(1,st->top,curTok);
                    return;
                }
            } else {
                // Pop current nonTerminal, push Rule, update topStack
                TreeNode* topStackAddr = topStack->nodeAddr;
                popStack(st);
                insertRuleInParseTree(topStackAddr, ruleID, curTok, st);
                topStack = peekStack(st);
            }
        }

    }
    
}
//##########################################

int main(int argc, char* argv[]) {
    // Setup 
    // if (argc != 2) {
    //     printf("Usage: ./a.out <filename>\n");
    //     return 1;
    // }

    FILE* fp = fopen("test_cases/t6_errors.txt", "r");
    if (fp == NULL) {
        printf("File Not Found.\n");
        exit(1);
    }

    setupLexer(fp);

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

    printProductionTable(pdtable);

    computeParseTable();

    initParseTree();
    parse();
    printParseTree(parseTree, "parseTable.txt");
    

    // #####################################################
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
    fclose(fp);
    return 0;
}

