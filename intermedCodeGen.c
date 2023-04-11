/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdio.h>
#include <string.h>
#include "symbolTable.h"
#include "intermedCodeGenDef.h"

QuadrupleTable* quadTable;

QuadrupleTable* initQuadrupleTable(void) {
    QuadrupleTable* quadTable = malloc(sizeof(QuadrupleTable));
    quadTable->currentNumber = 0;
    quadTable->head = NULL;
    quadTable->tail = NULL;
    quadTable->size = 0;

    return quadTable;
}

void appendQuadruple(SymbolTableNode* symbolTableNode, Quadruple* quad) {
    // Adding to the end of the quadruple table
    if (quadTable->head == NULL) {
        quadTable->head = quad;
        quadTable->tail = quad;
    } else {
        quadTable->tail->next = quad;
        quadTable->tail = quad;
    }
    quadTable->size++;

    // Adding to the symbolTable
    // Checking if the quadruple result if for a temporary or an actual variable
    if (quad->result == NULL || quad->result->name[0] != '$') {
        return;
    }
    Record* varRecord = quad->result;
    unsigned int hashVal = hash(varRecord->name);
    Record* insertAt = findVariableInsertion(symbolTableNode, varRecord->name, hashVal);
    if (insertAt == NULL) {
        symbolTableNode->hashTable[hashVal] = varRecord;
    } else {
        insertAt->next = varRecord;
    }

    return;
}

