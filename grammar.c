#include "grammar.h"
#include "trie.h"

Trie *nonTerminalTrie, *grammarTrie;

#define TOTAL_RULES 100     //total number of rules in grammar
#define NULL_RULES 50       //expected number of nullable rules

ProductionTable *pdtable;           //stores all rules
ProductionTable *nullpdtable;       //stores only NULLABLE rules

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

void printProductionTable(ProductionTable *pdtable) {
    printf("Printing Production Table\n");
    int sz = pdtable->ruleCount;
    for (int i = 1; i <= sz; i++) {
        printf("[%d] (%d)%s -> ", pdtable->grammarrules[i-1]->productionID, pdtable->grammarrules[i - 1]->LHS->tokenID, pdtable->grammarrules[i - 1]->LHS->lexeme);
        grammarElement *ptr = pdtable->grammarrules[i - 1]->RHSHead;
        while(ptr!=NULL) {
            printf("(%d)%s", ptr->tokenID, ptr->lexeme);
            if(ptr->isTerminal)
                printf("*"); // Print a star, if the element is a terminal
            printf(" ");
            ptr = ptr->next;
        }
        printf("\n");
    }
    return;
}

void insertRuleInProductionTable(ProductionTable *pdtable, ProductionRule *p) {
    if(pdtable->ruleCount == pdtable->maxRules - 1) {
        printf("Sorry, table is full\n");
        return;
    }
    pdtable->grammarrules[pdtable->ruleCount] = p;
    pdtable->ruleCount++;
}

int main() {
    nonTerminalTrie = setupTrie();
    grammarTrie = setupTrie();
    populateGrammarTrie(grammarTrie);
    pdtable = initializeProductionTable(pdtable, TOTAL_RULES);
    nullpdtable = initializeProductionTable(nullpdtable, NULL_RULES);
    char *grammarFile = "grammar.txt";
    FILE *f = fopen(grammarFile, "r");
    if(f==NULL)
        printf("File error\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0; // number of bytes read in a line
    int nonTerminalID = 0;
    int productionRuleID = 1;
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
        int nonTerminalCheck = searchGrammar(nonTerminalTrie, tok); //Check if non-terminal already exists in NT Trie
        if(nonTerminalCheck == -1) {
            nonTerminalID = insertWord(nonTerminalTrie, tok, nonTerminalID); //It doesn't exist, so add it
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
                strcpy(newElement->lexeme, tok); //copy the token string to the new element
                int terminalCheck = searchGrammar(grammarTrie, tok); //check if it is a terminal
                if (terminalCheck != -1) {
                    newElement->isTerminal = true;
                    newElement->tokenID = terminalCheck; //set tokenID to its unique enum from terminalTrie
                } else {
                    newElement->isTerminal = false; // not found in terminal trie, this means it is a non-terminal
                    int nonTerminalCheck = searchGrammar(nonTerminalTrie, tok); // check if it exists in non-terminal trie
                    if(nonTerminalCheck == -1) {
                        nonTerminalID = insertWord(nonTerminalTrie, tok, nonTerminalID); //it doesn not exist in NT trie, so insert
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
    printProductionTable(pdtable);
    // printProductionTable(nullpdtable);

    //Listing all non-terminals for which FOLLOW needs to be calculated
    char **listNullables = findNullableTerminals(nullpdtable);
    printf("{");
    for (int i = 0; i < nullpdtable->ruleCount; i++) {        
        printf("%s,", listNullables[i]);        
    }
    printf("}\n");

    // Testing trie
    printf("%s\t%d\n", "<moduleDeclaration>", searchWord(nonTerminalTrie, "<moduleDeclaration>"));
    printf("%s\t%d\n", "<moduleDef>", searchWord(nonTerminalTrie, "<moduleDef>"));

    fclose(f);
    return 0;
}