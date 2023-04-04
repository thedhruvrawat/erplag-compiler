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
#include "symbolTableDef.h"
#include "lexerDef.h"
#include "colorCodes.h"

SymbolTable* symbolTable;

// Fowler–Noll–Vo hash function
// https://datatracker.ietf.org/doc/html/draft-eastlake-fnv-03
unsigned int hash(const char *str) {
    unsigned int res = 0;
    const unsigned int prime = 16777619;
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        res = (res * prime) ^ str[i];
    }

    return (((res % HASH_TABLE_SIZE) + HASH_TABLE_SIZE) % HASH_TABLE_SIZE);
}

SymbolTableNode* initSymbolTableNode(void) {
    SymbolTableNode* newNode = malloc(sizeof(SymbolTableNode));
    // newNode->hashTable is a fixed-length array
    memset(newNode->hashTable, 0, sizeof(Record*) * HASH_TABLE_SIZE);
    newNode->children = NULL;
    newNode->nextOffset = 0;
    newNode->funcOutputST = NULL;
    newNode->next = NULL;
    newNode->parent = NULL;

    return newNode;
}

SymbolTable* initSymbolTable(void) {
    SymbolTable* symTab = malloc(sizeof(SymbolTable));
    // Global Record Table is a fixed-length array
    memset(symTab->global, 0, sizeof(GlobalRecord*) * HASH_TABLE_SIZE);
    symTab->height = 0;

    return symTab;
}

void pushToSymbolTableStack(SymbolTableStack* st, ASTNode* node, unsigned int offset) {
    SymbolTableStackNode* newNode = malloc(sizeof(SymbolTableStackNode));
    newNode->node = node;
    newNode->offset = offset;

    if (st->size == 0) {
        newNode->next = NULL;
    } else {
        newNode->next = st->top;
    }

    st->top = newNode;
    st->size++;

    return;
}

SymbolTableStackNode* peekSymbolTableStack(SymbolTableStack* st) {
    return st->top;
}

void popSymbolTableStack(SymbolTableStack* st) {
    if (st->size == 0) {
        printf("SymbolTableStack is empty!\n");
        exit(1);
    }

    SymbolTableStackNode* currTop = st->top;
    st->top = st->top->next;
    st->size--;
    free(currTop);

    return;
}

SymbolTableStack* initSymbolTableStack(void) {
    SymbolTableStack* st = malloc(sizeof(SymbolTableStack));
    st->top = NULL;
    st->size = 0;

    return st;
}

GlobalRecord* moduleExists(char* name, unsigned int hashVal) {
    GlobalRecord* moduleNode = symbolTable->global[hashVal];
    while (moduleNode != NULL) {
        if (strcmp(moduleNode->name, name) == 0) {
            return moduleNode;
        }
        moduleNode = moduleNode->next;
    }

    return NULL;
}

Record* variableExists(SymbolTableNode* symbolTableNode, char* name, unsigned int hashVal) {
    if (symbolTableNode == NULL) {
        return NULL;
    }

    Record* varRecord = variableExists(symbolTableNode->funcOutputST, name, hashVal);
    if (varRecord != NULL) {
        return varRecord;
    }
    
    varRecord = symbolTableNode->hashTable[hashVal];
    while (varRecord != NULL) {
        if (strcmp(varRecord->name, name) == 0) {
            return varRecord;
        }
        varRecord = varRecord->next;
    }

    if (symbolTableNode->parent == NULL) {
        return NULL;
    } else {
        return variableExists(symbolTableNode->parent, name, hashVal);
    }
}

