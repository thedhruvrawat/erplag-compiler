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

AST* tree = NULL;
ASTStack* st;
ParseTree* pt;

/**
 * @brief Initialises and returns an AST 
 * 
 * @return AST* 
 */
AST* initAST(void) {
    AST* tree = malloc(sizeof(AST));
    tree->root = NULL;
    tree->size = 0; 

    return tree;
}

/**
 * @brief Creates an AST Node given the corresponding label and parsetree node
 * 
 * @param label 
 * @param node 
 * @return ASTNode* 
 */
ASTNode* createASTNode(const char* label, ParseTreeNode* node) {
    ASTNode* newNode = malloc(sizeof(ASTNode));
    strcpy(newNode->label, label);
    newNode->parent = NULL;
    if (node != NULL) {
        newNode->isLeaf = node->isLeaf;
        if (node->isLeaf) {
            newNode->leaf = node->leaf;
        }
    } else {
        newNode->isLeaf = false;
        newNode->leaf.tok = NULL;
    }
    newNode->leftMostChild = NULL;
    newNode->rightMostChild = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->numChildren = 0;

    tree->size++; // New Node created -> It WILL be added to AST
    return newNode;
}

/**
 * @brief Appends the node to the list of children of the other node
 * 
 * @param node 
 * @param parent 
 */
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

/**
 * @brief Pops the rightMostChild of the given node
 * 
 * @param node 
 */
