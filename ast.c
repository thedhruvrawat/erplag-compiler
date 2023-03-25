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

    tree->size++; // New Node created -> It WILL be added to AST
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

void appendASTNodeAsChild(ASTNode* node, ASTNode* parent) { // Appends node to list of children of ASTNode parent with start,end of list pointed by leftMostChild, rightMostChildren
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

ASTStackNode* createASTStackNode(ParseTreeNode* parseTreeNode, ASTNode* par) { // 
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

void pushChildrenToASTStack(ASTStack* st, ASTNode* par, ParseTreeNode* curr) { // Inserts "curr" parseTreeNode into ASTstack "st" along with its siblings with "par" ASTNode as parent
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
                case 124: { // <declareStmt> = DECLARE <idList> COLON <dataType> SEMICOL
                    ASTNode* declareStmtNode = createASTNode("DECLARE", node->parseTreeNode);
                    appendASTNodeAsChild(declareStmtNode, node->parent);
                    pushChildrenToASTStack(st, declareStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 125: { // <conditionalStmt> = SWITCH BO ID BC START <caseStmts> <default> END
                    ASTNode* condStmtNode = createASTNode("SWITCH", node->parseTreeNode);
                    appendASTNodeAsChild(condStmtNode, node->parent);
                    pushChildrenToASTStack(st, condStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 126: { // <caseStmts> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", node->parseTreeNode);
                    appendASTNodeAsChild(caseStmtNode, node->parent);
                    pushChildrenToASTStack(st, caseStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 127: { // <N9> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", node->parseTreeNode);
                    appendASTNodeAsChild(caseStmtNode, node->parent);
                    pushChildrenToASTStack(st, caseStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 128: { // <N9> = e
                    break;
                }
                case 129: { // <value> = NUM
                    ASTNode* valueNode = createASTNode("NUM", node->parseTreeNode->child);
                    appendASTNodeAsChild(valueNode, node->parent);
                    break;
                }
                case 130: { // <value> = TRUE
                    ASTNode* valueNode = createASTNode("TRUE", node->parseTreeNode->child);
                    appendASTNodeAsChild(valueNode, node->parent);
                    break;
                }
                case 131: { // <value> = FALSE 
                    ASTNode* valueNode = createASTNode("FALSE", node->parseTreeNode->child);
                    appendASTNodeAsChild(valueNode, node->parent);
                    break;
                }
                case 132: { // <default> = DEFAULT COLON <statements> BREAK SEMICOL 
                    ASTNode* defaultNode = createASTNode("DEFAULT", node->parseTreeNode->child);
                    appendASTNodeAsChild(defaultNode, node->parent);
                    pushChildrenToASTStack(st, defaultNode, node->parseTreeNode->child->next->next);
                    break; 
                }
                case 133: { // <default> = e 
                    break;
                }
                case 134: { // <iterativeStmt> = FOR BO ID IN <range_for_loop> BC START <statements> END 

                    ASTNode* iterStmt = createASTNode("FOR", node->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, node->parent);
                    pushChildrenToASTStack(st, iterStmt, node->parseTreeNode->child);
                    break;
                }
                case 135: { // <iterativeStmt> = WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END 
                    ASTNode* iterStmt = createASTNode("WHILE", node->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, node->parent);
                    pushChildrenToASTStack(st, iterStmt, node->parseTreeNode->child);
                    break;
                }
                case 136: { // <range_for_loop> = <index_for_loop> RANGEOP <index_for_loop>
                // Since we have done syntactic checking we aren't using RANGE_FOR_LOOP, But again we don't if we might need later.
                    ASTNode* rangeForLoop = createASTNode("RANGE_FOR_LOOP", node->parseTreeNode);
                    appendASTNodeAsChild(rangeForLoop, node->parent);
                    pushChildrenToASTStack(st, rangeForLoop, node->parseTreeNode->child);
                    break;
                }
                case 137: { // <index_for_loop> = <sign_for_loop> <new_index_for_loop> 
                    ASTNode* indexForLoop = createASTNode("INDEX_FOR_LOOP", node->parseTreeNode);
                    appendASTNodeAsChild(indexForLoop, node->parent);
                    pushChildrenToASTStack(st, indexForLoop, node->parseTreeNode->child);
                    break;
                }
                case 138: { //<new_index_for_loop> = NUM 
                    ASTNode * numNode = createASTNode("NUM", node->parseTreeNode->child);
                    appendASTNodeAsChild(numNode, node->parent);
                    break;
                }
                case 139: { // <sign_for_loop> = PLUS
                    ASTNode * plusNode = createASTNode("PLUS", node->parseTreeNode->child);
                    appendASTNodeAsChild(plusNode, node->parent);
                    break;
                }
                case 140: { // <sign_for_loop> = MINUS
                    ASTNode * minusNode = createASTNode("MINUS", node->parseTreeNode->child);
                    appendASTNodeAsChild(minusNode, node->parent);
                    break;
                }
                case 141: { // <sign_for_loop> = e 
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