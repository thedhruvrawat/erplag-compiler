/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stackADT.h"

/**
 * @brief Creates a new stackNode for the given grammar element
 * 
 * @param GE 
 * @return stackNode* 
 */
stackNode *getStackNode(grammarElement *GE)
{
    stackNode *node = malloc(sizeof(stackNode));
    node->GE = GE;
    node->next = NULL;
    node->nodeAddr = NULL;
    return node;
}

/**
 * @brief Allocates memory for the stack from the heap and initializes it
 * 
 * @return stack* 
 */
stack *initStack(void)
{
    stack *st = malloc(sizeof(stack));
    st->top = NULL;
    st->size = 0;
    return st;
}

/**
 * @brief Returns the a pointer to the stackNode to the top of stack
 * 
 * @param st 
 * @return stackNode* 
 */
stackNode *peekStack(stack *st)
{
    return st->top;
}

/**
 * @brief Pops the stack
 * 
 * @param st 
 */
void popStack(stack * st)
{
    if (st->size == 0)
    {
        printf("Stack is empty!\n");
        return;
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

/**
 * @brief Pushes node onto the stack encapsulating Grammar Element, Address of Parse Node in the Tree
 * 
 * @param st 
 * @param GE 
 * @param nodeAddr 
 */
void pushStackGE(stack *st, grammarElement *GE, ParseTreeNode* nodeAddr)
{
    
    stackNode *newTop = getStackNode(GE);
    newTop->next = st->top;
    newTop->nodeAddr = nodeAddr;
    
    st->top = newTop;
    st->size++;

    return;
}

/**
 * @brief Returns if the stack is empty
 * 
 * @param st 
 * @return true 
 * @return false 
 */
bool isEmpty(stack * st){
    if(st->size)
        return false;
    else
        return true;
}

/**
 * @brief Frees the memory allocated for the stack
 * 
 * @param st 
 */
void destroyStack(stack* st) {
    while (!isEmpty(st)) {
        popStack(st);
    }

    free(st);
}