void popASTRightMostChild(ASTNode* node) {
    if (node->leftMostChild == NULL) {
        printf("No child in %s\n.", node->label);
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

/**
 * @brief Creates and initialises an AST Stack Node 
 * 
 * @param parseTreeNode 
 * @param par 
 * @return ASTStackNode* 
 */
ASTStackNode* createASTStackNode(ParseTreeNode* parseTreeNode, ASTNode* par) {
    ASTStackNode* newNode = malloc(sizeof(ASTStackNode));
    newNode->parseTreeNode = parseTreeNode;
    newNode->parent = par;
    newNode->next = NULL;

    return newNode;
}

/**
 * @brief Initialises a stack used in AST creation
 * 
 * @return ASTStack* 
 */
ASTStack* initASTStack(void) {
    ASTStack* st = malloc(sizeof(ASTStack));
    st->top = NULL;
    st->size = 0;

    return st;
}

/**
 * @brief Peek the top element of the stack used in AST creation
 * 
 * @param st 
 * @return ASTStackNode* 
 */
ASTStackNode* peekASTStack(ASTStack* st) {
    return st->top;
}

/**
 * @brief Pop the stack used in AST creation
 * 
 * @param st 
 */
void popASTStack(ASTStack* st) {
    if (st->size == 0) {
        printf("AST Stack is empty!\n");
        return;
    }

    ASTStackNode* currTop = st->top;

    st->top = st->top->next;
    st->size--;

    return;
}

/**
 * @brief Push a parsetree node with its corresponding AST parent to the stack used in AST creation
 * 
 * @param st 
 * @param parseTreeNode 
 * @param par 
 */
void pushASTStack(ASTStack* st, ParseTreeNode* parseTreeNode, ASTNode* par) {
    ASTStackNode* currTop = st->top;
    ASTStackNode* newNode = createASTStackNode(parseTreeNode, par);

    newNode->next = currTop;
    st->top = newNode;
    st->size++;

    return;
}

/**
 * @brief Pushes a linked list of parsetree nodes with its corresponding AST parent to the stack
 * used in AST creation recursively
 * 
 * @param st 
 * @param par 
 * @param curr 
 */
void pushChildrenToASTStack(ASTStack* st, ASTNode* par, ParseTreeNode* curr) { // Inserts "curr" parseTreeNode into ASTstack "st" along with its siblings with "par" ASTNode as parent
    if (curr == NULL) {
        return;
    }

    pushChildrenToASTStack(st, par, curr->next);
    pushASTStack(st, curr, par);

    return;
}

/**
 * @brief Check whether the terminal should be preserved and used in the AST
 * 
 * @param node 
 * @return true 
 * @return false 
 */
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

/**
 * @brief Generates the AST
 * 
 */
void createAST(void) {
    // Initializing the root node of the AST
    tree = initAST();

    // Initializing the stack
    st = initASTStack();
    pushASTStack(st, pt->root, NULL);

    ASTNode* moduleDecNode;
    ASTNode* caseStmtsNode;
    ASTStackNode* stackNode = NULL;
    while (st->size > 0) {
        if (stackNode != NULL) {
            free(stackNode);
        }
        stackNode = peekASTStack(st);
        popASTStack(st);

        if (stackNode->parseTreeNode->isLeaf) {
            if (isUsefulTerminal(stackNode)) {
                ASTNode* leafNode = createASTNode(token_types[stackNode->parseTreeNode->tokenID], stackNode->parseTreeNode);
                appendASTNodeAsChild(leafNode, stackNode->parent);
            } else if (stackNode->parseTreeNode->tokenID == START) {
                stackNode->parent->scope.scopeStart = stackNode->parseTreeNode->leaf.tok->linenum;
            } else if (stackNode->parseTreeNode->tokenID == END) {
                stackNode->parent->scope.scopeEnd = stackNode->parseTreeNode->leaf.tok->linenum;
            }
        } else {
            switch (stackNode->parseTreeNode->productionID) {
                case 0: { // <program> = <moduleDeclarations> <otherModules> <driverModule> <otherModules> 
                    ASTNode* programNode = createASTNode("PROGRAM", stackNode->parseTreeNode);
                    tree->root = programNode;
                    pushChildrenToASTStack(st, programNode, stackNode->parseTreeNode->child);
                    moduleDecNode = createASTNode("MODULE_DEC", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(moduleDecNode, programNode);
                    break;
                }
                case 1: { // <moduleDeclarations> = <moduleDeclaration> <moduleDeclarations>
                    pushChildrenToASTStack(st, moduleDecNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 2: { // <moduleDeclarations> = e
                    break;
                }
                case 3: { // <moduleDeclaration> = DECLARE MODULE ID SEMICOL
                    pushChildrenToASTStack(st, moduleDecNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 4: { // <otherModules> = <module>  <otherModules>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 5: { // <otherModules> = e
                    break;
                }
                case 6: { // <driverModule> = DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
                    ASTNode* driverNode = createASTNode("DRIVER", stackNode->parseTreeNode->child->next);
                    appendASTNodeAsChild(driverNode, stackNode->parent);
                    pushChildrenToASTStack(st, driverNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 7: { // <module> = DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
                    ASTNode* moduleNode = createASTNode("MODULE", stackNode->parseTreeNode->child->next);
                    appendASTNodeAsChild(moduleNode, stackNode->parent);
                    pushChildrenToASTStack(st, moduleNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 8: { // <ret> = RETURNS SQBO <output_plist> SQBC SEMICOL
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 9: { // <ret> = e 
                    // Add an empty output list to keep things consistent
                    ASTNode* outputParaListNode = createASTNode("OUTPUT_LIST", stackNode->parseTreeNode);
                    appendASTNodeAsChild(outputParaListNode, stackNode->parent);
                    break;
                }
                case 10: { // <input_plist> = ID COLON <dataType> <N1>
                    ASTNode* inputParaListNode = createASTNode("INPUT_LIST", stackNode->parseTreeNode);
                    appendASTNodeAsChild(inputParaListNode, stackNode->parent);
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, inputParaListNode);
                    pushChildrenToASTStack(st, idDataTypeNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 11: { // <N1> = COMMA ID COLON <dataType> <N1>
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, stackNode->parent->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 12: { // <N1> = e
                    break;
                }
                case 13: { // <output_plist> = ID COLON <type> <N2>
                    ASTNode* outputParaListNode = createASTNode("OUTPUT_LIST", stackNode->parseTreeNode);
                    appendASTNodeAsChild(outputParaListNode, stackNode->parent);
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, outputParaListNode);
                    pushChildrenToASTStack(st, idDataTypeNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 14: { // <N2> = COMMA ID COLON <type> <N2>
                    ASTNode* idDataTypeNode = createASTNode("ID_DATATYPE", NULL);
                    appendASTNodeAsChild(idDataTypeNode, stackNode->parent->parent);
                    pushChildrenToASTStack(st, idDataTypeNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 15: { // <N2> = e
                    break;
                }
                case 16: { // <dataType> = INTEGER
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 17: { // <dataType> = REAL
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 18: { // <dataType> = BOOLEAN
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 19: { // <dataType> = ARRAY SQBO <range_arrays> SQBC OF <type>
                    ASTNode* arrayNode = createASTNode("ARRAY", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(arrayNode, stackNode->parent);
                    pushChildrenToASTStack(st, arrayNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 20: { // <range_arrays> = <index_arr> RANGEOP <index_arr>
                    ASTNode* rangeNode = createASTNode("RANGE_ARRAYS", stackNode->parseTreeNode);
                    appendASTNodeAsChild(rangeNode, stackNode->parent);
                    pushChildrenToASTStack(st, rangeNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 21: { // <index_arr> = <sign> <new_index>
                    ASTNode* indexNode = createASTNode("ARRAY_INDEX", stackNode->parseTreeNode);
                    appendASTNodeAsChild(indexNode, stackNode->parent);
                    pushChildrenToASTStack(st, indexNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 22: { // <new_index> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 23: { // <new_index> = ID
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 24: { // <sign> = PLUS
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 25: { // <sign> = MINUS
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 26: { // <sign> = e
                    break;
                }
                case 27: { // <type> = INTEGER
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 28: { // <type> = REAL
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 29: { // <type> = BOOLEAN
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 30: { // <moduleDef> = START <statements> END
                    ASTNode* moduleDefNode = createASTNode("MODULE_DEF", stackNode->parseTreeNode);
                    appendASTNodeAsChild(moduleDefNode, stackNode->parent);
                    pushChildrenToASTStack(st, moduleDefNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 31: { // <statements> = <statement> <statements>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 32: { // <statements> = e 
                    break;
                }
                case 33: { // <statement> = <ioStmt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 34: { // <statement> = <simpleStmt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 35: { // <statement> = <declareStmt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 36: { // <statement> = <conditionalStmt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 37: { // <statement> = <iterativeStmt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 38: { // <ioStmt> = GET_VALUE BO ID BC SEMICOL
                    ASTNode* getValNode = createASTNode("GET_VALUE", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(getValNode, stackNode->parent);
                    pushChildrenToASTStack(st, getValNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 39: { // <ioStmt> = PRINT BO <var_print> BC SEMICOL
                    ASTNode* printNode = createASTNode("PRINT", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(printNode, stackNode->parent);
                    pushChildrenToASTStack(st, printNode, stackNode->parseTreeNode->child->next);
                    break;
                }
                case 40: { // <boolConstt> = TRUE
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 41: { // <boolConstt> = TRUE
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }       
                case 42: { // <id_num_rnum> = ID
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 43: { // <id_num_rnum> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 44: { // <id_num_rnum> = RNUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 45: { // <var_print> = <boolConstt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 46: { // <var_print> = ID <P1>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 47: { // <var_print> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                } 
                case 48: { // <var_print> = RNUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 49: { // <P1> = SQBO <index_arr> SQBC 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 50: { // <P1> = e 
                    break;
                }
                case 51: { // <simpleStmt> = <assignmentStmt> 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 52: { // <simpleStmt> = <moduleReuseStmt> 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 53: { // <assignmentStmt> = ID <whichStmt>
                    ASTNode* assignmentNode = createASTNode("ASSIGN_STMT", stackNode->parseTreeNode);
                    appendASTNodeAsChild(assignmentNode, stackNode->parent);
                    pushChildrenToASTStack(st, assignmentNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 54: { // <whichStmt> = <lvalueIDStmt> 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 55: { // <whichStmt> = <lvalueARRStmt> 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 56: { // <lvalueIDStmt> = ASSIGNOP <expression> SEMICOL 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 57: { // <lvalueARRStmt> = SQBO <element_index_with_expressions> SQBC ASSIGNOP <expression> SEMICOL 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 58: { // <moduleReuseStmt> = <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL 
                    ASTNode* moduleReuseStmtNode = createASTNode("MODULE_REUSE_STMT", stackNode->parseTreeNode);
                    appendASTNodeAsChild(moduleReuseStmtNode, stackNode->parent);
                    pushChildrenToASTStack(st, moduleReuseStmtNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 59: { // <actual_para_list> = MINUS <N_13>
                    // Only create node when the parent is "MODULE_REUSE_STATEMENT"
                    if (stackNode->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", stackNode->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, stackNode->parent);
                        ASTNode* minusNode = createASTNode("MINUS_NODE", stackNode->parseTreeNode->child);
                        appendASTNodeAsChild(minusNode, paraListNode);
                        pushChildrenToASTStack(st, minusNode, stackNode->parseTreeNode->child->next);
                    } else {
                        ASTNode* minusNode = createASTNode("MINUS_NODE", stackNode->parseTreeNode->child);
                        appendASTNodeAsChild(minusNode, stackNode->parent);
                        pushChildrenToASTStack(st, minusNode, stackNode->parseTreeNode->child->next);
                    }
                    break;
                }
                case 60: { // <N_13> = NUM <N_12>
                    ASTNode* numNode = createASTNode("NUM", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(numNode, stackNode->parent);
                    pushChildrenToASTStack(st, stackNode->parent->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 61: { // <N_13> = RNUM <N_12>
                    ASTNode* rnumNode = createASTNode("RNUM", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(rnumNode, stackNode->parent);
                    pushChildrenToASTStack(st, stackNode->parent->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 62: { // <N_13> = ID <N_11> <N_12>
                    ASTNode* idNode = createASTNode("ID", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(idNode, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, stackNode->parent);
                    pushChildrenToASTStack(st, stackNode->parent->parent, stackNode->parseTreeNode->child->next->next);
                    break;
                }
                case 63: { // <actual_para_list> = NUM <N_12>
                    if (stackNode->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", stackNode->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, stackNode->parent);
                        pushChildrenToASTStack(st, paraListNode, stackNode->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    }
                    break;
                }
                case 64: { // <actual_para_list> = RNUM <N_12>
                    if (stackNode->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", stackNode->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, stackNode->parent);
                        pushChildrenToASTStack(st, paraListNode, stackNode->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    }
                    break;
                }
                case 65: { // <actual_para_list> = <boolConstt> <N_12>
                    if (stackNode->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", stackNode->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, stackNode->parent);
                        pushChildrenToASTStack(st, paraListNode, stackNode->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    }
                    break;
                }
                case 66: { // <actual_para_list> = ID <N_11> <N_12>
                    if (stackNode->parent->label[0] == 'M') {
                        ASTNode* paraListNode = createASTNode("PARA_LIST", stackNode->parseTreeNode);
                        appendASTNodeAsChild(paraListNode, stackNode->parent);
                        pushChildrenToASTStack(st, paraListNode, stackNode->parseTreeNode->child);
                    } else {
                        pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    }
                    break;
                }
                case 67: { // <N_12> = COMMA <actual_para_list>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 68: { // <N_12> = e
                    break;
                }
                case 69: { // <optional> = SQBO <idList> SQBC ASSIGNOP
                    pushASTStack(st, stackNode->parseTreeNode->child->next, stackNode->parent);
                    break;
                }
                case 70: { // <optional> = e 
                    // To keep things consistent, create a dummy node
                    ASTNode* idListNode = createASTNode("ASSIGN_LIST", stackNode->parseTreeNode);
                    appendASTNodeAsChild(idListNode, stackNode->parent);
                    break;
                }
                case 71: { // <idList> = ID <N3>
                    ASTNode* idListNode = createASTNode("ASSIGN_LIST", stackNode->parseTreeNode);
                    appendASTNodeAsChild(idListNode, stackNode->parent);
                    pushChildrenToASTStack(st, idListNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 72: { // <N3> = COMMA ID <N3>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 73: { // <N3> = e 
                    break;
                }
                case 74: { // <expression> = <arithmeticOrBooleanExpr> 
                    ASTNode* expressionNode = createASTNode("EXPR", stackNode->parseTreeNode);
                    appendASTNodeAsChild(expressionNode, stackNode->parent);
                    pushChildrenToASTStack(st, expressionNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 75: { // <expression> = <U>
                    ASTNode* expressionNode = createASTNode("EXPR", stackNode->parseTreeNode);
                    appendASTNodeAsChild(expressionNode, stackNode->parent);
                    pushChildrenToASTStack(st, expressionNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 76: { // <U> = <unary_op> <new_NT>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 77: { // <new_NT> = BO <arithmeticExpr> BC 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 78: { // <new_NT> = <id_num_rnum>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 79: { // <unary_op> = PLUS
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 80: { // <unary_op> = MINUS 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 81: { // <arithmeticOrBooleanExpr> = <AnyTerm> <N7>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 82: { // <N7> = <logicalOp> <AnyTerm> <N7>
                    ParseTreeNode* logicalPTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* logicalNode = createASTNode(token_types[logicalPTNode->tokenID], logicalPTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, logicalNode);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(logicalNode, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, logicalNode);
                    pushASTStack(st, stackNode->parseTreeNode->child->next->next, stackNode->parent);
                    break;
                }
                case 83: { // <N7> = e
                    break;
                }
                case 84: { // <AnyTerm> = <arithmeticExpr> <N8>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 85: { // <AnyTerm> = <boolConstt>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 86: { // <N8> = <relationalOp> <arithmeticExpr>
                    ParseTreeNode* relationalPTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* relationalNode = createASTNode(token_types[relationalPTNode->tokenID], relationalPTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, relationalNode);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(relationalNode, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, relationalNode);
                    break;
                }
                case 87: { // <N8> = e
                    break;
                }
                case 88: { // <arithmeticExpr> = <term> <N4>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 89: { // <N4> = <op1> <term> <N4>
                    ParseTreeNode* op1PTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* op1Node = createASTNode(token_types[op1PTNode->tokenID], op1PTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, op1Node);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(op1Node, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, op1Node);
                    pushASTStack(st, stackNode->parseTreeNode->child->next->next, stackNode->parent);
                    break;
                }
                case 90: { // <N4> = e 
                    break;
                }
                case 91: { // <term> = <factor> <N5>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 92: { // <N5> = <op2> <factor> <N5> 
                    ParseTreeNode* op2PTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* op2Node = createASTNode(token_types[op2PTNode->tokenID], op2PTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, op2Node);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(op2Node, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, op2Node);
                    pushASTStack(st, stackNode->parseTreeNode->child->next->next, stackNode->parent);
                    break;
                }
                case 93: { // <N5> = e 
                    break;
                }
                case 94: { //<factor> = BO <arithmeticOrBooleanExpr> BC 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 95: { // <factor> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 96: { // <factor> = RNUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 97 : { // <factor> = ID <N_11>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 98: { // <N_11> = SQBO <element_index_with_expressions> SQBC
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 99: { // <N_11> = e 
                    break;
                }
                case 100: { // <arrExpr> = <arrTerm> <arr_N4>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 101: { // <arr_N4> = <op1> <arrTerm> <arr_N4> 
                    ParseTreeNode* op1PTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* op1Node = createASTNode(token_types[op1PTNode->tokenID], op1PTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, op1Node);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(op1Node, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, op1Node);
                    pushASTStack(st, stackNode->parseTreeNode->child->next->next, stackNode->parent);
                    break;
                }
                case 102: { // <arr_N4> = e 
                    break;
                }
                case 103: { // <arrTerm> = <arrFactor> <arr_N5>
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 104: { // <arr_N5> = <op2> <arrFactor> <arr_N5>
                    ParseTreeNode* op2PTNode = stackNode->parseTreeNode->child->child;
                    ASTNode* op2Node = createASTNode(token_types[op2PTNode->tokenID], op2PTNode);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, op2Node);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(op2Node, stackNode->parent);
                    pushASTStack(st, stackNode->parseTreeNode->child->next, op2Node);
                    pushASTStack(st, stackNode->parseTreeNode->child->next->next, stackNode->parent);
                    break;
                }
                case 105: { // <arr_N5> = e 
                    break;
                }
                case 106: { // <arrFactor> = ID
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 107: { // <arrFactor> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 108: { // <arrFactor> = <boolConstt> 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 109: { // <arrFactor> = BO <arrExpr> BC 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 110: { // <element_index_with_expressions> = <sign> <arrExpr>
                    ASTNode* arrNode = createASTNode("ARRAY", NULL);
                    appendASTNodeAsChild(stackNode->parent->rightMostChild, arrNode);
                    popASTRightMostChild(stackNode->parent);
                    appendASTNodeAsChild(arrNode, stackNode->parent);
                    ASTNode* elementIndex = createASTNode("ARRAY_INDEX", stackNode->parseTreeNode);
                    appendASTNodeAsChild(elementIndex, arrNode);
                    pushChildrenToASTStack(st, elementIndex, stackNode->parseTreeNode->child);
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
                    ASTNode* declareStmtNode = createASTNode("DECLARE", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(declareStmtNode, stackNode->parent);
                    pushChildrenToASTStack(st, declareStmtNode, stackNode->parseTreeNode->child->next); // We donot need to insert DECLARE
                    break;
                }
                case 124: { // <conditionalStmt> = SWITCH BO ID BC START <caseStmts> <default> END
                    ASTNode* condStmtNode = createASTNode("SWITCH", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(condStmtNode, stackNode->parent);
                    pushChildrenToASTStack(st, condStmtNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 125: { // <caseStmts> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(caseStmtNode, stackNode->parent);
                    pushChildrenToASTStack(st, caseStmtNode, stackNode->parseTreeNode->child);
                    break;
                }
                case 126: { // <N9> = CASE <value> COLON <statements> BREAK SEMICOL <N9>
                    ASTNode* caseStmtNode = createASTNode("CASE", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(caseStmtNode, stackNode->parent->parent);
                    pushChildrenToASTStack(st, caseStmtNode, stackNode->parseTreeNode->child->next); // We donot need to insert CASE
                    break;
                }
                case 127: { // <N9> = e
                    break;
                }
                case 128: { // <value> = NUM
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 129: { // <value> = TRUE
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 130: { // <value> = FALSE 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 131: { // <default> = DEFAULT COLON <statements> BREAK SEMICOL 
                    ASTNode* defaultNode = createASTNode("DEFAULT", stackNode->parseTreeNode->child);
                    appendASTNodeAsChild(defaultNode, stackNode->parent);
                    pushChildrenToASTStack(st, defaultNode, stackNode->parseTreeNode->child);
                    break; 
                }
                case 132: { // <default> = e 
                    break;
                }
                case 133: { // <iterativeStmt> = FOR BO ID IN <range_for_loop> BC START <statements> END 
                    ASTNode* iterStmt = createASTNode("FOR", stackNode->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, stackNode->parent);
                    pushChildrenToASTStack(st, iterStmt, stackNode->parseTreeNode->child);
                    break;
                }
                case 134: { // <iterativeStmt> = WHILE BO <arithmeticOrBooleanExpr> BC START <statements> END 
                    ASTNode* iterStmt = createASTNode("WHILE", stackNode->parseTreeNode);
                    appendASTNodeAsChild(iterStmt, stackNode->parent);
                    pushChildrenToASTStack(st, iterStmt, stackNode->parseTreeNode->child);
                    break;
                }
                case 135: { // <range_for_loop> = <index_for_loop> RANGEOP <index_for_loop>
                    ASTNode* rangeForLoop = createASTNode("RANGE_FOR_LOOP", stackNode->parseTreeNode);
                    appendASTNodeAsChild(rangeForLoop, stackNode->parent);
                    pushChildrenToASTStack(st, rangeForLoop, stackNode->parseTreeNode->child);
                    break;
                }
                case 136: { // <index_for_loop> = <sign_for_loop> <new_index_for_loop> 
                    ASTNode* indexForLoop = createASTNode("INDEX_FOR_LOOP", stackNode->parseTreeNode);
                    appendASTNodeAsChild(indexForLoop, stackNode->parent);
                    pushChildrenToASTStack(st, indexForLoop, stackNode->parseTreeNode->child);
                    break;
                }
                case 137: { //<new_index_for_loop> = NUM 
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 138: { // <sign_for_loop> = PLUS
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
                    break;
                }
                case 139: { // <sign_for_loop> = MINUS
                    pushChildrenToASTStack(st, stackNode->parent, stackNode->parseTreeNode->child);
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

/**
 * @brief Initialised the queue used for printing of the AST
 * 
 * @return ASTQueue* 
 */
ASTQueue* initQueue(void) {
    ASTQueue* q = malloc(sizeof(ASTQueue));
    q->front = NULL;
    q->tail = NULL;
    q->sz = 0;

    return q;
}

/**
 * @brief Peek the front element of the queue used for printing of the AST
 * 
 * @param q 
 * @return ASTQueueNode* 
 */
ASTQueueNode* peekQueue(ASTQueue* q) {
    return q->front;
}

/**
 * @brief Pop the queue used for printing of the AST
 * 
 * @param q 
 */
void popQueue(ASTQueue* q) {
    ASTQueueNode* currFront = q->front;
    q->front = currFront->next;
    q->sz--;

    free(currFront);
    return;
}

/**
 * @brief Add an AST node to the queue used for printing of the AST
 * 
 * @param q 
 * @param node 
 */
void appendQueue(ASTQueue* q, ASTNode* node) {
    ASTQueueNode* qNode = malloc(sizeof(ASTQueueNode));

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

/**
 * @brief Print the AST using level-order traversal
 * 
 */
void prettyPrintAST(void) {
    ASTQueue* q = initQueue();

    appendQueue(q, tree->root);

    while (q->sz > 0) {
        int sz = q->sz;
        printf("%d\n", sz);

        for (int i = 0; i < sz; ++i) {
            ASTQueueNode* qNode = peekQueue(q);
            ASTNode* node = qNode->node;
            if (node == NULL) {
                printf("\n");
                popQueue(q);
                continue;
            }
            if (node->isLeaf) {
                printf("(%s %s %s)\n", node->parent->label, node->label, node->leaf.tok->lexeme);
            } else {
                printf("(%s %s)\n", node->parent->label, node->label);
            }
            popQueue(q);

            ASTNode* curr = node->leftMostChild;
            while (curr != NULL) {
                appendQueue(q, curr);
                curr = curr->next;
            }
            appendQueue(q, curr);
        }

        printf("\n");
    }

    return;
}

/**
 * @brief Driver function for AST
 * 
 * @param parseTree 
 */
void ASTCreator(ParseTree* parseTree) {
    pt = parseTree;
    createAST();
    return;
}