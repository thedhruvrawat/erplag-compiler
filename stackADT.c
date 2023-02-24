#include "stackADT.h"

stackNode * getStackNode(grammarElement * GE){
    stackNode * node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
}

stack * initStack(void){
    stack * st = malloc(sizeof(stack));
    stack->top = NULL;
    stack->size = 0;
    return st;
}

grammarElement* top(stack * S){
    return s->top->GE;
}

void pushGE(stack * S, grammarElement * GE){
    grammarElement * currTop = top(S);
    stackNode * newTop = getStackNode(GE);
    newTop->next = currTop;
    S->size++;
}

void pushRuleID(stack * S, int ID){
    pushRule(S, pdtable->grammarrules[ID]);
    return;
}

void pushRule(stack * S, ProductionRule * rule){
    // Insert the Rule RHS from right to left
    grammarElement * curr = rule->RHSTail;
    while(curr != NULL){
        pushGE(S, curr);
        curr = curr->prev;
    }
    return;
}

grammarElement * pop(stack * S){
    if(S->size == 0){
        printf("Stack is empty!\n");
        return NULL;
    }

    stackNode * currTop = S->top;
    grammarElement * GE = currTop->GE;

    S->top = S->top->next;
    S->size--;
    free(currTop);
    
    return GE;
}