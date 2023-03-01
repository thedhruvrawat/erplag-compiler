/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
#include "stackADT.h"
// TESTING IS LEFT

// Creates a node of stack for a given Grammar Element
stackNode *getStackNode(grammarElement *GE)
{
    stackNode *node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
    node->nodeAddr = NULL;
    return node;
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
    }

    stackNode *currTop = st->top;

    st->top = st->top->next;
    st->size--;
    if (st->size == 0) {
        free(currTop->GE);
    }
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

void destroyStack(stack* st) {
    while (!isEmpty(st)) {
        popStack(st);
    }

    free(st);
}