/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include "parserDef.h"

typedef struct ASTNode {
    char label[20];
    ParseTreeNode* parseTreeNode;
    struct ASTNode* leftMostChild;
    struct ASTNode* rightMostChild;
    struct ASTNode* next;
    struct ASTNode* prev;
} ASTNode;

typedef struct AST {
    ASTNode* root;
    int size;
} AST;

/* 
typedef struct addrListNode {
    ASTNode* node;
    struct addrListNode* prev;
    struct addrListNode* next;
} addrListNode; */

typedef struct ASTNodeList {
    ASTNode* head;
    ASTNode* tail;
    int sz;
} ASTNodeList;

typedef struct ASTStackNode {
    ParseTreeNode* parseTreeNode;
    struct ASTStackNode* next;
    ASTNode* parent;
    ASTNode* addr;
    ASTNode* addrInherited;
    ASTNode* addrSynthesized;
    ASTNode* addrList;
} ASTStackNode;

typedef struct ASTStack {
    struct ASTStackNode* top;
    int size;
} ASTStack;