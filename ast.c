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

/* ASTNodeList* initASTNodeList(void) {
    ASTNodeList* list = malloc(sizeof(ASTNodeList));
    list->head = NULL;
    list->tail = NULL;
    list->sz = 0;

    return list;
}
 */
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

    tree->size++; // New Node created -> It WILL be added to AST
    return newNode;
}
/* 
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
} */

void appendASTNodeAsChild(ASTNode* node, ASTNode* parent) { // Appends node to list of children of ASTNode parent with start,end of list pointed by leftMostChild, rightMostChildren
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

void popASTRightMostChild(ASTNode* node) {
    if (node->leftMostChild == NULL) {
        printf("No leftMostChild in %s\n.", node->label);
        exit(1);
    }

    node->rightMostChild = node->rightMostChild->prev;
    node->numChildren--;
    if (node->numChildren == 0) {
        node->leftMostChild = NULL;
    } else {
        node->rightMostChild->next = NULL;
    }
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
    ASTNode* caseStmtsNode;
    while (st->size > 0) {
        ASTStackNode* node = peekASTStack(st);
        popASTStack(st);

        if (node->parseTreeNode->isLeaf) {
            if (isUsefulTerminal(node)) {
                ASTNode* leafNode = createASTNode(token_types[node->parseTreeNode->tokenID], node->parseTreeNode);
                appendASTNodeAsChild(leafNode, node->parent);
            }
        } else {
            switch (node->parseTreeNode->productionID) {
                case 0: { // <program> = <moduleDeclarations> <otherModules> <driverModule> <otherModules> 
                    ASTNode* programNode = createASTNode("PROGRAM", node->parseTreeNode);
                    tree->root = programNode;
                    pushChildrenToASTStack(st, programNode, node->parseTreeNode->child);
                    moduleDecNode = createASTNode("MODULE_DEC", node->parseTreeNode->child);
                    appendASTNodeAsChild(moduleDecNode, programNode);
                    break;
                }
                case 1: { // <moduleDeclarations> = <moduleDeclaration> <moduleDeclarations>
                    pushChildrenToASTStack(st, moduleDecNode, node->parseTreeNode->child);
                    break;
                }
                case 2: { // <moduleDeclarations> = e
                    break;
                }
                case 3: { // <moduleDeclaration> = DECLARE MODULE ID SEMICOL
                    pushChildrenToASTStack(st, moduleDecNode, node->parseTreeNode->child);
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
                    ASTNode* driverNode = createASTNode("DRIVER", node->parseTreeNode->child->next);
                    appendASTNodeAsChild(driverNode, node->parent);
                    pushChildrenToASTStack(st, driverNode, node->parseTreeNode->child);
                    break;
                }
                case 7: { // <module> = DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
                    ASTNode* moduleNode = createASTNode("MODULE", node->parseTreeNode);
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
                    ASTNode* inputParaListNode = createASTNode("INPUT_LIST", node->parseTreeNode);
                    appendASTNodeAsChild(inputParaListNode, node->parent);
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, inputParaListNode);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 11: { // <N1> = COMMA ID COLON <dataType> <N1>
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, node->parent->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, node->parseTreeNode->child);
                    break;
                }
                case 12: { // <N1> = e
                    break;
                }
                case 13: { // <output_plist> = ID COLON <type> <N2>
                    ASTNode* outputParaListNode = createASTNode("OUTPUT_LIST", node->parseTreeNode);
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
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 17: { // <dataType> = REAL
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 18: { // <dataType> = BOOLEAN
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
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
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 23: { // <new_index> = ID
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 24: { // <sign> = PLUS
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 25: { // <sign> = MINUS
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 26: { // <sign> = e
                    break;
                }
                case 27: { // <type> = INTEGER
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 28: { // <type> = REAL
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 29: { // <type> = BOOLEAN
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 30: { // <moduleDef> = START <statements> END
                    ASTNode* moduleDefNode = createASTNode("MODULE_DEF", node->parseTreeNode);
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
                    pushChildrenToASTStack(st, getValNode, node->parseTreeNode->child);
                    break;
                }
                case 39: { // <ioStmt> = PRINT BO <var_print> BC SEMICOL
                    ASTNode* printNode = createASTNode("PRINT", node->parseTreeNode->child);
                    appendASTNodeAsChild(printNode, node->parent);
                    pushChildrenToASTStack(st, printNode, node->parseTreeNode->child->next);
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
                    appendASTNodeAsChild(assignmentNode, node->parent);
                    pushChildrenToASTStack(st, assignmentNode, node->parseTreeNode->child);
                    break;
                }
                case 54: { // <whichStmt> = <lvalueIDStmt> 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 55: { // <whichStmt> = <lvalueARRStmt> 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 56: { // <lvalueIDStmt> = ASSIGNOP <expression> SEMICOL 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 57: { // <lvalueARRStmt> = SQBO <element_index_with_expressions> SQBC ASSIGNOP <expression> SEMICOL 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 58: { // <moduleReuseStmt> = <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL 
                    ASTNode* moduleReuseStmtNode = createASTNode("MODULE_REUSE_STMT", node->parseTreeNode);
                    appendASTNodeAsChild(moduleReuseStmtNode, node->parent);
                    pushChildrenToASTStack(st, moduleReuseStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 59: { // <actual_para_list> = MINUS <N_13>
                    // Only create node when the parent is "MODULE_REUSE_STATEMENT"
                    if (node->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", node->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, node->parent);
                        ASTNode* minusNode = createASTNode("MINUS_NODE", node->parseTreeNode->child);
                        appendASTNodeAsChild(minusNode, paraListNode);
                        pushChildrenToASTStack(st, minusNode, node->parseTreeNode->child->next);
                    } else {
                        ASTNode* minusNode = createASTNode("MINUS_NODE", node->parseTreeNode->child);
                        appendASTNodeAsChild(minusNode, node->parent);
                        pushChildrenToASTStack(st, minusNode, node->parseTreeNode->child->next);
                    }
                    break;
                }
                case 60: { // <N_13> = NUM <N_12>
                    ASTNode* numNode = createASTNode("NUM", node->parseTreeNode->child);
                    appendASTNodeAsChild(numNode, node->parent);
                    pushChildrenToASTStack(st, node->parent->parent, node->parseTreeNode->child);
                    break;
                }
                case 61: { // <N_13> = RNUM <N_12>
                    ASTNode* rnumNode = createASTNode("RNUM", node->parseTreeNode->child);
                    appendASTNodeAsChild(rnumNode, node->parent);
                    pushChildrenToASTStack(st, node->parent->parent, node->parseTreeNode->child);
                    break;
                }
                case 62: { // <N_13> = ID <N_11> <N_12>
                    ASTNode* idNode = createASTNode("ID", node->parseTreeNode->child);
                    appendASTNodeAsChild(idNode, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, node->parent);
                    pushChildrenToASTStack(st, node->parent->parent, node->parseTreeNode->child->next->next);
                    break;
                }
                case 63: { // <actual_para_list> = NUM <N_12>
                    if (node->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", node->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, node->parent);
                        pushChildrenToASTStack(st, paraListNode, node->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    }
                    break;
                }
                case 64: { // <actual_para_list> = RNUM <N_12>
                    if (node->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", node->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, node->parent);
                        pushChildrenToASTStack(st, paraListNode, node->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    }
                    break;
                }
                case 65: { // <actual_para_list> = <boolConstt> <N_12>
                    if (node->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", node->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, node->parent);
                        pushChildrenToASTStack(st, paraListNode, node->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    }
                    break;
                }
                case 66: { // <actual_para_list> = ID <N_11> <N_12>
                    if (node->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", node->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, node->parent);
                        pushChildrenToASTStack(st, paraListNode, node->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    }
                    break;
                }
                case 67: { // <N_12> = COMMA <actual_para_list>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 68: { // <N_12> = e
                    break;
                }
                case 69: { // <optional> = SQBO <idList> SQBC ASSIGNOP
                    pushASTStack(st, node->parseTreeNode->child->next, node->parent);
                    break;
                }
                case 70: { // <optional> = e 
                    break;
                }
                case 71: { // <idList> = ID <N3>
                    ASTNode* idListNode = createASTNode("ASSIGN_LIST", node->parseTreeNode);
                    appendASTNodeAsChild(idListNode, node->parent);
                    pushChildrenToASTStack(st, idListNode, node->parseTreeNode->child);
                    break;
                }
                case 72: { // <N3> = COMMA ID <N3>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 73: { // <N3> = e 
                    break;
                }
                case 74: { // <expression> = <arithmeticOrBooleanExpr> 
                    ASTNode* expressionNode = createASTNode("EXPR", node->parseTreeNode);
                    appendASTNodeAsChild(expressionNode, node->parent);
                    pushChildrenToASTStack(st, expressionNode, node->parseTreeNode->child);
                    break;
                }
                case 75: { // <expression> = <U>
                    ASTNode* expressionNode = createASTNode("EXPR", node->parseTreeNode);
                    appendASTNodeAsChild(expressionNode, node->parent);
                    pushChildrenToASTStack(st, expressionNode, node->parseTreeNode->child);
                    break;
                }
                case 76: { // <U> = <unary_op> <new_NT>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 77: { // <new_NT> = BO <arithmeticExpr> BC 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 78: { // <new_NT> = <id_num_rnum>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 79: { // <unary_op> = PLUS
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 80: { // <unary_op> = MINUS 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 81: { // <arithmeticOrBooleanExpr> = <AnyTerm> <N7>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 82: { // <N7> = <logicalOp> <AnyTerm> <N7>
                    ParseTreeNode* logicalPTNode = node->parseTreeNode->child->child;
                    ASTNode* logicalNode = createASTNode(token_types[logicalPTNode->tokenID], logicalPTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, logicalNode);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(logicalNode, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, logicalNode);
                    pushASTStack(st, node->parseTreeNode->child->next->next, node->parent);
                    break;
                }
                case 83: { // <N7> = e
                    break;
                }
                case 84: { // <AnyTerm> = <arithmeticExpr> <N8>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 85: { // <AnyTerm> = <boolConstt>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 86: { // <N8> = <relationalOp> <arithmeticExpr>
                    ParseTreeNode* relationalPTNode = node->parseTreeNode->child->child;
                    ASTNode* relationalNode = createASTNode(token_types[relationalPTNode->tokenID], relationalPTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, relationalNode);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(relationalNode, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, relationalNode);
                    break;
                }
                case 87: { // <N8> = e
                    break;
                }
                case 88: { // <arithmeticExpr> = <term> <N4>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 89: { // <N4> = <op1> <term> <N4>
                    ParseTreeNode* op1PTNode = node->parseTreeNode->child->child;
                    ASTNode* op1Node = createASTNode(token_types[op1PTNode->tokenID], op1PTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, op1Node);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(op1Node, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, op1Node);
                    pushASTStack(st, node->parseTreeNode->child->next->next, node->parent);
                    break;
                }
                case 90: { // <N4> = e 
                    break;
                }
                case 91: { // <term> = <factor> <N5>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 92: { // <N5> = <op2> <factor> <N5> 
                    ParseTreeNode* op2PTNode = node->parseTreeNode->child->child;
                    ASTNode* op2Node = createASTNode(token_types[op2PTNode->tokenID], op2PTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, op2Node);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(op2Node, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, op2Node);
                    pushASTStack(st, node->parseTreeNode->child->next->next, node->parent);
                    break;
                }
                case 93: { // <N5> = e 
                    break;
                }
                case 94: { //<factor> = BO <arithmeticOrBooleanExpr> BC 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 95: { // <factor> = NUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 96: { // <factor> = RNUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 97 : { // <factor> = ID <N_11>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 98: { // <N_11> = SQBO <element_index_with_expressions> SQBC
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 99: { // <N_11> = e 
                    break;
                }
                case 100: { // <arrExpr> = <arrTerm> <arr_N4>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 101: { // <arr_N4> = <op1> <arrTerm> <arr_N4> 
                    ParseTreeNode* op1PTNode = node->parseTreeNode->child->child;
                    ASTNode* op1Node = createASTNode(token_types[op1PTNode->tokenID], op1PTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, op1Node);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(op1Node, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, op1Node);
                    pushASTStack(st, node->parseTreeNode->child->next->next, node->parent);
                    break;
                }
                case 102: { // <arr_N4> = e 
                    break;
                }
                case 103: { // <arrTerm> = <arrFactor> <arr_N5>
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 104: { // <arr_N5> = <op2> <arrFactor> <arr_N5>
                    ParseTreeNode* op2PTNode = node->parseTreeNode->child->child;
                    ASTNode* op2Node = createASTNode(token_types[op2PTNode->tokenID], op2PTNode);
                    appendASTNodeAsChild(node->parent->rightMostChild, op2Node);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(op2Node, node->parent);
                    pushASTStack(st, node->parseTreeNode->child->next, op2Node);
                    pushASTStack(st, node->parseTreeNode->child->next->next, node->parent);
                    break;
                }
                case 105: { // <arr_N5> = e 
                    break;
                }
                case 106: { // <arrFactor> = ID
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 107: { // <arrFactor> = NUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 108: { // <arrFactor> = <boolConstt> 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 109: { // <arrFactor> = BO <arrExpr> BC 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 110: { // <element_index_with_expressions> = <sign> <arrExpr>
                    ASTNode* arrNode = createASTNode("ARRAY", NULL);
                    appendASTNodeAsChild(node->parent->rightMostChild, arrNode);
                    popASTRightMostChild(node->parent);
                    appendASTNodeAsChild(arrNode, node->parent);
                    ASTNode* elementIndex = createASTNode("ARRAY_INDEX", node->parseTreeNode);
                    appendASTNodeAsChild(elementIndex, arrNode);
                    pushChildrenToASTStack(st, elementIndex, node->parseTreeNode->child);
                    break;
                }
                case 111: { // <op1> = PLUS 
                    // Already handled wherever it expands
                    break;
                }
                case 112: { // <op1> = MINUS
                    // Already handled wherever it expands
                    break;
                }
                case 113: { // <op2> = MUL 
                    // Already handled wherever it expands
                    break;
                }
                case 114: { // <op2> = DIV 
                    // Already handled wherever it expands
                    break;
                }
                case 115: { // <logicalOp> = AND 
                    // Already handled wherever it expands
                    break;
                }
                case 116: { // <logicalOp> = OR 
                    // Already handled wherever it expands
                    break;
                }
                case 117: { // <relationalOp> = LT
                    // Already handled wherever it expands
                    break;
                }
                case 118: { // <relationalOp> = LE
                    // Already handled wherever it expands
                    break;
                }
                case 119: { // <relationalOp> = GT
                    // Already handled wherever it expands
                    break;
                }
                case 120: { // <relationalOp> = GE
                    // Already handled wherever it expands
                    break;
                }
                case 121: { // <relationalOp> = EQ
                    // Already handled wherever it expands
                    break;
                }
                case 122: { // <relationalOp> = NE
                    // Already handled wherever it expands
                    break;
                }
                case 123: { // <declareStmt> = DECLARE <idList> COLON <dataType> SEMICOL
                    ASTNode* declareStmtNode = createASTNode("DECLARE", node->parseTreeNode->child);
                    appendASTNodeAsChild(declareStmtNode, node->parent);
                    pushChildrenToASTStack(st, declareStmtNode, node->parseTreeNode->child->next); // We donot need to insert DECLARE
                    break;
                }
                case 124: { // <conditionalStmt> = SWITCH BO ID BC START <caseStmts> <default> END
                    ASTNode* condStmtNode = createASTNode("SWITCH", node->parseTreeNode->child);
                    appendASTNodeAsChild(condStmtNode, node->parent);
                    pushChildrenToASTStack(st, condStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 125: { // <caseStmts> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", node->parseTreeNode->child);
                    appendASTNodeAsChild(caseStmtNode, node->parent);
                    pushChildrenToASTStack(st, caseStmtNode, node->parseTreeNode->child);
                    break;
                }
                case 126: { // <N9> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", node->parseTreeNode->child);
                    appendASTNodeAsChild(caseStmtNode, node->parent->parent);
                    pushChildrenToASTStack(st, caseStmtNode, node->parseTreeNode->child->next); // We donot need to insert CASE
                    break;
                }
                case 127: { // <N9> = e
                    break;
                }
                case 128: { // <value> = NUM
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 129: { // <value> = TRUE
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 130: { // <value> = FALSE 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 131: { // <default> = DEFAULT COLON <statements> BREAK SEMICOL 
                    ASTNode* defaultNode = createASTNode("DEFAULT", node->parseTreeNode->child);
                    appendASTNodeAsChild(defaultNode, node->parent);
                    pushChildrenToASTStack(st, defaultNode, node->parseTreeNode->child);
                    break; 
                }
                case 132: { // <default> = e 
                    break;
                }
                case 133: { // <iterativeStmt> = FOR BO ID IN <range_for_loop> BC START <statements> END 
                    ASTNode* iterStmt = createASTNode("FOR", node->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, node->parent);
                    pushChildrenToASTStack(st, iterStmt, node->parseTreeNode->child);
                    break;
                }
                case 134: { // <iterativeStmt> = WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END 
                    ASTNode* iterStmt = createASTNode("WHILE", node->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, node->parent);
                    pushChildrenToASTStack(st, iterStmt, node->parseTreeNode->child);
                    break;
                }
                case 135: { // <range_for_loop> = <index_for_loop> RANGEOP <index_for_loop>
                    ASTNode* rangeForLoop = createASTNode("RANGE_FOR_LOOP", node->parseTreeNode);
                    appendASTNodeAsChild(rangeForLoop, node->parent);
                    pushChildrenToASTStack(st, rangeForLoop, node->parseTreeNode->child);
                    break;
                }
                case 136: { // <index_for_loop> = <sign_for_loop> <new_index_for_loop> 
                    ASTNode* indexForLoop = createASTNode("INDEX_FOR_LOOP", node->parseTreeNode);
                    appendASTNodeAsChild(indexForLoop, node->parent);
                    pushChildrenToASTStack(st, indexForLoop, node->parseTreeNode->child);
                    break;
                }
                case 137: { //<new_index_for_loop> = NUM 
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 138: { // <sign_for_loop> = PLUS
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 139: { // <sign_for_loop> = MINUS
                    pushChildrenToASTStack(st, node->parent, node->parseTreeNode->child);
                    break;
                }
                case 140: { // <sign_for_loop> = e 
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
                printf("(%s %s %s)\n", node->parent->label, node->label, node->parseTreeNode->tok->lexeme);
            } else {
                printf("(%s %s)\n", node->parent->label, node->label);
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

    printf("Number of nodes in AST: %d\n", tree->size);

    return;
}

void ASTCreator(ParseTree* parseTree) {
    pt = parseTree;
    createAST();
    // printAST(tree->root, false);
    prettyPrintAST();
    printf("Number of nodes in Parse Tree: %d\n", pt->sz);
    printf("Compression Ratio: %.2lf%%\n", (((double) tree->size) / pt->sz) * 100);
    return;
}