Record* generateTempRecord(SymbolTableNode* symbolTableNode, char* name, VAR_TYPE type, unsigned int* offset) {
    Record* newRec = (Record*) malloc(sizeof(Record));
    strcpy(newRec->name, name);
    newRec->offset = *offset;
    newRec->type.varType = type;
    newRec->assigned = 0;
    newRec->iterator = false;
    newRec->next = NULL;

    // Modify offset
    switch (type) {
        case INT: {
            *offset += SIZEOF_INT;
            break;
        }
        case REAL: {
            *offset += SIZEOF_REAL;
            break;
        }
        case BOOL: {
            *offset += SIZEOF_BOOL;
            break;
        }
        default: {
            // Should never reach here
            break;
        }
    }

    return newRec;
}
Quadruple* generateQuadruple(SymbolTableNode* symbolTableNode, OPERATOR op, ASTNode* arg1, ASTNode* arg2, ASTNode* result, VAR_TYPE type) {
    Quadruple* quad = (Quadruple*) malloc(sizeof(Quadruple));
    quad->op = op;
    quad->next = NULL;
    quad->symbolTableNode = symbolTableNode;

    switch(op) {
        case PLUS_OP:
        case MINUS_OP:
        case MUL_OP:
        case DIV_OP: 
        case GT_OP: 
        case LT_OP: 
        case GE_OP: 
        case LE_OP: 
        case EQ_OP: 
        case NE_OP: 
        case AND_OP: 
        case OR_OP: {   
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == RNUM) {
                quad->isArg1ID = false;
                quad->arg1Type = DOUBLE;
                quad->arg1Real = arg1->leaf.tok->rnum;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = quad->arg1ID->type.varType;
            }

            if (arg2->isLeaf && arg2->leaf.tok->tok == NUM) {
                quad->isArg2ID = false;
                quad->arg2Type = INT;
                quad->arg2Num = arg2->leaf.tok->num;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == RNUM) {
                quad->isArg2ID = false;
                quad->arg2Type = DOUBLE;
                quad->arg2Real = arg2->leaf.tok->rnum;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == TRUE) {
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = true;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == FALSE) {
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = false;
            } else {
                quad->isArg2ID = true;
                char* name = arg2->symbolTableLabel;
                quad->arg2ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg2Type = quad->arg2ID->type.varType;
            }

            Record* resultRecord;
            if (result == NULL) {
                char name[20];
                sprintf(name, "$_t%d", ++quadTable->currentNumber);
                resultRecord = generateTempRecord(symbolTableNode, name, type, &symbolTableNode->nextOffset);
            } else {
                char* name = result->symbolTableLabel;
                resultRecord = variableExists(symbolTableNode, name, hash(name));
            }

            quad->result = resultRecord;

            break;
        }
        case UMINUS_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == RNUM) {
                quad->isArg1ID = false;
                quad->arg1Type = DOUBLE;
                quad->arg1Real = arg1->leaf.tok->rnum;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = quad->arg1ID->type.varType;
            }

            Record* resultRecord;
            if (result == NULL) {
                char name[20];
                sprintf(name, "$_t%d", ++quadTable->currentNumber);
                resultRecord = generateTempRecord(symbolTableNode, name, type, &symbolTableNode->nextOffset);
            } else {
                char* name = result->symbolTableLabel;
                resultRecord = variableExists(symbolTableNode, name, hash(name));
            }

            quad->isArg2ID = true;
            quad->arg2ID = NULL;
            quad->result = resultRecord;
            break;
        }
        case ASSIGN_VAR_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == RNUM) {
                quad->isArg1ID = false;
                quad->arg1Type = DOUBLE;
                quad->arg1Real = arg1->leaf.tok->rnum;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = quad->arg1ID->type.varType;
            }

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            char* name = result->leaf.tok->lexeme;
            quad->result = variableExists(symbolTableNode, name, hash(name));

            break;
        }
        case ASSIGN_ARRAY_OP: {
            quad->isArg1ID = true;
            quad->arg1Type = ARR;

            char* name = arg1->leaf.tok->lexeme;
            quad->arg1ID = variableExists(symbolTableNode, name, hash(name));

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            name = result->symbolTableLabel;
            quad->result = variableExists(symbolTableNode, name, hash(name));

            break;
        }
        case ASSIGN_ARRAY_ACCESS_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == RNUM) {
                quad->isArg1ID = false;
                quad->arg1Type = DOUBLE;
                quad->arg1Real = arg1->leaf.tok->rnum;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = ARR;
            }

            if (arg2->isLeaf && arg2->leaf.tok->tok == NUM) {
                quad->isArg2ID = false;
                quad->arg2Type = INT;
                quad->arg2Num = arg2->leaf.tok->num;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == RNUM) {
                quad->isArg2ID = false;
                quad->arg2Type = DOUBLE;
                quad->arg2Real = arg2->leaf.tok->rnum;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == TRUE) {
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = true;
            } else if (arg2->isLeaf && arg2->leaf.tok->tok == FALSE) {
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = false;
            } else {
                quad->isArg2ID = true;
                char* name = arg2->symbolTableLabel;
                quad->arg2ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg2Type = INT;
            }

            char* name = result->leaf.tok->lexeme;
            quad->result = variableExists(symbolTableNode, name, hash(name));

            break;
        }
        case ARRAY_ACCESS_OP: {
            quad->isArg1ID = true;
            char* name = arg1->leaf.tok->lexeme;
            quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
            quad->arg1Type = ARR;

            if (arg2->isLeaf && arg2->leaf.tok->tok == NUM) {
                quad->isArg2ID = false;
                quad->arg2Type = INT;
                quad->arg2Num = arg2->leaf.tok->num;
            } else {
                quad->isArg2ID = true;
                name = arg2->symbolTableLabel;
                quad->arg2Type = INT;
                quad->arg2ID = variableExists(symbolTableNode, name, hash(name));
            }

            char resName[20];
            sprintf(resName, "$_t%d", ++quadTable->currentNumber);
            quad->result = generateTempRecord(symbolTableNode, resName, type, &symbolTableNode->nextOffset);

            break;
        }
        case MODULE_OP:
        case DRIVER_OP: {
            quad->isArg1ID = true;
            quad->arg1ID = NULL;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
        case GET_VALUE_OP: {
            quad->isArg1ID = true;
            quad->arg1ID = NULL;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            char* name = result->leaf.tok->lexeme;
            quad->result = variableExists(symbolTableNode, name, hash(name));

            break;
        }
        case PRINT_ID_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == RNUM) {
                quad->isArg1ID = false;
                quad->arg1Type = DOUBLE;
                quad->arg1Real = arg1->leaf.tok->rnum;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->leaf.tok->lexeme;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = quad->arg1ID->type.varType;
            }

            quad->isArg2ID = true;
            quad->arg2ID = NULL;
            quad->result = NULL;

            break;
        }
        case PRINT_ARR_ELE_OP: {
            quad->isArg1ID = true;
            char* name = arg1->leaf.tok->lexeme;
            quad->arg1ID = variableExists(symbolTableNode, name, hash(name));

            if (arg2->isLeaf && arg2->leaf.tok->tok == NUM) {
                quad->isArg2ID = false;
                quad->arg2Type = INT;
                quad->arg2Num = arg2->leaf.tok->num;
            } else {
                quad->isArg2ID = true;
                name = arg2->symbolTableLabel;
                quad->arg2ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg2Type = quad->arg2ID->type.array.arrType;
            }

            quad->result = NULL;

            break;
        }
        case MODULE_USE_OP: {
            quad->isArg1ID = true;
            quad->arg1ID = NULL;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;
            break;
        }
        case SWITCH_OP: {
            quad->isArg1ID = true;
            char* name = arg1->leaf.tok->lexeme;
            quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
            quad->arg1Type = quad->arg1ID->type.varType;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
        case CASE_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == NUM) {
                quad->isArg1ID = false;
                quad->arg1Type = INT;
                quad->arg1Num = arg1->leaf.tok->num;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            }

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
        case DEFAULT_OP: {
            quad->isArg1ID = true;
            quad->arg1ID = NULL;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
        case FOR_OP: {
            quad->isArg1ID = false;
            quad->arg1ID = NULL;
            quad->arg1Type = INT;

            quad->isArg2ID = false;
            quad->arg2ID = NULL;
            quad->arg2Type = INT;

            char* name = result->leaf.tok->lexeme;
            quad->result = variableExists(symbolTableNode, name, hash(name));
            break;
        }
        case WHILE_OP: {
            if (arg1->isLeaf && arg1->leaf.tok->tok == TRUE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg1ID = false;
                quad->arg1Type = BOOL;
                quad->arg1Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
                quad->arg1Type = quad->arg1ID->type.varType;
            }

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
        case START_OP: 
        case END_OP: {
            // Just the operator itself is sufficient
            quad->isArg1ID = true;
            quad->arg1ID = NULL;

            quad->isArg2ID = true;
            quad->arg2ID = NULL;

            quad->result = NULL;

            break;
        }
    }

    appendQuadruple(symbolTableNode, quad);
    return quad;
}

