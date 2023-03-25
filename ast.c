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
    newNode->parent = NULL;
    newNode->parseTreeNode = node;
    newNode->leftMostChild = NULL;
    newNode->rightMostChild = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->numChildren = 0;

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
        node->parent = parent;
        parent->numChildren++;
        return;
    }

    parent->rightMostChild->next = node;
    node->prev = parent->rightMostChild;
    parent->rightMostChild = node;
    node->parent = parent;
    parent->numChildren++;
    
    return;
}

ASTStackNode* createASTStackNode(ParseTreeNode* parseTreeNode, ASTNode* par) {
    ASTStackNode* newNode = malloc(sizeof(ASTStackNode));
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
                    tree->root = programNode;
                    pushChildrenToASTStack(st, programNode, node->parseTreeNode->child);
                    moduleDecNode = createASTNode("MODULEDEC", node->parseTreeNode);
                    appendASTNodeAsChild(moduleDecNode, programNode);
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
                    break;
                }
                case 6: { // <driverModule> = DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
                    ASTNode* driverNode = createASTNode("DRIVER", node->parseTreeNode);
                    printf("%p\n", node->parent);
                    appendASTNodeAsChild(driverNode, node->parent);
                    pushChildrenToASTStack(st, driverNode, node->parseTreeNode->child);
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
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 9: { // <ret> = e 
                    break;
                }
                case 10: { // <input_plist> = ID COLON <dataType> <N1>
                    ASTNode* inputParaListNode = createASTNode("INPUTLIST", node->parseTreeNode);
                    appendASTNodeAsChild(inputParaListNode, node->parent);
                    printf("inputParaListNode: %p\n", inputParaListNode);
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, inputParaListNode);
                    printf("idDataTypeNode: %p\n", idDataTypeNode);
                    printf("idDataTypeNode: %p\n", idDataTypeNode->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 11: { // <N1> = COMMA ID COLON <dataType> <N1>
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    printf("parent of N1: %p\n", node->parent);
                    printf("parent of N1: %p\n", node->parent->parent);
                    appendASTNodeAsChild(idDataTypeNode, node->parent->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 12: { // <N1> = e
                    break;
                }
                case 13: { // <output_plist> = ID COLON <type> <N2>
                    ASTNode* outputParaListNode = createASTNode("OUTPUTLIST", node->parseTreeNode);
                    appendASTNodeAsChild(outputParaListNode, node->parent);
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, outputParaListNode);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 14: { // <N2> = COMMA ID COLON <type> <N2>
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, node->parent->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 15: { // <N2> = e
                    break;
                }
                case 16: { // <dataType> = INTEGER
                    ASTNode* intNode = createASTNode("INTEGER", node->parseTreeNode->child);
                    appendASTNodeAsChild(intNode, node->parent);
                    break;
                }
                case 17: { // <dataType> = REAL
                    ASTNode* realNode = createASTNode("REAL", node->parseTreeNode->child);
                    appendASTNodeAsChild(realNode, node->parent);
                    break;
                }
                case 18: { // <dataType> = BOOLEAN
                    ASTNode* boolNode = createASTNode("BOOLEAN", node->parseTreeNode->child);
                    appendASTNodeAsChild(boolNode, node->parent);
                    break;
                }
                case 19: { // <dataType> = ARRAY SQBO <range_arrays> SQBC OF <type>
                    ASTNode* arrayNode = createASTNode("ARRAY", node->parseTreeNode->child);
                    appendASTNodeAsChild(arrayNode, node->parent);
                    pushChildrenToASTStack(st, arrayNode, node->parseTreeNode->child);
                    break;
                }
                case 20: { // <range_arrays> = <index_arr> RANGEOP <index_arr>
                    ASTNode* rangeNode = createASTNode("RANGE_ARRAYS", node->parseTreeNode);
                    appendASTNodeAsChild(rangeNode, node->parent);
                    pushChildrenToASTStack(st, rangeNode, node->parseTreeNode->child);
                    break;
                }
                case 21: { // <index_arr> = <sign> <new_index>
                    ASTNode* indexNode = createASTNode("INDEX", node->parseTreeNode);
                    appendASTNodeAsChild(indexNode, node->parent);
                    pushChildrenToASTStack(st, indexNode, node->parseTreeNode->child);
                    break;
                }
                case 22: { // <new_index> = NUM
                    ASTNode* numNode = createASTNode("NUM", node->parseTreeNode->child);
                    appendASTNodeAsChild(numNode, node->parent);
                    break;
                }
                case 23: { // <new_index> = ID
                    ASTNode* idNode = createASTNode("ID", node->parseTreeNode->child);
                    appendASTNodeAsChild(idNode, node->parent);
                    break;
                }
                case 24: { // <sign> = PLUS
                    ASTNode* signNode = createASTNode("PLUS", node->parseTreeNode->child);
                    appendASTNodeAsChild(signNode, node->parent);
                    break;
                }
                case 25: { // <sign> = MINUS
                    ASTNode* signNode = createASTNode("MINUS", node->parseTreeNode->child);
                    appendASTNodeAsChild(signNode, node->parent);
                    break;
                }
                case 26: { // <sign> = e
                    break;
                }
                case 27: { // <type> = INTEGER
                    ASTNode* intNode = createASTNode("INTEGER", node->parseTreeNode->child);
                    appendASTNodeAsChild(intNode, node->parent);
                    break;
                }
                case 28: { // <type> = REAL
                    ASTNode* realNode = createASTNode("REAL", node->parseTreeNode->child);
                    appendASTNodeAsChild(realNode, node->parent);
                    break;
                }
                case 29: { // <type> = BOOLEAN
                    ASTNode* boolNode = createASTNode("BOOLEAN", node->parseTreeNode->child);
                    appendASTNodeAsChild(boolNode, node->parent);
                    break;
                }
                case 30: { // <moduleDef> = START <statements> END
                    ASTNode* moduleDefNode = createASTNode("MODULEDEF", node->parseTreeNode);
                    appendASTNodeAsChild(moduleDefNode, node->parent);
                    pushChildrenToASTStack(st, moduleDefNode, node->parseTreeNode->child);
                    break;
                }
                case 31: { // <statements> = <statement> <statements>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 32: { // <statements> = e 
                    break;
                }
                case 33: { // <statement> = <ioStmt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 34: { // <statement> = <simpleStmt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 35: { // <statement> = <declareStmt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 36: { // <statement> = <conditionalStmt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 37: { // <statement> = <iterativeStmt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 38: { // <ioStmt> = GET_VALUE BO ID BC SEMICOL
                    ASTNode* getValNode = createASTNode("GET_VALUE", node->parseTreeNode->child);
                    appendASTNodeAsChild(getValNode, node->parent);
                    break;
                }
                case 39: { // <ioStmt> = PRINT BO <var_print> BC SEMICOL
                    ASTNode* printNode = createASTNode("PRINT", node->parseTreeNode->child);
                    appendASTNodeAsChild(printNode, node->parent);
                    break;
                }
                case 40: { // <boolConstt> = TRUE
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 41: { // <boolConstt> = TRUE
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }       
                case 42: { // <id_num_rnum> = ID
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 43: { // <id_num_rnum> = NUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 44: { // <id_num_rnum> = RNUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 45: { // <var_print> = <boolConstt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 46: { // <var_print> = ID <P1>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 47: { // <var_print> = NUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                } 
                case 48: { // <var_print> = RNUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 49: { // <P1> = SQBO <index_arr> SQBC 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 50: { // <P1> = e 
                    break;
                }
                case 51: { // <simpleStmt> = <assignmentStmt> 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 52: { // <simpleStmt> = <moduleReuseStmt> 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 53: { // <assignmentStmt> = ID <whichStmt>
                    ASTNode* assignmentNode = createASTNode("ASSIGN_STMT", node->parseTreeNode);
                    pushChildrenToASTStack(st, assignmentNode, node->parseTreeNode->child);
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
    if (node->parseTreeNode != NULL && node->parseTreeNode->isLeaf) {
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

typedef struct QueueNode {
    ASTNode* node;
    struct QueueNode* next;
} QueueNode;
typedef struct Queue {
    QueueNode* front;
    QueueNode* tail;
    int sz;
} Queue;

Queue* initQueue(void) {
    Queue* q = malloc(sizeof(Queue));
    q->front = NULL;
    q->tail = NULL;
    q->sz = 0;

    return q;
}

QueueNode* peekQueue(Queue* q) {
    return q->front;
}

void popQueue(Queue* q) {
    QueueNode* currFront = q->front;
    q->front = currFront->next;
    q->sz--;

    free(currFront);
    return;
}

void appendQueue(Queue* q, ASTNode* node) {
    QueueNode* qNode = malloc(sizeof(QueueNode));

    qNode->node = node;
    qNode->next = NULL;
    q->sz++;
    if (q->front == NULL) {
        q->front = q->tail = qNode;
    } else {
        q->tail->next = qNode;
        q->tail = qNode;
    }

    return;
}

void prettyPrintAST(void) {
    Queue* q = initQueue();

    appendQueue(q, tree->root);

    while (q->sz > 0) {
        int sz = q->sz;
        printf("%d\n", sz);

        for (int i = 0; i < sz; ++i) {
            QueueNode* qNode = peekQueue(q);
            ASTNode* node = qNode->node;
            if (node->parseTreeNode != NULL && node->parseTreeNode->isLeaf) {
                printf("%s\t%s\t\t", node->label, node->parseTreeNode->tok->lexeme);
            } else {
                printf("%s\t\t", node->label);
            }
            popQueue(q);

            ASTNode* curr = node->leftMostChild;
            while (curr != NULL) {
                appendQueue(q, curr);
                curr = curr->next;
            }
        }

        printf("\n");
    }

    return;
}

void ASTCreator(ParseTree* parseTree) {
    pt = parseTree;
    createAST();
    // printAST(tree->root, false);
    prettyPrintAST();
    return;
}