Record* generateRecord(SymbolTableNode* symbolTableNode, ASTNode* idNode, ASTNode* dataTypeNode, unsigned int* nextOffset) {
    char* name = idNode->leaf.tok->lexeme;
    Record* newRec = malloc(sizeof(Record));
    strcpy(newRec->name, name);
    newRec->iterator = false;
    newRec->offset = *nextOffset;
    // newRec->linenum = idNode->leaf.tok->linenum;
    newRec->next = NULL;

    switch (dataTypeNode->leaf.tok->tok) {
        case INTEGER: {
            newRec->type.varType = INT;
            *nextOffset += sizeof(int);
            break;
        }
        case REAL: {
            newRec->type.varType = DOUBLE;
            *nextOffset += sizeof(double);
            break;
        }
        case BOOLEAN: {
            newRec->type.varType = BOOL;
            *nextOffset += sizeof(bool);
            break;
        }
        case ARRAY: {
            newRec->type.varType = ARR;
            int elementSize;
            
            // Setting the datatype of the elements of the array
            switch (dataTypeNode->rightMostChild->leaf.tok->tok) {
                case INTEGER: {
                    newRec->type.array.arrType = INT;
                    elementSize = sizeof(int);
                    break;
                }
                case REAL: {
                    newRec->type.array.arrType = DOUBLE;
                    elementSize = sizeof(double);
                    break;
                }
                case BOOLEAN: {
                    newRec->type.array.arrType = BOOL;
                    elementSize = sizeof(bool);
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }

            ASTNode* rangeArrNode = dataTypeNode->leftMostChild;

            // Resolving left bound
            ASTNode* leftBound = rangeArrNode->leftMostChild;
            ASTNode* leftIDNum = leftBound->leftMostChild;
            if (leftBound->numChildren == 2) {
                newRec->type.array.leftNegative = (leftIDNum->label[0] == 'M');
                leftIDNum = leftIDNum->next;
            } else {
                newRec->type.array.leftNegative = false;
            }

            if (leftIDNum->leaf.tok->tok == ID) {
                newRec->type.array.isLeftID = true;
                strcpy(newRec->type.array.leftID, leftIDNum->leaf.tok->lexeme);
            } else {
                newRec->type.array.isLeftID = false;
                newRec->type.array.left = leftIDNum->leaf.tok->num;
            }

            // Resolving right bound
            ASTNode* rightBound = rangeArrNode->rightMostChild;
            ASTNode* rightIDNum = rightBound->leftMostChild;
            if (rightBound->numChildren == 2) {
                newRec->type.array.rightNegative = (rightIDNum->label[0] == 'M');
                rightIDNum = rightIDNum->next;
            } else {
                newRec->type.array.rightNegative = false;
            }

            if (rightIDNum->leaf.tok->tok == ID) {
                newRec->type.array.isRightID = true;
                strcpy(newRec->type.array.rightID, rightIDNum->leaf.tok->lexeme);
            } else {
                newRec->type.array.isRightID = false;
                newRec->type.array.right = rightIDNum->leaf.tok->num;
            }

            // The static flag of the ast node itself (required during code generation (maybe))
            bool isStatic = !(newRec->type.array.isLeftID || newRec->type.array.isRightID);

            // Calculating next offset
            if (isStatic) {
                int left = newRec->type.array.left * (newRec->type.array.leftNegative ? -1 : 1);
                int right = newRec->type.array.right * (newRec->type.array.rightNegative ? -1 : 1);

                if (left > right) {
                    printf(RED BOLD "[Semantic Analyser] Specified subrange for array \"%s\" are illegal. (%d > %d)\n" RESET, name, left, right);
                    return NULL;
                }
                *nextOffset += elementSize * (right - left);
            } else {
                // Adding the size of pointer
                *nextOffset += sizeof(void*);
                // Check whether both the dynamic bounds have been declared and are of integer type
                if (newRec->type.array.isLeftID) {
                    char* leftID = newRec->type.array.leftID;
                    Record* leftRec = variableExists(symbolTableNode, leftID, hash(leftID));
                    if (leftRec == NULL) {
                        printf(RED BOLD "[Semantic Analyser] The left bound of array \"%s\" is not declared.\n" RESET, name);
                        return NULL;
                    }
                    if (leftRec->type.varType != INT) {
                        printf(RED BOLD "[Semantic Analyser] The left bound of array \"%s\" is not of type integer.\n" RESET, name);
                        return NULL;
                    }
                }
            }
            break;            
        }
        default: {
            // Should never reach here
            break;
        }
    }

    return newRec;
}

GlobalRecord* findFunction(char* name, unsigned int hashVal) {
    if (symbolTable->global[hashVal] == NULL) {
        return NULL;
    }

    GlobalRecord* curr = symbolTable->global[hashVal];
    while (curr->next != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return curr;
}


Record* findVariableInsertion(SymbolTableNode* symbolTableNode, char* name, unsigned int hashVal) {
    if (symbolTableNode->hashTable[hashVal] == NULL) {
        return NULL;
    }

    Record* curr = symbolTableNode->hashTable[hashVal];
    while (curr->next != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return curr;
}

void populateInputOutputList(GlobalRecord* funcRecord, ASTNode* inputList, ASTNode* outputList) {
    unsigned int* offset = &funcRecord->funcST->nextOffset;
    SymbolTableNode* symbolTableNode = funcRecord->funcST;

    // Creating the inputList
    int inputListSize = 0;
    Record* sentinel = malloc(sizeof(Record));
    sentinel->next = NULL;
    Record* curr = sentinel;
    ASTNode* inputNode = inputList->leftMostChild;
    while (inputNode != NULL) {
        // Adding the input variable to the inputList
        Record* newRecord = generateRecord(symbolTableNode, inputNode->leftMostChild, inputNode->rightMostChild, offset);
        curr->next = newRecord;
        curr = curr->next;
        inputListSize++;

        // Adding the input variable to the symbolTableNode
        char* name = newRecord->name;
        unsigned int hashVal = hash(name);
        Record* varRecord = findVariableInsertion(symbolTableNode, name, hashVal);
        int linenum = inputNode->leftMostChild->leaf.tok->linenum;
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
            symbolTableNode->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf(RED BOLD "[Semantic Analyser] Redeclaration of variable %s in input list of module %s at line %d\n" RESET, name, funcRecord->name, linenum);
        } else {
            varRecord->next = malloc(sizeof(Record));
            varRecord = varRecord->next;
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
        }

        inputNode = inputNode->next;
    }
    curr->next = NULL;
    funcRecord->inputList = sentinel->next;
    funcRecord->inputListSize = inputListSize;

    // Creating the outputList
    int outputListSize = 0;
    sentinel->next = NULL;
    curr = sentinel;
    ASTNode* outputNode = outputList->leftMostChild;
    while (outputNode != NULL) {
        // Adding the output variable to the outputList
        Record* newRecord = generateRecord(symbolTableNode, outputNode->leftMostChild, outputNode->rightMostChild, offset);
        curr->next = newRecord;
        curr = curr->next;
        outputListSize++;

        // Adding the output variable to the outputSymbolTableNode
        symbolTableNode = funcRecord->outputST;
        char* name = newRecord->name;
        unsigned int hashVal = hash(name);

        // Checking if the variable has already been declared in the inputList
        Record* varRecord = variableExists(funcRecord->funcST, name, hashVal);
        if (varRecord != NULL) {
            printf(RED BOLD "[Semantic Analyser] Redeclaration of variable %s in output list of module %s at line %d\n" RESET, name, funcRecord->name, outputNode->leftMostChild->leaf.tok->linenum);
            outputNode = outputNode->next;
            continue;
        }

        varRecord = findVariableInsertion(symbolTableNode, name, hashVal);
        int linenum = outputNode->leftMostChild->leaf.tok->linenum;
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
            symbolTableNode->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf(RED BOLD "[Semantic Analyser] Redeclaration of variable %s in output list of module %s at line %d\n" RESET, name, funcRecord->name, linenum);
        } else {
            varRecord->next = malloc(sizeof(Record));
            varRecord = varRecord->next;
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
        }

        outputNode = outputNode->next;
    }
    curr->next = NULL;
    funcRecord->outputList = sentinel->next;
    funcRecord->outputListSize = outputListSize;

    free(sentinel);
    return;
}

VAR_TYPE typeExtractor(ASTNode* exprNode, SymbolTableNode* symbolTableNode) {
    // We are entering the function for the first time
    if (strcmp(exprNode->label, "EXPR") == 0) {
        // If unary operator
        if (exprNode->numChildren == 2) {
            VAR_TYPE rhsType = typeExtractor(exprNode->rightMostChild, symbolTableNode);
            // Unary operations are not allowed on arrays
            if (rhsType == ARR) {
                return ERROR;
            } else {
                return rhsType;
            }
        } else {
            return typeExtractor(exprNode->leftMostChild, symbolTableNode);
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

        return varRecord->type.array.arrType;
    }
    
    // If not array
    switch (exprNode->leaf.tok->tok) {
        case PLUS:
        case MINUS:
        case MUL: {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

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
            return INT;
            break;
        }
        case RNUM: {
            return DOUBLE;
            break;
        }
        case TRUE:
        case FALSE: {
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
                return varRecord->type.varType;
            }
            break;
        }
        default: {
            // Should never reach here
            return ERROR;
            break;
        }
    }
    
}

void populateSymbolTable(SymbolTableNode* symbolTableNode, ASTNode* statement) {
    /*
        Types of statements possible:
        1. GET_VALUE                    Use
        2. PRINT                        Use
        3. ASSIGN_STMT                  Use
        4. MODULE_REUSE_STMT            Use
        5. DECLARE                      Populate
        6. SWITCH                       New and Use
        7. FOR                          New and Populate and Use
        8. WHILE                        New and Populate and Use
    */
    SymbolTableNode* sentinel = initSymbolTableNode();
    SymbolTableNode* currChild = sentinel;
    while (statement != NULL) {
        switch (statement->label[0]) {
            case 'G': { // GET_VALUE
                ASTNode* idNode = statement->leftMostChild;
                char* name = idNode->leaf.tok->lexeme;

                // Check if variable exists
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, idNode->leaf.tok->linenum);
                } else if (varRecord->iterator) {
                    printf(RED BOLD "[Semantic Analyser] Cannot get value of iterator %s at line %d\n" RESET, name, idNode->leaf.tok->linenum);
                }
                break;
            }
            case 'P': { // PRINT
                ASTNode* printNode = statement->leftMostChild;
                if (strcmp(printNode->label, "ID") == 0) { // ID
                    char* name = printNode->leaf.tok->lexeme;
                    Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                    if (varRecord == NULL) {
                        printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, printNode->leaf.tok->linenum);
                    }

                    if (printNode->next == NULL) { break; }

                    // Case where there is a subscript
                    ASTNode* indexNode = printNode->next;
                    if (varRecord->type.varType != ARR) {
                        printf(RED BOLD "[Semantic Analyser] Variable %s is not an array at line %d\n" RESET, name, printNode->leaf.tok->linenum);
                    } else {
                        // If array is static, check if the index is within bounds
                        if (!varRecord->type.array.isLeftID && !varRecord->type.array.isRightID && strcmp(indexNode->rightMostChild->label, "NUM") == 0) {
                            int left = varRecord->type.array.left;
                            if (varRecord->type.array.leftNegative) {
                                left = -left;
                            }
                            int right = varRecord->type.array.right;;
                            if (varRecord->type.array.rightNegative) {
                                right = -right;
                            }
                            int index = indexNode->rightMostChild->leaf.tok->num;
                            // If MINUS is present, the index is negative
                            if (indexNode->numChildren == 2) {
                                if (indexNode->leftMostChild->leaf.tok->tok == MINUS) {
                                    index = -index;
                                }
                            }
                            if (index < left || index > right) {
                                printf(RED BOLD "[Semantic Analyser] Index %d out of bounds for array %s at line %d\n" RESET, index, name, indexNode->leftMostChild->leaf.tok->linenum);
                            }
                        } else if (strcmp(indexNode->rightMostChild->label, "ID") == 0) {
                            // Checking for existence of the index variable and the type
                            char* indexName = indexNode->rightMostChild->leaf.tok->lexeme;
                            Record* indexRecord = variableExists(symbolTableNode, indexName, hash(indexName));
                            if (indexRecord == NULL || strcmp(indexRecord->name, indexName) != 0) {
                                printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, indexName, indexNode->leaf.tok->linenum);
                            } else if (indexRecord->type.varType != INTEGER) {
                                printf(RED BOLD "[Semantic Analyser] Index %s is not an integer at line %d\n" RESET, indexName, indexNode->leaf.tok->linenum);
                            }
                        }
                    }
                } 
                break;
            }
            case 'A': { // ASSIGN_STMT
                // Check whether the LHS is defined
                ASTNode* idNode = statement->leftMostChild;
                bool arrayAccess = !(strcmp(statement->leftMostChild->label, "ID") == 0);
                if (arrayAccess) {
                    idNode = statement->leftMostChild->leftMostChild;
                }

                // Check existence of idNode and store type
                VAR_TYPE idType;
                char* name = idNode->leaf.tok->lexeme;
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, idNode->leaf.tok->linenum);
                    break;
                } else {
                    idType = varRecord->type.varType;
                }

                if (varRecord->iterator) {
                    printf(RED BOLD "[Semantic Analyser] Cannot assign to iterator %s at line %d\n" RESET, name, idNode->leaf.tok->linenum);
                }

                switch (idType) {
                    case INT: {
                        // Check if RHS is an integer
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != INTEGER && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected INTEGER type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                        }
                        break;
                    }
                    case DOUBLE: {
                        // Check if RHS is a double
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != DOUBLE && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected REAL type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                        }
                        break;
                    }
                    case BOOL: {
                        // Check if RHS is a boolean
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != BOOLEAN && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected BOOLEAN type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                        }
                        break;
                    }
                    case ARR: {
                        if (arrayAccess) {
                            VAR_TYPE arrayType = varRecord->type.array.arrType;
                            char *typeStrings[] = {
                                "INTEGER",
                                "REAL",
                                "BOOLEAN"
                            };
                            // Check if RHS type matches the array type
                            VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                            if (rhsType != arrayType && rhsType != ERROR) {
                                printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected %s on the RHS.\n" RESET, idNode->leaf.tok->linenum, typeStrings[arrayType]);
                            }
                        } else {
                            // Check if RHS is an array
                            VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                            if (rhsType != ARR && rhsType != ERROR) {
                                printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected ARRAY type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                            } else if (rhsType == ARR) {
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }
                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                // Check if the module exists
                ASTNode* outputListNode = statement->leftMostChild;
                ASTNode* moduleNode = outputListNode->next;
                ASTNode* inputListNode = moduleNode->next;
                char* moduleName = moduleNode->leaf.tok->lexeme;
                GlobalRecord* moduleRecord = moduleExists(moduleName, hash(moduleName));
                if (moduleRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Undefined module %s at line %d\n" RESET, moduleName, moduleNode->leaf.tok->linenum);
                    break;
                }

                // Check if the module has been declared or defined
                if (!moduleRecord->declared && !moduleRecord->defined) {
                    printf(RED BOLD "[Semantic Analyser] Module %s has not been declared or defined yet at line %d" RESET, moduleName, moduleNode->leaf.tok->linenum);
                }

                // Check for recursion
                if (moduleRecord->called) {
                    printf(RED BOLD "[Semantic Analyser] Recursion is not permitted at line %d.\n" RESET, moduleNode->leaf.tok->linenum);
                    break;
                }

                // Checking if the module declaration was redundant or not
                // Module is redundant if it has been both declared and defined when called for the first time
                if (moduleRecord->checkedRedundancy == false) {
                    if (moduleRecord->declared && moduleRecord->defined) {
                        printf(RED BOLD "[Semantic Analyser] Redundant declaration of module %s.\n" RESET, moduleName);
                    }
                    moduleRecord->checkedRedundancy = true;
                }

                char *typeStrings[] = {
                    "INTEGER",
                    "REAL",
                    "BOOLEAN",
                    "ARRAY"
                };

                // Check if the input parameters match
                Record* inputNode = moduleRecord->inputList;
                ASTNode* curr = inputListNode->leftMostChild;
                while (curr != NULL) {

                    // Check if it is a MINUS_NODE
                    bool isMinus = false;
                    if (strcmp(curr->label, "MINUS_NODE") == 0) {
                        isMinus = true;
                        curr = curr->leftMostChild;
                    }

                    // Check if the variable exists if ID or ARRAY
                    Record* varRecord = NULL;
                    if (strcmp(curr->label, "ID") == 0) {
                        char* name = curr->leaf.tok->lexeme;
                        varRecord = variableExists(symbolTableNode, name, hash(name));
                        if (varRecord == NULL) {
                            printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, curr->leaf.tok->linenum);
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            inputNode = inputNode->next;
                            continue;
                        }
                    }
                    
                    if (inputNode == NULL) {
                        printf(RED BOLD "[Semantic Analyser] Too many input parameters for module %s at line %d\n" RESET, moduleName, curr->leaf.tok->linenum);
                        break;
                    }

                    VAR_TYPE inputType = typeExtractor(curr, symbolTableNode);
                    if (inputNode->type.varType == ARRAY) {
                        if (inputType != ARRAY) {
                            printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected ARRAY type.\n" RESET, moduleNode->leaf.tok->linenum);
                        } else if (inputType == ARRAY && isMinus) {
                            printf(RED BOLD "[Semantic Analyser] Unary minus operation not allowed on array %s at line %d.\n" RESET, inputNode->name, curr->leaf.tok->linenum);
                        } else {
                            // Check if the array types match
                            VAR_TYPE arrayType = inputNode->type.array.arrType; 
                            VAR_TYPE inputArrayType = varRecord->type.array.arrType;

                            if (arrayType != inputArrayType) {
                                printf(RED BOLD "[Semantic Analyser] Array type mismatch at line %d. Expected array of %s type.\n" RESET, moduleNode->leaf.tok->linenum, typeStrings[arrayType]);
                            }
                            
                            // Check if the array dimensions match if both are static
                            bool inputNodeIsStatic = !inputNode->type.array.isLeftID && !inputNode->type.array.isRightID;
                            bool varRecordIsStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;

                            if (inputNodeIsStatic && varRecordIsStatic) {
                                int inputNodeLeft = inputNode->type.array.left;
                                int inputNodeRight = inputNode->type.array.right;
                                int varRecordLeft = varRecord->type.array.left;
                                int varRecordRight = varRecord->type.array.right;

                                if (inputNodeLeft != varRecordLeft || inputNodeRight != varRecordRight) {
                                    printf(RED BOLD "[Semantic Analyser] Array dimensions mismatch at line %d. Expected array of [%d..%d].\n" RESET, moduleNode->leaf.tok->linenum, inputNodeLeft, inputNodeRight);
                                }
                            }
                        }
                    } else if (inputType != inputNode->type.varType && inputType != ERROR) {
                        printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected %s type.\n" RESET, moduleNode->leaf.tok->linenum, typeStrings[inputNode->type.varType]);
                    }

                    if (isMinus) {
                        curr = curr->parent;
                    }
                    curr = curr->next;
                    inputNode = inputNode->next;
                }

                if (inputNode != NULL) {
                    printf(RED BOLD "[Semantic Analyser] Too few input parameters for module %s at line %d\n" RESET, moduleName, moduleNode->leaf.tok->linenum);
                }


                // Check if the output parameters match
                Record* outputNode = moduleRecord->outputList;
                curr = outputListNode->leftMostChild;
                while (curr != NULL) {
                    // Check if it is a MINUS_NODE
                    bool isMinus = false;
                    if (strcmp(curr->label, "MINUS_NODE") == 0) {
                        isMinus = true;
                        curr = curr->leftMostChild;
                    }

                    // Check if the variable exists if ID or ARRAY
                    Record* varRecord = NULL;
                    if (strcmp(curr->label, "ID") == 0) {
                        char* name = curr->leaf.tok->lexeme;
                        varRecord = variableExists(symbolTableNode, name, hash(name));
                        if (varRecord == NULL) {
                            printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, curr->leaf.tok->linenum);
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            outputNode = outputNode->next;
                            continue;
                        } else if (varRecord->iterator) {
                            printf(RED BOLD "[Semantic Analyser] Iterator variable %s cannot be used as output parameter at line %d\n" RESET, name, curr->leaf.tok->linenum);
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            outputNode = outputNode->next;
                            continue;
                        }
                    }

                    if (outputNode == NULL) {
                        printf(RED BOLD "[Semantic Analyser] Too many output parameters for module %s at line %d\n" RESET, moduleName, curr->leaf.tok->linenum);
                        break;
                    }

                    VAR_TYPE outputType = typeExtractor(curr, symbolTableNode);
                    if (outputNode->type.varType == ARRAY) {
                        if (outputType != ARRAY) {
                            printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected ARRAY type.\n" RESET, moduleNode->leaf.tok->linenum);
                        } else if (outputType == ARRAY && isMinus) {
                            printf(RED BOLD "[Semantic Analyser] Unary minus operation not allowed on array %s at line %d.\n" RESET, outputNode->name, curr->leaf.tok->linenum);
                        } else {
                            // Check if the array types match
                            VAR_TYPE arrayType = outputNode->type.array.arrType; 
                            VAR_TYPE outputArrayType = varRecord->type.array.arrType;

                            if (arrayType != outputArrayType) {
                                printf(RED BOLD "[Semantic Analyser] Array type mismatch at line %d. Expected array of %s type.\n" RESET, moduleNode->leaf.tok->linenum, typeStrings[arrayType]);
                            }
                            
                            // Check if the array dimensions match if both are static
                            bool outputNodeIsStatic = !outputNode->type.array.isLeftID && !outputNode->type.array.isRightID;
                            bool varRecordIsStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;

                            if (outputNodeIsStatic && varRecordIsStatic) {
                                int outputNodeLeft = outputNode->type.array.left;
                                int outputNodeRight = outputNode->type.array.right;
                                int varRecordLeft = varRecord->type.array.left;
                                int varRecordRight = varRecord->type.array.right;

                                if (outputNodeLeft != varRecordLeft || outputNodeRight != varRecordRight) {
                                    printf(RED BOLD "[Semantic Analyser] Array dimensions mismatch at line %d. Expected array of [%d..%d].\n" RESET, moduleNode->leaf.tok->linenum, outputNodeLeft, outputNodeRight);
                                }
                            }
                        }
                    } else if (outputType != outputNode->type.varType && outputType != ERROR) {
                        printf(RED BOLD "[Semantic Analyser] Type mismatch at line %d. Expected %s type.\n" RESET, moduleNode->leaf.tok->linenum, typeStrings[outputNode->type.varType]);
                    }

                    if (isMinus) {
                        curr = curr->parent;
                    }
                    curr = curr->next;
                    outputNode = outputNode->next;
                }

                if (outputNode != NULL) {
                    printf(RED BOLD "[Semantic Analyser] Too few output parameters for module %s at line %d\n" RESET, moduleName, moduleNode->leaf.tok->linenum);
                }
                break;
            }
            case 'D': { // DECLARE_STMT
                ASTNode* assignListNode = statement->leftMostChild;
                ASTNode* dataTypeNode = statement->rightMostChild;

                ASTNode* curr = assignListNode->leftMostChild;
                while (curr != NULL) {
                    // Checking if already exists
                    char* name = curr->leaf.tok->lexeme;
                    unsigned int hashVal = hash(name);

                    // In outputList
                    if (variableExists(symbolTableNode->funcOutputST, name, hashVal) != NULL) {
                        printf(RED BOLD "[Semantic Analyser] Redeclaration of output variable %s at line %d\n" RESET, name, curr->leaf.tok->linenum);
                        curr = curr->next;
                        continue;
                    }

                    // Otherwise, check in current symbol table
                    Record* varRecord = findVariableInsertion(symbolTableNode, name, hashVal);
                    if (varRecord == NULL) {
                        varRecord = generateRecord(symbolTableNode, curr, dataTypeNode, &symbolTableNode->nextOffset);
                        symbolTableNode->hashTable[hashVal] = varRecord;
                    } else if (strcmp(varRecord->name, name) == 0) {
                        printf(RED BOLD "[Semantic Analyser] Redeclaration of variable %s at line %d\n" RESET, name, curr->leaf.tok->linenum);
                    } else {
                        varRecord->next = generateRecord(symbolTableNode, curr, dataTypeNode, &symbolTableNode->nextOffset);
                    }
                    
                    curr = curr->next;
                }
                break;
            }
            case 'S': { // SWITCH
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;
                currChild->funcOutputST = symbolTableNode->funcOutputST;

                // ID in SWITCH
                ASTNode* idNode = statement->leftMostChild;
                VAR_TYPE switchType;
                char* name = idNode->leaf.tok->lexeme;
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord != NULL) {
                    switchType = varRecord->type.varType;
                } else {
                    printf(RED BOLD "[Semantic Analyser] Undefined variable %s at line %d\n" RESET, name, idNode->leaf.tok->linenum);
                }

                switch (switchType) {
                    case INT: {
                        // Checking if default statement exists
                        ASTNode* defaultCase = statement->rightMostChild;
                        if (statement->rightMostChild->leaf.tok->tok != DEFAULT) {
                            printf(RED BOLD "[Semantic Analyser] Default statement required for switch statement of type INTEGER at line %d\n" RESET, idNode->leaf.tok->linenum);
                            defaultCase = NULL;
                        }

                        // Iterating through all the case statements
                        ASTNode* caseStatements = statement->leftMostChild->next;
                        while (caseStatements != NULL) {
                            // Checking if the case statement is of type INTEGER
                            if (caseStatements != defaultCase && caseStatements->leftMostChild->leaf.tok->tok != NUM) {
                                printf(RED BOLD "[Semantic Analyser] Case label not of type INTEGER at line %d\n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                            }

                            // Populating the symbol table for the statements in the case
                            if (caseStatements == defaultCase) {
                                populateSymbolTable(currChild, caseStatements->leftMostChild);
                            } else {
                                populateSymbolTable(currChild, caseStatements->leftMostChild->next);
                            }
                            caseStatements = caseStatements->next;
                        }
                        break;
                    }
                    case DOUBLE: {
                        printf(RED BOLD "[Semantic Analyser] REAL type identifier not allowed in switch statement at line %d\n" RESET, idNode->leaf.tok->linenum);
                        break;
                    }
                    case BOOL: {
                        // Checking that there is no default statement
                        if (statement->rightMostChild->leaf.tok->tok == DEFAULT) {
                            printf(RED BOLD "[Semantic Analyser] Default statement not allowed in switch statement of type BOOLEAN at line %d\n" RESET, idNode->leaf.tok->linenum);
                        }

                        // Iterating through all the case statements
                        ASTNode* caseStatements = statement->leftMostChild->next;
                        while (caseStatements != NULL) {
                            // Checking if the case statement is of type BOOLEAN
                            if (caseStatements->leftMostChild->leaf.tok->tok != TRUE && caseStatements->leftMostChild->leaf.tok->tok != FALSE) {
                                printf(RED BOLD "[Semantic Analyser] Case label not of type BOOLEAN at line %d\n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                            }
                            // Populating the symbol table for the statements in the case
                            populateSymbolTable(currChild, caseStatements->leftMostChild->next);
                            caseStatements = caseStatements->next;
                        }
                        break;
                    }
                    case ARR: {
                        printf(RED BOLD "[Semantic Analyser] ARRAY type identifier not allowed in switch statement at line %d\n" RESET, idNode->leaf.tok->linenum);
                        break;
                    }
                    default: {
                        // Should never reach here
                        break;
                    }
                }

                break;
            }
            case 'F': { // FOR
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;
                currChild->funcOutputST = symbolTableNode->funcOutputST;

                // ID IN FOR LOOP 
                ASTNode* idNode = statement->leftMostChild; 
                char* name = idNode->leaf.tok->lexeme;
                unsigned int hashVal = hash(name);
                Record* varRecord = malloc(sizeof(Record));
                currChild->hashTable[hashVal] = varRecord;
                strcpy(varRecord->name, name);
                varRecord->iterator = true;
                varRecord->offset = currChild->nextOffset;
                currChild->nextOffset += sizeof(int);
                // varRecord->linenum = idNode->leaf.tok->linenum;
                varRecord->type.varType = INT;
                varRecord->next = NULL;

                // Statements inside the for loop
                populateSymbolTable(currChild, statement->leftMostChild->next->next);
                break;
            }
            case 'W': { // WHILE
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;
                currChild->funcOutputST = symbolTableNode->funcOutputST;

                // Check whether the expression is a boolean expression or not
                ASTNode* exprNode = statement->leftMostChild;
                VAR_TYPE exprType = typeExtractor(exprNode, symbolTableNode);

                if (exprType != BOOL) {
                    printf(RED BOLD "[Semantic Analyser] Expression in while loop not of type BOOLEAN at line %d\n" RESET, exprNode->leaf.tok->linenum);
                }

                // Statements inside the while loop
                populateSymbolTable(currChild, statement->leftMostChild->next);
                break;
            }
        }
        statement = statement->next;
    }

    currChild->next = NULL;
    symbolTableNode->children = sentinel->next;
    free(sentinel);
    return;
}