Quadruple* generateStartQuadruple(void) {
    return generateQuadruple(NULL, START_OP, NULL, NULL, NULL, 0);
}

Quadruple* generateEndQuadruple(void) {
    return generateQuadruple(NULL, END_OP, NULL, NULL, NULL, 0);
}

void populateQuadrupleForExpressions(ASTNode* exprNode, SymbolTableNode* symbolTableNode) {
    // We are entering the function for the first time
    if (strcmp(exprNode->label, "EXPR") == 0) {
        // If unary operator
        if (exprNode->numChildren == 2) {
            populateQuadrupleForExpressions(exprNode->rightMostChild, symbolTableNode);
            // Adding to quadruple table
            switch (exprNode->leftMostChild->leaf.tok->tok) {
                case PLUS: {
                    strcpy(exprNode->leftMostChild->symbolTableLabel, exprNode->rightMostChild->symbolTableLabel);
                    break;
                }
                case MINUS: {
                    int type = exprNode->rightMostChild->type;
                    Quadruple* quad = generateQuadruple(symbolTableNode, UMINUS_OP, exprNode->rightMostChild, NULL, NULL, type);
                    strcpy(exprNode->leftMostChild->symbolTableLabel, quad->result->name);
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }
        } else {
            populateQuadrupleForExpressions(exprNode->leftMostChild, symbolTableNode);
            strcpy(exprNode->symbolTableLabel, exprNode->leftMostChild->symbolTableLabel);
        }

        return;
    }

    // If array (since there is no token in that case)
    if (strcmp(exprNode->label, "ARRAY") == 0) {
        populateQuadrupleForExpressions(exprNode->rightMostChild, symbolTableNode);
        Quadruple* quad = generateQuadruple(symbolTableNode, ARRAY_ACCESS_OP, exprNode->leftMostChild, exprNode->rightMostChild->leftMostChild, NULL, exprNode->type);
        strcpy(exprNode->symbolTableLabel, quad->result->name);
        return;
    }

    if (strcmp(exprNode->label, "ARRAY_INDEX") == 0) {
        populateQuadrupleForExpressions(exprNode->rightMostChild, symbolTableNode);
        if (exprNode->numChildren == 2) {
            if (exprNode->leftMostChild->leaf.tok->tok == MINUS) {
                int type = exprNode->rightMostChild->type;
                Quadruple* quad = generateQuadruple(symbolTableNode, UMINUS_OP, exprNode->rightMostChild, NULL, NULL, type);
                strcpy(exprNode->leftMostChild->symbolTableLabel, quad->result->name);
            } else {
                strcpy(exprNode->leftMostChild->symbolTableLabel, exprNode->rightMostChild->symbolTableLabel);
            }
        }

        return;
    }
    
    // If not array
    VAR_TYPE type = exprNode->type;
    OPERATOR op;
    switch (exprNode->leaf.tok->tok) {
        case PLUS: {
            op = PLUS_OP;
            break;
        }
        case MINUS: {
            op = MINUS_OP;
            break;
        }
        case MUL:  {
            op = MUL_OP;
            break;
        }
        case DIV: {
            op = DIV_OP;
            break;
        }
        case AND: {
            op = AND_OP;
            break;
        }
        case OR: {
            op = OR_OP;
            break;
        }
        case GT: {
            op = GT_OP;
            break;
        }
        case LT: {
            op = LT_OP;
            break;
        }
        case GE: {
            op = GE_OP;
            break;
        }
        case LE: {
            op = LE_OP;
            break;
        }
        case EQ: {
            op = EQ_OP;
            break;
        }
        case NE: {
            op = NE_OP;
            break;
        }
        case NUM: 
        case RNUM: 
        case TRUE:
        case FALSE: {
            // Modify symbolTableLabel
            strcpy(exprNode->symbolTableLabel, exprNode->leaf.tok->lexeme);
            return;
        }
        case ID: {
            char* name = exprNode->leaf.tok->lexeme;
            unsigned int hashVal = hash(name);
            Record* varRecord = variableExists(symbolTableNode, name, hashVal);
            // Modify symbolTableLabel
            strcpy(exprNode->symbolTableLabel, name);
            return;
        }
        default: {
            // Should never reach here
            break;
        }
    }

    populateQuadrupleForExpressions(exprNode->leftMostChild, symbolTableNode);
    populateQuadrupleForExpressions(exprNode->rightMostChild, symbolTableNode);
    Quadruple* quad = generateQuadruple(symbolTableNode, op, exprNode->leftMostChild, exprNode->rightMostChild, NULL, type);
    strcpy(exprNode->symbolTableLabel, quad->result->name);

    return;
}

void populateQuadrupleTable(ASTNode* statement, SymbolTableNode* symbolTableNode) {
    /*
        Types of statements possible:
        1. GET_VALUE
        2. PRINT
        3. ASSIGN_STMT
        4. MODULE_REUSE_STMT
        5. DECLARE
        6. SWITCH
        7. FOR
        8. WHILE
    */

    while (statement != NULL) {
        switch (statement->label[0]) {
            case 'G': { // GET_VALUE
                generateQuadruple(symbolTableNode, GET_VALUE_OP, NULL, NULL, statement->rightMostChild, 0);
                break;
            }
            case 'P': { // PRINT
                if (statement->numChildren == 1) {
                    generateQuadruple(symbolTableNode, PRINT_ID_OP, statement->leftMostChild, NULL, NULL, 0);
                } else {
                    ASTNode* indexNode = statement->rightMostChild;
                    populateQuadrupleForExpressions(indexNode, symbolTableNode);
                    generateQuadruple(symbolTableNode, PRINT_ARR_ELE_OP, statement->leftMostChild, indexNode->leftMostChild, NULL, 0);
                }

                break;
            }
            case 'A': { // ASSIGN_STMT
                ASTNode* idNode = statement->leftMostChild;
                bool arrayAccess = !(strcmp(idNode->label, "ID") == 0);
                if (arrayAccess) {
                    idNode = idNode->leftMostChild;
                }

                char* name = idNode->leaf.tok->lexeme;
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                VAR_TYPE idType = varRecord->type.varType;

                populateQuadrupleForExpressions(statement->rightMostChild, symbolTableNode);

                ASTNode* expr = statement->rightMostChild->leftMostChild;
                if (arrayAccess) {
                    ASTNode* indexNode = statement->leftMostChild->rightMostChild;
                    populateQuadrupleForExpressions(indexNode->rightMostChild, symbolTableNode);
                    generateQuadruple(symbolTableNode, ASSIGN_ARRAY_ACCESS_OP, expr, indexNode->leftMostChild, idNode, varRecord->type.array.arrType);
                } else if (idType != ARR) {
                    strcpy(idNode->symbolTableLabel, varRecord->name);
                    generateQuadruple(symbolTableNode, ASSIGN_VAR_OP, expr, NULL, idNode, idType);
                } else {
                    generateQuadruple(symbolTableNode, ASSIGN_ARRAY_OP, expr, NULL, idNode, varRecord->type.array.arrType);
                }
                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                Quadruple* quad = generateQuadruple(symbolTableNode, MODULE_USE_OP, NULL, NULL, NULL, 0);

                // TODO
                /* ASTNode* outputNode = statement->leftMostChild;
                ASTNode* moduleNode = outputNode->next;
                ASTNode* inputNode = moduleNode->next;

                strcpy(quad->moduleName, moduleNode->leaf.tok->lexeme);

                RecordList* inputList = malloc(sizeof(RecordList));
                inputList->head = inputList->tail = NULL;
                inputList->size = 0;

                ASTNode* curr = inputNode->leftMostChild;
                while (curr != NULL) {
                    bool isMinus = false;
                    if (strcmp(curr->label, "MINUS_NODE") == 0) {
                        isMinus = true;
                        curr = curr->leftMostChild;
                    }

                    Record* varRecord = NULL;
                    if (strcmp(curr->label, "ID") == 0) {
                        char* name = curr->leaf.tok->lexeme;
                        varRecord = variableExists(symbolTableNode, name, hash(name));
                    }

                    
                }
                

                RecordList* outputList = malloc(sizeof(RecordList));
                outputList->head = outputList->tail = NULL;
                outputList->size = 0; */
                break;
            }
            case 'D': { // DECLARE
                // No quadruple needed
                break;
            }
            case 'S': { // SWITCH
                generateQuadruple(symbolTableNode, SWITCH_OP, statement->leftMostChild, NULL, NULL, 0);

                ASTNode* caseNode = statement->leftMostChild->next;
                while (caseNode != NULL) {
                    if (caseNode->leaf.tok->tok == DEFAULT) {
                        generateQuadruple(symbolTableNode, DEFAULT_OP, NULL, NULL, NULL, 0);
                    } else {
                        generateQuadruple(symbolTableNode, CASE_OP, caseNode->leftMostChild, NULL, NULL, 0);
                    }
                    generateStartQuadruple();
                    populateQuadrupleTable(caseNode->leftMostChild->next, symbolTableNode);
                    generateEndQuadruple();

                    caseNode = caseNode->next;
                }
                break;
            }
            case 'F': { // FOR
                ASTNode* idNode = statement->leftMostChild; 
                ASTNode* rangeNode = statement->leftMostChild->next;
                ASTNode* leftIndexNode = rangeNode->leftMostChild;
                ASTNode* rightIndexNode = rangeNode->rightMostChild;

                int left = leftIndexNode->rightMostChild->leaf.tok->num;
                int right = rightIndexNode->rightMostChild->leaf.tok->num;
                if (leftIndexNode->numChildren == 2) {
                    if (leftIndexNode->leftMostChild->leaf.tok->tok == MINUS) {
                        left = -left;
                    }
                }
                if (rightIndexNode->numChildren == 2) {
                    if (rightIndexNode->leftMostChild->leaf.tok->tok == MINUS) {
                        right = -right;
                    }
                }

                Quadruple* quad = generateQuadruple(symbolTableNode, FOR_OP, NULL, NULL, idNode, 0);
                quad->arg1Num = left;
                quad->arg2Num = right;

                generateStartQuadruple();
                populateQuadrupleTable(statement->leftMostChild->next->next, symbolTableNode);
                generateEndQuadruple();

                break;
            }
            case 'W': { // WHILE
                populateQuadrupleForExpressions(statement->leftMostChild, symbolTableNode);
                generateQuadruple(symbolTableNode, WHILE_OP, statement->leftMostChild, NULL, NULL, 0);

                generateStartQuadruple();
                populateQuadrupleTable(statement->leftMostChild->next, symbolTableNode);
                generateEndQuadruple();
                break;
            }

        }
        statement = statement->next;
    }
}

void printQuadrupleTable(void) {
    FILE* fp = fopen("quadrupleTable.txt", "w");
    fprintf(fp, "%-25s%-10s%-10s%-10s%-10s%-10s\n", "Operator", "Arg1", "Arg2", "Result", "Type", "Offset");

    Quadruple* quad = quadTable->head;
    char* opStrings[] = {
        "PLUS_OP",
        "MINUS_OP",
        "MUL_OP",
        "DIV_OP",
        "GT_OP",
        "LT_OP",
        "LE_OP",
        "GE_OP",
        "EQ_OP",
        "NE_OP",
        "AND_OP",
        "OR_OP",
        "UPLUS_OP",
        "UMINUS_OP",
        "ASSIGN_VAR_OP",
        "ASSIGN_ARRAY_OP",
        "ASSIGN_ARRAY_ACCESS_OP",
        "ARRAY_ACCESS_OP",
        "MODULE_OP",
        "DRIVER_OP",
        "GET_VALUE_OP",
        "PRINT_ID_OP",
        "PRINT_ARR_ELE_OP",
        "MODULE_USE_OP",
        "SWITCH_OP",
        "CASE_OP",
        "DEFAULT_OP",
        "FOR_OP",
        "WHILE_OP",
        "START_OP",
        "END_OP"
    };

    char* typeStrings[] = {
        "INTEGER",
        "REAL",
        "BOOLEAN",
        "ARRAY",
    };

    while (quad != NULL) {
        fprintf(fp, "%-25s", opStrings[quad->op]);

        printf("%s\n", opStrings[quad->op]);
        if (quad->isArg1ID) {
            if (quad->arg1ID == NULL) { 
                fprintf(fp, "%-10s", "**");
            } else {
                fprintf(fp, "%-10s", quad->arg1ID->name);
            }
        } else {
            switch(quad->arg1Type) {
                case INT: {
                    fprintf(fp, "%-10d", quad->arg1Num);
                    break;
                }
                case DOUBLE: {
                    fprintf(fp, "%-10lf", quad->arg1Real);
                    break;
                }
                case BOOL: {
                    fprintf(fp, "%-10s", (quad->arg1Bool ? "TRUE" : "FALSE"));
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }
        }

        if (quad->isArg2ID) {
            if (quad->arg2ID == NULL) {
                fprintf(fp, "%-10s", "**");
            } else {
                fprintf(fp, "%-10s", quad->arg2ID->name);
            }
        } else {
            switch(quad->arg2Type) {
                case INT: {
                    fprintf(fp, "%-10d", quad->arg2Num);
                    break;
                }
                case DOUBLE: {
                    fprintf(fp, "%-10lf", quad->arg2Real);
                    break;
                }
                case BOOL: {
                    fprintf(fp, "%-10s", (quad->arg2Bool ? "TRUE" : "FALSE"));
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }
        }

        if (quad->result == NULL) {
            fprintf(fp, "%-10s", "**");
            if (quad->op == PRINT_ID_OP) {
                if (quad->isArg1ID) {
                    fprintf(fp, "%-10s", typeStrings[quad->arg1ID->type.varType]);
                } else {
                    fprintf(fp, "%-10s", typeStrings[quad->arg1Type]);
                }
            } else if (quad->op == PRINT_ARR_ELE_OP) {
                fprintf(fp, "%-10s", typeStrings[quad->arg1ID->type.array.arrType]);
            } else {
                fprintf(fp, "%-10s", "**");
            }
        } else {
            fprintf(fp, "%-10s", quad->result->name);
            fprintf(fp, "%-10s", typeStrings[quad->result->type.varType]);
        }

        if (quad->result == NULL) {
            fprintf(fp, "%-10s\n", "**");
        } else {
            fprintf(fp, "%-10d\n", quad->result->offset);
        }

        quad = quad->next;
    }

    fclose(fp);
}

void createQuadrupleTable(void) {
    quadTable = initQuadrupleTable();

    // Iterate through the modules
    ASTNode* module = tree->root->leftMostChild->next;
    while (module != NULL) {
        // make quadruple for module
        char* name;
        Quadruple* quad;
        if (strcmp(module->label, "DRIVER") == 0) {
            name = "DRIVER";
            quad = generateQuadruple(NULL, DRIVER_OP, NULL, NULL, NULL, 0);
        } else {
            name = module->leftMostChild->leaf.tok->lexeme;
            quad = generateQuadruple(NULL, MODULE_OP, NULL, NULL, NULL, 0);
            strcpy(quad->moduleName, name);
        }
        GlobalRecord* moduleRecord = moduleExists(name, hash(name));
        generateStartQuadruple();
        populateQuadrupleTable(module->rightMostChild->leftMostChild, moduleRecord->funcST);
        generateEndQuadruple();

        module = module->next;
    }
}