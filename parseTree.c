#include "parseTree.h"

typedef struct parseNode{
    parseNode *parent; // It points to GE from where RULE is derived
    parseNode *child; // It points to RHSHead
    grammarElement * GE; // It points to LHS
} parseNode;

typedef struct parseTree{
    parseNode *root ;

} parseTree;

void initParseTree(void){
    parseTree * pT = (parseTree *) malloc(sizeof(parseTree));
    pT->root = NULL;

    return pT;
}
// void makeLeafNode(T, [b]) // if T is a terminal 
// void treeNodeOfChildren()