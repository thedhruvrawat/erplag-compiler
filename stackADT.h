/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef STACK_ADT
#define STACK_ADT
#include "parserDef.h"

typedef struct stackNode{
    grammarElement  * GE;
    struct stackNode * next;
    ParseTreeNode * nodeAddr;
} stackNode;

typedef struct stack{
    stackNode * top;
    int size;
} stack;
stackNode *getStackNode(grammarElement *GE);
stack *initStack(void);
stackNode *peekStack(stack *st);
void popStack(stack *S);
void pushStackGE(stack *S, grammarElement *GE, ParseTreeNode* nodeAddr);
bool isEmpty(stack * S);
void destroyStack(stack* st);

#endif