void addModuleDeclarationToSymbolTable(ASTNode* moduleDeclarationNode) {
    char* name = moduleDeclarationNode->leaf.tok->lexeme;
    GlobalRecord* funcRecord = moduleExists(name, hash(name));

    if (funcRecord == NULL) {
        printf(RED BOLD "[Semantic Analyser] Module %s declared but never defined\n" RESET, name);
        return;
    }

    if (funcRecord->declared) {
        printf(RED BOLD "[Semantic Analyser] Module %s redeclared at line %d\n" RESET, name, moduleDeclarationNode->leftMostChild->leaf.tok->linenum);
        return;
    } else {
        funcRecord->declared = true;
    }

    return;
}

void addFunctionToSymbolTable(ASTNode* moduleNode) {
    bool driver = (strcmp(moduleNode->label, "DRIVER") == 0);
    char* name;
    if (driver) {
        name = "DRIVER";
    } else {
        name = moduleNode->leftMostChild->leaf.tok->lexeme;
    }
    GlobalRecord* funcRecord = moduleExists(name, hash(name));

    if (funcRecord->defined) { return; }
    funcRecord->defined = true;
    ASTNode* statementsNode;
    if (driver) {
        statementsNode = moduleNode->leftMostChild->leftMostChild;
    } else {
        statementsNode = moduleNode->rightMostChild->leftMostChild;
    }

    funcRecord->called = true;
    populateSymbolTable(funcRecord->funcST, statementsNode);
    funcRecord->called = false;

    return;
}

