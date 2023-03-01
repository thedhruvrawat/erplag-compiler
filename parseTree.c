/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
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