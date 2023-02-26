#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
typedef struct stackNode{
    grammarElement  * GE;
    struct stackNode * next;
} stackNode;

typedef struct stack{
    stackNode * top;
    int size;
} stack;
stackNode *getStackNode(grammarElement *GE);
stack *initStack(void);
grammarElement *peekStack(stack *st);
grammarElement *popStack(stack *S);
void pushStackGE(stack *S, grammarElement *GE);
bool isEmpty(stack * S);