void addModuleSignatureToSymbolTable(ASTNode* moduleSignatureNode) {
    bool driver = (strcmp(moduleSignatureNode->label, "DRIVER") == 0);
    char* name;
    if (driver) {
        name = "DRIVER";
    } else {
        name = moduleSignatureNode->leftMostChild->leaf.tok->lexeme;
    }
    unsigned int hashVal = hash(name);

    GlobalRecord* funcRecord;
    if (symbolTable->global[hashVal] != NULL) {
        funcRecord = findFunction(name, hashVal);
        if (strcmp(funcRecord->name, name) == 0) {
            printf(RED BOLD "[Semantic Analyser] Redeclaration of module %s at line %d.\n" RESET, name, moduleSignatureNode->leftMostChild->leaf.tok->linenum);
            return;
        } else {
            funcRecord->next = malloc(sizeof(GlobalRecord));
            funcRecord = funcRecord->next;
        }
    } else {
        funcRecord = malloc(sizeof(GlobalRecord));
        symbolTable->global[hashVal] = funcRecord;
    }

    strcpy(funcRecord->name, name);
    funcRecord->linenum = moduleSignatureNode->leaf.tok->linenum;
    funcRecord->called = false;
    funcRecord->declared = false;
    funcRecord->defined = false;
    funcRecord->driver = driver;
    funcRecord->funcST = initSymbolTableNode();
    funcRecord->outputST = initSymbolTableNode();
    funcRecord->funcST->funcOutputST = funcRecord->outputST;
    funcRecord->inputList = NULL;
    funcRecord->outputList = NULL;
    funcRecord->inputListSize = 0;
    funcRecord->outputListSize = 0;
    funcRecord->next = NULL;

    if (!driver) {
        ASTNode* inputList = moduleSignatureNode->leftMostChild->next;
        ASTNode* outputList = inputList->next;
        populateInputOutputList(funcRecord, inputList, outputList);
    }
    return;
}

