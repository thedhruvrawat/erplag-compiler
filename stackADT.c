#include "stackADT.h"
// TESTING IS LEFT
stackNode *getStackNode(grammarElement *GE)
{
    stackNode *node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
    node->nodeAddr = NULL;
}

stack *initStack(void)
{
    stack *st = malloc(sizeof(stack));
    st->top = NULL;
    st->size = 0;
    return st;
}

stackNode *peekStack(stack *st)
{
    return st->top;
}
void popStack(stack * st)
{
    if (st->size == 0)
    {
        printf("Stack is empty!\n");
        return ;
    }// top of the stack is non-terminal

    stackNode *currTop = st->top;

    st->top = st->top->next;
    st->size--;
    free(currTop);
    
    return;
}

void pushStackGE(stack *st, grammarElement *GE, TreeNode* nodeAddr)
{
    
    stackNode *newTop = getStackNode(GE);
    newTop->next = st->top;
    newTop->nodeAddr = nodeAddr;
    
    st->top = newTop;
    st->size++;

    return;
}

bool isEmpty(stack * st){
    if(st->size)
        return false;
    else
        return true;
}