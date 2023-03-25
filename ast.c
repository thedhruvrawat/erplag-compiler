/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astDef.h"

AST* tree;
ASTStack* st;
ParseTree* pt;

ASTNodeList* initASTNodeList(void) {
    ASTNodeList* list = malloc(sizeof(ASTNodeList));
    list->head = NULL;
    list->tail = NULL;
    list->sz = 0;
}

AST* initAST(void) {
    AST* tree = malloc(sizeof(AST));
    tree->root = NULL;
    tree->size = 0; 

    return tree;
}

ASTNode* createASTNode(const char* label, ParseTreeNode* node) {
    ASTNode* newNode = malloc(sizeof(ASTNode));
    strcpy(newNode->label, label);
    newNode->parseTreeNode = node;
    newNode->leftMostChild = NULL;
    newNode->rightMostChild = NULL;
    newNode->next = NULL;

    tree->size++;
    return newNode;
}

void insertAtEnd(ASTNodeList* list, ASTNode* node) {
    if (list == NULL) {
        list = initASTNodeList();
    }

    if (list->sz == 0) {
        list->head = node;
        list->tail = list->head;
        list->sz++;
        return;
    }

    list->tail->next = node;
    list->tail = node;
    list->sz++;

    return;
}

void insertAtStart(ASTNodeList* list, ASTNode* node) {
    if (list == NULL) {
        list = initASTNodeList();
    }

    if (list->sz == 0) {
        list->head = node;
        list->tail = list->head;
        list->sz++;
        return;
    }

    list->head->prev = node;
    node->next = list->head;
    list->head = node;
    list->sz++;

    return;
}

void appendASTNodeAsChild(ASTNode* node, ASTNode* parent) {
    if (parent->leftMostChild == NULL) {
        parent->leftMostChild = node;
        parent->rightMostChild = node;
        return;
    }

    parent->rightMostChild->next = node;
    node->prev = parent->rightMostChild;
    parent->rightMostChild = node;
    
    return;
}

ASTStackNode* createASTStackNode(ParseTreeNode* parseTreeNode, ASTNode* par) {
    ASTStackNode* newNode = (ASTStackNode*) malloc(sizeof(ASTStackNode));
    newNode->parseTreeNode = parseTreeNode;
    newNode->parent = par;
    newNode->next = NULL;

    return newNode;
}

ASTStack* initASTStack(void) {
    ASTStack* st = malloc(sizeof(ASTStack));
    st->top = NULL;
    st->size = 0;

    return st;
}

ASTStackNode* peekASTStack(ASTStack* st) {
    return st->top;
}

void popASTStack(ASTStack* st) {
    if (st->size == 0) {
        printf("AST Stack is empty!\n");
        return;
    }

    ASTStackNode* currTop = st->top;

    st->top = st->top->next;
    st->size--;

    // free(currTop);
    return;
}

void pushASTStack(ASTStack* st, ParseTreeNode* parseTreeNode, ASTNode* par) {
    ASTStackNode* currTop = st->top;
    ASTStackNode* newNode = createASTStackNode(parseTreeNode, par);

    newNode->next = currTop;
    st->top = newNode;
    st->size++;

    return;
}

void pushChildrenToASTStack(ASTStack* st, ASTNode* par, ParseTreeNode* curr) {
    if (curr == NULL) {
        return;
    }

    pushChildrenToASTStack(st, par, curr->next);
    pushASTStack(st, curr, par);

    return;
}

bool isUsefulTerminal(ASTStackNode* node) {
    switch (node->parseTreeNode->tokenID) {
        case INTEGER:
        case REAL:
        case BOOLEAN:
        case ARRAY:
        case GET_VALUE:
        case PRINT:
        case ID:
        case NUM:
        case RNUM:
        case PLUS:
        case MINUS:
        case MUL:
        case DIV:
        case LT:
        case LE:
        case GE:
        case GT:
        case EQ:
        case NE:
        case ASSIGNOP:
        case TRUE:
        case FALSE:
        case AND:
        case OR:
        case EPS:
            return true;
        default:
            return false;
    }
}