void printSymbolTableRec(SymbolTableNode* symbolTableNode) {
    if (symbolTableNode == NULL) {
        return;
    }
    
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTableNode->hashTable[i] == NULL) { continue; }
        Record* varRecord = symbolTableNode->hashTable[i];
        while (varRecord != NULL) {
            printf("%-5dVariable: %-7s\tOffset: %5d\t", i, varRecord->name, varRecord->offset);
            switch (varRecord->type.varType) {
                case INT: {
                    printf("INTEGER\n");
                    break;
                }
                case DOUBLE: {
                    printf("REAL\n");
                    break;
                }
                case BOOL: {
                    printf("BOOLEAN\n");
                    break;
                }
                case ARR: {
                    char* typeStrings[] = {
                        "INTEGER",
                        "REAL",
                        "BOOLEAN"
                    };
                    printf("ARRAY\t%s\t", typeStrings[varRecord->type.array.arrType]);

                    if (varRecord->type.array.leftNegative) {
                        printf("-");
                    }

                    if (varRecord->type.array.isLeftID) {
                        printf("%s\t", varRecord->type.array.leftID);
                    } else {
                        printf("%d\t", varRecord->type.array.left);
                    }

                    if (varRecord->type.array.rightNegative) {
                        printf("-");
                    }

                    if (varRecord->type.array.isRightID) {
                        printf("%s\n", varRecord->type.array.rightID);
                    } else {
                        printf("%d\n", varRecord->type.array.right);
                    }
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }
            varRecord = varRecord->next;
        }
    }

    if (symbolTableNode->children == NULL) {
        return;
    }

    printf("Printing Children\n");
    SymbolTableNode* child = symbolTableNode->children;
    while (child != NULL) {
        printSymbolTableRec(child);
        child = child->next;
    }
    printf("End of Children\n");
    printf("-------------------------------------------------------------------------------------\n");

    return;
}

