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
