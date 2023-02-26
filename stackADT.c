#include "stackADT.h"
// TESTING IS LEFT
stackNode *getStackNode(grammarElement *GE)
{
    stackNode *node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
}

stack *initStack(void)
{
    stack *st = malloc(sizeof(stack));
    st->top = NULL;
    st->size = 0;
    return st;
}

grammarElement *peekStack(stack *st)
{
    if(st->size != 0)
        return st->top->GE;
    else
        return NULL;
}

grammarElement *popStack(stack * st)
{
    if (st->size == 0)
    {
        printf("Stack is empty!\n");
        return NULL;
    }

    stackNode *currTop = st->top;
    grammarElement *GE = currTop->GE;

    st->top = st->top->next;
    st->size--;
    free(currTop);

    return GE;
}

void pushStackGE(stack *st, grammarElement *GE)
{
    
    stackNode *newTop = getStackNode(GE);
    newTop->next = st->top;
    
    st->top = newTop;
    st->size++;
}

bool isEmpty(stack * st){
    if(st->size)
        return false;
    else
        return true;
}