void printSymbolTable(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTable->global[i] == NULL) { continue; }
        GlobalRecord* funcRecord = symbolTable->global[i];
        while (funcRecord != NULL) {
            printf("%-5dFunction: %s\n", i, funcRecord->name);
            printSymbolTableRec(funcRecord->outputST);
            printSymbolTableRec(funcRecord->funcST);
            funcRecord = funcRecord->next;
            printf("-------------------------------------------------------------------------------------\n");
        }
    }

    return;
}

void generateSymbolTable(AST* ast) {
    symbolTable = initSymbolTable();

    // Adding all the modules signatures to the symbol table
    ASTNode* module = ast->root->leftMostChild->next;
    while (module != NULL) {
        addModuleSignatureToSymbolTable(module);
        module = module->next;
    }

    // Adding module declarations;
    ASTNode* moduleDeclarations = ast->root->leftMostChild->leftMostChild;
    while (moduleDeclarations != NULL) {
        addModuleDeclarationToSymbolTable(moduleDeclarations);
        moduleDeclarations = moduleDeclarations->next;
    }

    // Adding all the functions recursively
    ASTNode* functions = ast->root->leftMostChild->next;
    while (functions != NULL) {
        addFunctionToSymbolTable(functions);
        functions = functions->next;
    }

    // Checking for modules declared but not defined
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTable->global[i] == NULL) { continue; }
        GlobalRecord* funcRecord = symbolTable->global[i];
        while (funcRecord != NULL) {
            if (funcRecord->defined == false) {
                printf("Module %s declared but not defined.\n", funcRecord->name);
            }
            funcRecord = funcRecord->next;
        }
    }

    return;
}