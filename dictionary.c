#include "dictionary.h"
#include "trie.h"

Trie *nonTerminalTrie;

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
        printf("[%d] %s -> ", i, pdtable->grammarrules[i - 1]->LHS->lexeme);
        grammarElement *ptr = pdtable->grammarrules[i - 1]->RHS;
        while(ptr->next!=NULL) {
            printf("%s ", ptr->lexeme);
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
    pdtable = initializeProductionTable(pdtable, TOTAL_RULES);
    nullpdtable = initializeProductionTable(nullpdtable, NULL_RULES);
    char *grammarFile = "grammar.txt";
    FILE *f = fopen(grammarFile, "r");
    if(f==NULL)
        printf("File error\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0; // number of bytes read in a line
    while ((read = getline(&line, &len, f))!=-1) {
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

        //Right side of Production Rule
        grammarElement *RHSHead = malloc(sizeof(grammarElement));
        grammarElement* RHSCurrent = RHSHead;

        int count = 0; //to count number of elements in RHS
        
        while(tok) {
            count++;
            if(count>2) {
                if(strcmp(tok, "e")==0)
                    p->isEpsilon = true;
                strcpy(RHSCurrent->lexeme, tok);
                RHSCurrent->next = (grammarElement *)malloc(sizeof(grammarElement));
                RHSCurrent = RHSCurrent->next;
            }
            tok = strtok(NULL, " ");
        }
        RHSCurrent = NULL;
        p->RHScount = count - 2;
        p->RHS = RHSHead;
        insertRuleInProductionTable(pdtable, p);
        if(p->isEpsilon)
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

    fclose(f);
    return 0;
}