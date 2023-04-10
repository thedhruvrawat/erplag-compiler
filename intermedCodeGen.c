/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdio.h>
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
            printf("Arg1: %s\n", arg1->symbolTableLabel);
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
            break;
        }

    }

    appendQuadruple(symbolTableNode, quad);
    return quad;
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

void populateQuadrupleForExpressions(ASTNode* exprNode, SymbolTableNode* symbolTableNode) {
    // We are entering the function for the first time
    /* if (strcmp(exprNode->label, "EXPR") == 0) {
        // If unary operator
        if (exprNode->numChildren == 2) {
            VAR_TYPE rhsType = typeExtractor(exprNode->rightMostChild, symbolTableNode);
            // Unary operations are not allowed on arrays
            if (rhsType == ARR) {
                printf(RED BOLD "[Semantic Analyser] Unary operations are not allowed on arrays at line %d\n" RESET, exprNode->rightMostChild->leaf.tok->linenum);
                return ERROR;
            } else {
                // Adding to quadruple table
                switch (exprNode->leftMostChild->leaf.tok->tok) {
                    case PLUS: {
                        strcpy(exprNode->leftMostChild->symbolTableLabel, exprNode->rightMostChild->symbolTableLabel);
                        break;
                    }
                    case MINUS: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, UMINUS_OP, exprNode->rightMostChild, NULL, NULL, rhsType);
                        strcpy(exprNode->leftMostChild->symbolTableLabel, quad->result->name);
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }
                return rhsType;
            }
        } else {
            VAR_TYPE type = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            strcpy(exprNode->symbolTableLabel, exprNode->leftMostChild->symbolTableLabel);
            return type;
        }
    }

    // If array (since there is no token in that case)
    if (strcmp(exprNode->label, "ARRAY") == 0) {
        char* name = exprNode->leftMostChild->leaf.tok->lexeme;
        unsigned int hashVal = hash(name);
        Record* varRecord = variableExists(symbolTableNode, name, hashVal);
        if (varRecord == NULL) {
            printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d.\n" RESET, name, exprNode->leftMostChild->leaf.tok->linenum);
            return ERROR;
        }

        // Type checking for the index
        ASTNode* indexNode = exprNode->rightMostChild;
        VAR_TYPE indexType = typeExtractor(indexNode->rightMostChild, symbolTableNode);
        if (indexType != INT) {
            printf(RED BOLD "[Semantic Analyser] Array index must be of type INTEGER at line %d.\n" RESET, exprNode->leftMostChild->leaf.tok->linenum);
            return ERROR;
        }

        // Bound checking for the array
        if (strcmp(indexNode->rightMostChild->label, "NUM") == 0) {
            int index = indexNode->rightMostChild->leaf.tok->num;
            if (indexNode->leftMostChild->leaf.tok->tok == MINUS) {
                index = -index;
            }

            if (!varRecord->type.array.isLeftID && !varRecord->type.array.isRightID) {
                int left = varRecord->type.array.left;
                if (varRecord->type.array.leftNegative) {
                    left = -left;
                }
                int right = varRecord->type.array.right;
                if (varRecord->type.array.rightNegative) {
                    right = -right;
                }

                if (index < left || index > right) {
                    printf(RED BOLD "[Semantic Analyser] Array index out of bounds at line %d.\n" RESET, indexNode->rightMostChild->leaf.tok->linenum);
                }
            }
        }

        // Adding to quadruple table
        // TODO: Add quadruple for array access

        return varRecord->type.array.arrType;
    }
    
    // If not array
    switch (exprNode->leaf.tok->tok) {
        case PLUS:
        case MINUS:
        case MUL: {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            // Add to quadruple table if no error
            if ((leftType != ERROR && rightType != ERROR) && (leftType == rightType) && (leftType == INT || leftType == DOUBLE)) {
                switch (exprNode->leaf.tok->tok) {
                    case PLUS: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, PLUS_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, leftType);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case MINUS: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, MINUS_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, leftType);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case MUL: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, MUL_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, leftType);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }
            }

            if (leftType == INT && rightType == INT) {
                return INT;
            } else if (leftType == DOUBLE && rightType == DOUBLE) {
                return DOUBLE;
            } else if (leftType == ERROR || rightType == ERROR) {
                return ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Invalid types for %s at line %d.\n" RESET, token_types[exprNode->leaf.tok->tok],exprNode->leaf.tok->linenum);
                return ERROR;
            }
            break;
        }
        case DIV: {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            if ((leftType == INT || leftType == DOUBLE) && (rightType == INT || rightType == DOUBLE)) {
                // Add to quadruple table
                Quadruple* quad = generateQuadruple(symbolTableNode, DIV_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, DOUBLE);
                strcpy(exprNode->symbolTableLabel, quad->result->name);

                return DOUBLE;
            } else if (leftType == ERROR || rightType == ERROR) {
                return ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Invalid types for DIV at line %d.\n" RESET, exprNode->leaf.tok->linenum);
                return ERROR;
            }
            break;
        }
        case AND: 
        case OR: 
        {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            if (leftType == BOOL && rightType == BOOL) {
                // Add to quadruple table
                switch (exprNode->leaf.tok->tok) {
                    case AND: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, AND_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case OR: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, OR_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }

                return BOOL;
            } else if (leftType == ERROR || rightType == ERROR) {
                return ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Invalid types for %s at line %d.\n" RESET, token_types[exprNode->leaf.tok->tok],exprNode->leaf.tok->linenum);
                return ERROR;
            }
            break;
        }
        case GT: 
        case LT:
        case GE:
        case LE:
        case EQ:
        case NE:
        {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            // Add to quadruple table
            if ((leftType != ERROR && rightType != ERROR) && (leftType == rightType) && (leftType == INT || leftType == DOUBLE)) {
                switch (exprNode->leaf.tok->tok) {
                    case GT: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, GT_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case LT: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, LT_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case GE: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, GE_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case LE: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, LE_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case EQ: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, EQ_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    case NE: {
                        Quadruple* quad = generateQuadruple(symbolTableNode, NE_OP, exprNode->leftMostChild, exprNode->rightMostChild, NULL, BOOL);
                        strcpy(exprNode->symbolTableLabel, quad->result->name);
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }
            }

            if (leftType == INT && rightType == INT) {
                return BOOL;
            } else if (leftType == DOUBLE && rightType == DOUBLE) {
                return BOOL;
            } else if (leftType == ERROR || rightType == ERROR) {
                return ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Invalid types for %s at line %d.\n" RESET, token_types[exprNode->leaf.tok->tok],exprNode->leaf.tok->linenum);
                return ERROR;
            }
            break;
        }
        case NUM: {
            // Modify symbolTableLabel
            strcpy(exprNode->symbolTableLabel, exprNode->leaf.tok->lexeme);
            return INT;
            break;
        }
        case RNUM: {
            // Modify symbolTableLabel
            strcpy(exprNode->symbolTableLabel, exprNode->leaf.tok->lexeme);
            return DOUBLE;
            break;
        }
        case TRUE:
        case FALSE: {
            // Modify symbolTableLabel
            strcpy(exprNode->symbolTableLabel, exprNode->leaf.tok->lexeme);
            return BOOL;
            break;
        }
        case ID: {
            char* name = exprNode->leaf.tok->lexeme;
            unsigned int hashVal = hash(name);
            Record* varRecord = variableExists(symbolTableNode, name, hashVal);
            if (varRecord == NULL) {
                printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, exprNode->leaf.tok->linenum);
                return ERROR;
            } else {
                // Modify symbolTableLabel
                printf("Name: %s\n", name);
                strcpy(exprNode->symbolTableLabel, name);
                return varRecord->type.varType;
            }
            break;
        }
        default: {
            // Should never reach here
            return ERROR;
            break;
        }
    } */
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
                break;
            }
            case 'P': { // PRINT
                break;
            }
            case 'A': { // ASSIGN_STMT

               /*  // Add to quadTable
                if (arrayAccess) {
                    Quadruple* quad = generateQuadruple(symbolTableNode, ASSIGN_ARRAY_ACCESS_OP, statement->rightMostChild, NULL, statement->leftMostChild, varRecord->type.array.arrType);
                } else if (idType != ARR) {
                    strcpy(idNode->symbolTableLabel, varRecord->name);
                    Quadruple* quad = generateQuadruple(symbolTableNode, ASSIGN_VAR_OP, statement->rightMostChild->leftMostChild, NULL, idNode, idType);
                } else {
                    Quadruple* quad = generateQuadruple(symbolTableNode, ASSIGN_ARRAY_OP, statement->rightMostChild, NULL, idNode, varRecord->type.array.arrType);
                } */
                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                break;
            }
            case 'D': { // DECLARE
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