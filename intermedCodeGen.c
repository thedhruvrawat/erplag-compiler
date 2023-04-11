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
    if (quad->result->name[0] != '$') {
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

Record* generateTempRecord(SymbolTableNode* symbolTableNode, char* name, VAR_TYPE type, int* offset) {
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
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = true;
            } else if (arg1->isLeaf && arg1->leaf.tok->tok == FALSE) {
                quad->isArg2ID = false;
                quad->arg2Type = BOOL;
                quad->arg2Bool = false;
            } else {
                quad->isArg1ID = true;
                char* name = arg1->symbolTableLabel;
                quad->arg1ID = variableExists(symbolTableNode, name, hash(name));
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
            }

            char* name = result->leaf.tok->lexeme;
            quad->result = variableExists(symbolTableNode, name, hash(name));

            break;
        }

    }

    appendQuadruple(symbolTableNode, quad);
    return quad;
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

        // Adding to quadruple table
        // TODO: Add quadruple for array access
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
                // generateQuadruple(symbolTableNode, GET_VALUE_OP, NULL, NULL, statement->rightMostChild, 0);
                break;
            }
            case 'P': { // PRINT
                // Make two cases: one for single element and one for array access
                /* if (statement->rightMostChild->numChildren == 1) {
                    generateQuadruple(symbolTableNode, PRINT_ID_OP, NULL, NULL, statement->rightMostChild->leftMostChild, 0);
                } else {
                    ASTNode* indexNode = statement->rightMostChild->rightMostChild;
                    populateQuadrupleForExpressions(indexNode, symbolTableNode);
                    generateQuadruple(symbolTableNode, PRINT_ARR_ELE_OP, NULL, indexNode->leftMostChild, statement->rightMostChild->leftMostChild, 0);
                } */

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
                    populateQuadrupleForExpressions(indexNode, symbolTableNode);
                    generateQuadruple(symbolTableNode, ASSIGN_ARRAY_ACCESS_OP, expr, indexNode->leftMostChild, statement->leftMostChild, varRecord->type.array.arrType);
                } else if (idType != ARR) {
                    strcpy(idNode->symbolTableLabel, varRecord->name);
                    generateQuadruple(symbolTableNode, ASSIGN_VAR_OP, expr, NULL, idNode, idType);
                } else {
                    generateQuadruple(symbolTableNode, ASSIGN_ARRAY_OP, expr, NULL, idNode, varRecord->type.array.arrType);
                }
                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                break;
            }
            case 'D': { // DECLARE
                // No quadruple needed
                break;
            }
            case 'S': { // SWITCH
                break;
            }
            case 'F': { // FOR
                break;
            }
            case 'W': { // WHILE
                break;
            }

        }
        statement = statement->next;
    }
}

void printQuadrupleTable(void) {
    FILE* fp = fopen("quadrupleTable.txt", "w");
    fprintf(fp, "%-25s%-10s%-10s%-10s%-10s\n", "Operator", "Arg1", "Arg2", "Result", "Type");

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
        "MODULE_OP",
        "DRIVER_OP",
        "GET_VALUE_OP",
        "PRINT_ID_OP",
        "PRINT_ARR_ELE_OP",
        "MODULE_USE_OP",
        "SWITCH_OP",
        "CASE_OP",
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
            }
        }

        fprintf(fp, "%-10s", quad->result->name);
        fprintf(fp, "%-10s\n", typeStrings[quad->result->type.varType]);

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
        if (strcmp(module->label, "DRIVER") == 0) {
            name = "DRIVER";
        } else {
            name = module->leftMostChild->leaf.tok->lexeme;
        }
        GlobalRecord* moduleRecord = moduleExists(name, hash(name));
        populateQuadrupleTable(module->rightMostChild->leftMostChild, moduleRecord->funcST);

        module = module->next;
    }
}