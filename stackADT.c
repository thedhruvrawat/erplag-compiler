#include "stackADT.h"
// TESTING IS LEFT

// Creates a node of stack for a given Grammar Element
stackNode *getStackNode(grammarElement *GE)
{
    stackNode *node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
    node->nodeAddr = NULL;
}

// Returns a new Stack 
stack *initStack(void)
{
    stack *st = malloc(sizeof(stack));
    st->top = NULL;
    st->size = 0;
    return st;
}

// Returns the a pointer to the stackNode to the top of stack
stackNode *peekStack(stack *st)
{
    return st->top;
}

// Pops the stack => Free the allocated memory
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

// Pushes node onto the stack encapsulating Grammar Element, Address of Parse Node in the Tree
void pushStackGE(stack *st, grammarElement *GE, TreeNode* nodeAddr)
{
    
    stackNode *newTop = getStackNode(GE);
    newTop->next = st->top;
    newTop->nodeAddr = nodeAddr;
    
    st->top = newTop;
    st->size++;

    return;
}

// Returns is the stack is empty
bool isEmpty(stack * st){
    if(st->size)
        return false;
    else
        return true;
}