void createAST(void) {
    // Initializing the root node of the AST
    tree = initAST();

    // Initializing the stack
    st = initASTStack();
    pushASTStack(st, pt->root, NULL);
    printf("%d\n", pt->root->productionID);
    printf("%d\n", st->top->parseTreeNode->productionID);
    printf("%p\n", st->top);
    printf("%p\n", st->top->parseTreeNode);

    ASTNode* moduleDecNode;

    while (st->size > 0) {
        ASTStackNode* node = peekASTStack(st);
        popASTStack(st);
        printf("prodID: %d\n", node->parseTreeNode->productionID);

        if (node->parseTreeNode->isLeaf) {
            if (isUsefulTerminal(node)) {
                printf("Terminal\n");
                ASTNode* leafNode = createASTNode(token_types[node->parseTreeNode->tokenID], node->parseTreeNode);
                printf("%p\n", node->parent);
                printf("%s\n", node->parseTreeNode->tok->lexeme);
                appendASTNodeAsChild(leafNode, node->parent);
            }
        } else {
            switch (node->parseTreeNode->productionID) {
                case 0: { // <program> = <moduleDeclarations> <otherModules> <driverModule> <otherModules> 
                    ASTNode* programNode = createASTNode("PROGRAM", node->parseTreeNode);
                    pushChildrenToASTStack(st, programNode, node->parseTreeNode->child);
                    moduleDecNode = createASTNode("MODULEDEC", node->parseTreeNode);
                    appendASTNodeAsChild(moduleDecNode, programNode);
                    tree->root = programNode;
                    printf("programNode: %p\n", programNode);
                    break;
                }
                case 1: { // <moduleDeclarations> = <moduleDeclaration> <moduleDeclarations>
                    pushASTStack(st, node->parseTreeNode->child, moduleDecNode);
                    break;
                }
                case 2: { // <moduleDeclarations> = e
                    continue;
                }
                case 3: { // <moduleDeclaration> = DECLARE MODULE ID SEMICOL
                    ASTNode* moduleNode = createASTNode("ID", node->parseTreeNode->child->next->next);
                    appendASTNodeAsChild(moduleNode, moduleDecNode);
                    break;
                }
                case 4: { // <otherModules> = <module>  <otherModules>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 5: { // <otherModules> = e
                    continue;
                }
                case 6: { // <driverModule> = DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
                    ASTNode* driverNode = createASTNode("DRIVER", node->parseTreeNode);
                    printf("%p\n", node->parent);
                    appendASTNodeAsChild(driverNode, node->parent);
                    break;
                }
                case 7: { // <module> = DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
                    ASTNode* moduleNode = createASTNode("MODULE", node->parseTreeNode);
                    printf("%p\n", node->parent);
                    appendASTNodeAsChild(moduleNode, node->parent);
                    pushChildrenToASTStack(st, moduleNode, node->parseTreeNode->child);
                    break;
                }
                case 8: { // <ret> = RETURNS SQBO <output_plist> SQBC SEMICOL
                    ASTNode* retNode = createASTNode("RETURNS", node->parseTreeNode);
                    appendASTNodeAsChild(retNode, node->parent);
                    pushChildrenToASTStack(st, retNode, retNode->parseTreeNode->child);
                    break;
                }
                case 9: { // <ret> = e 
                    ASTNode* retNode = createASTNode("RETURNS", node->parseTreeNode);
                    appendASTNodeAsChild(retNode, node->parent);
                    break;
                }
                default: {
                    continue;
                }
            }
        }
    }
    return;
}

void printAST(ASTNode* node, bool firstChild) {
    if (node == NULL) {
        return;
    }

    printAST(node->leftMostChild, true);

    printf("%p\t", node);
    if (node->parseTreeNode->isLeaf) {
        printf("ASTNode: %s\t%s\n", node->label, node->parseTreeNode->tok->lexeme);
    } else {
        printf("ASTNode: %s\n", node->label);
    }

    if (node->leftMostChild != NULL) {
        printAST(node->leftMostChild->next, false);
    }    

    if (!firstChild) {
        printAST(node->next, false);
    }

    return;
}

void ASTCreator(ParseTree* parseTree) {
    pt = parseTree;
    createAST();
    printAST(tree->root, false);

    printf("printing level 1: \n");
    ASTNode* curr = tree->root->leftMostChild;
    while (curr != NULL) {
        if (curr->parseTreeNode->isLeaf) {
            printf("%s\t%s\n", curr->label, curr->parseTreeNode->tok->lexeme);
        } else {
            printf("%s\n", curr->label);
        }
        curr = curr->next;
    }

    return;
}