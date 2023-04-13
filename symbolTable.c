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

#define max(a, b) ((a) > (b) ? (a) : (b))

SymbolTable* symbolTable;
bool SEMANTIC_ERROR = false;

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
    newNode->scopeStart = 0;
    newNode->scopeEnd = 0;
    newNode->nextOffset = 0;
    newNode->nestingLevel = 0;
    newNode->funcInputST = NULL;
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
        return variableExists(symbolTableNode->funcInputST, name, hashVal);
    } else {
        return variableExists(symbolTableNode->parent, name, hashVal);
    }
}

Record* generateRecord(SymbolTableNode* symbolTableNode, ASTNode* idNode, ASTNode* dataTypeNode, unsigned int* nextOffset, bool parameterVariable) {
    char* name = idNode->leaf.tok->lexeme;
    Record* newRec = malloc(sizeof(Record));
    strcpy(newRec->name, name);
    newRec->iterator = false;
    newRec->assigned = 0;
    newRec->offset = *nextOffset;
    newRec->next = NULL;

    switch (dataTypeNode->leaf.tok->tok) {
        case INTEGER: {
            newRec->type.varType = INT;
            newRec->width = SIZEOF_INT;
            *nextOffset += SIZEOF_INT;
            break;
        }
        case REAL: {
            newRec->type.varType = DOUBLE;
            newRec->width = SIZEOF_REAL;
            *nextOffset += SIZEOF_REAL;
            break;
        }
        case BOOLEAN: {
            newRec->type.varType = BOOL;
            newRec->width = SIZEOF_BOOL;
            *nextOffset += SIZEOF_BOOL;
            break;
        }
        case ARRAY: {
            newRec->type.varType = ARR;
            int elementSize;
            
            // Setting the datatype of the elements of the array
            switch (dataTypeNode->rightMostChild->leaf.tok->tok) {
                case INTEGER: {
                    newRec->type.array.arrType = INT;
                    elementSize = SIZEOF_INT;
                    break;
                }
                case REAL: {
                    newRec->type.array.arrType = DOUBLE;
                    elementSize = SIZEOF_REAL;
                    break;
                }
                case BOOLEAN: {
                    newRec->type.array.arrType = BOOL;
                    elementSize = SIZEOF_BOOL;
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

            // The static flag of the ast node itself (required d6433b4dcdee72uring code generation (maybe))
            bool isStatic = !(newRec->type.array.isLeftID || newRec->type.array.isRightID);

            // Calculating next offset
            if (isStatic) {
                int left = newRec->type.array.left * (newRec->type.array.leftNegative ? -1 : 1);
                int right = newRec->type.array.right * (newRec->type.array.rightNegative ? -1 : 1);

                if (left > right) {
                    printf(RED BOLD "[Semantic Analyser] Specified subrange for array \"%s\" are illegal. (%d > %d)\n" RESET, name, left, right);
                    SEMANTIC_ERROR = true;
                    return NULL;
                }

                if (parameterVariable) {
                    newRec->width = sizeof(void*) + 2 * SIZEOF_INT;
                    *nextOffset += newRec->width;
                } else {
                    newRec->width = sizeof(void*)  + elementSize * (right - left + 1);
                    *nextOffset += newRec->width;
                }
            } else {
                // Adding the size of pointer
                newRec->width = sizeof(void*);
                *nextOffset += sizeof(void*);
                // Check whether both the dynamic bounds have been declared and are of integer type
                if (newRec->type.array.isLeftID) {
                    char* leftID = newRec->type.array.leftID;
                    Record* leftRec = variableExists(symbolTableNode, leftID, hash(leftID));
                    if (leftRec == NULL) {
                        printf(RED BOLD "[Semantic Analyser] The left bound of array \"%s\" is not declared.\n" RESET, name);
                        SEMANTIC_ERROR = true;
                    } else if (leftRec->type.varType != INT) {
                        printf(RED BOLD "[Semantic Analyser] The left bound of array \"%s\" is not of type integer.\n" RESET, name);
                        SEMANTIC_ERROR = true;
                    }
                }

                if (newRec->type.array.isRightID) {
                    char* rightID = newRec->type.array.rightID;
                    Record* rightRec = variableExists(symbolTableNode, rightID, hash(rightID));
                    if (rightRec == NULL) {
                        printf(RED BOLD "[Semantic Analyser] The right bound of array \"%s\" is not declared.\n" RESET, name);
                       SEMANTIC_ERROR = true;
                    } else if (rightRec->type.varType != INT) {
                        printf(RED BOLD "[Semantic Analyser] The right bound of array \"%s\" is not of type integer.\n" RESET, name);
                        SEMANTIC_ERROR = true;
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
    SymbolTableNode* inputST = funcRecord->inputST;
    SymbolTableNode* outputST = funcRecord->outputST;

    // Creating the inputList
    int inputListSize = 0;
    Record* sentinel = malloc(sizeof(Record));
    sentinel->next = NULL;
    Record* curr = sentinel;
    ASTNode* inputNode = inputList->leftMostChild;
    while (inputNode != NULL) {
        // Adding the input variable to the inputList
        Record* newRecord = generateRecord(inputST, inputNode->leftMostChild, inputNode->rightMostChild, offset, true);
        if (newRecord == NULL) { 
            funcRecord->error = true;
            inputNode = inputNode->next;
            continue; 
        }
        curr->next = newRecord;
        curr = curr->next;
        inputListSize++;

        // Adding the input variable to the symbolTableNode
        char* name = newRecord->name;
        unsigned int hashVal = hash(name);
        Record* varRecord = findVariableInsertion(inputST, name, hashVal);
        int linenum = inputNode->leftMostChild->leaf.tok->linenum;
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
            inputST->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf(RED BOLD "[Semantic Analyser] Line %d: Redeclaration of variable %s in input list of module %s\n" RESET, linenum, name, funcRecord->name);
            SEMANTIC_ERROR = true;
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
        Record* newRecord = generateRecord(symbolTableNode, outputNode->leftMostChild, outputNode->rightMostChild, offset, true);
        if (newRecord == NULL) { 
            funcRecord->error = true;
            outputNode = outputNode->next;
            continue; 
        }
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
            printf(RED BOLD "[Semantic Analyser] Line %d: Redeclaration of variable %s in output list of module %s\n" RESET, outputNode->leftMostChild->leaf.tok->linenum, name, funcRecord->name);
            SEMANTIC_ERROR = true;
            outputNode = outputNode->next;
            continue;
        }

        varRecord = findVariableInsertion(outputST, name, hashVal);
        int linenum = outputNode->leftMostChild->leaf.tok->linenum;
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, newRecord, sizeof(Record));
            varRecord->next = NULL;
            outputST->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf(RED BOLD "[Semantic Analyser] Line %d: Redeclaration of variable %s in output list of module %s\n" RESET, linenum, name, funcRecord->name);
            SEMANTIC_ERROR = true;
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
            exprNode->type = rhsType;
            // Unary operations are not allowed on arrays
            if (rhsType == ARR) {
                printf(RED BOLD "[Semantic Analyser] Line %d: Unary operations are not allowed on arrays\n" RESET, exprNode->rightMostChild->leaf.tok->linenum);
                SEMANTIC_ERROR = true;
                return ERROR;
            } else {
                return rhsType;
            }
        } else {
            VAR_TYPE rhsType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            exprNode->type = rhsType;
            return rhsType;
        }
    }

    // If array (since there is no token in that case)
    if (strcmp(exprNode->label, "ARRAY") == 0) {
        char* name = exprNode->leftMostChild->leaf.tok->lexeme;
        unsigned int hashVal = hash(name);
        Record* varRecord = variableExists(symbolTableNode, name, hashVal);
        if (varRecord == NULL) {
            printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s.\n" RESET, exprNode->leftMostChild->leaf.tok->linenum, name);
            exprNode->type = ERROR;
            SEMANTIC_ERROR = true;
            return ERROR;
        }

        // Type checking for the index
        ASTNode* indexNode = exprNode->rightMostChild;
        VAR_TYPE indexType = typeExtractor(indexNode->rightMostChild, symbolTableNode);
        if (indexType != INT) {
            printf(RED BOLD "[Semantic Analyser] Line %d: Array index must be of type INTEGER.\n" RESET, exprNode->leftMostChild->leaf.tok->linenum);
            indexNode->type = ERROR;
            SEMANTIC_ERROR = true;
            return ERROR;
        }

        indexNode->type = INT;

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
                    printf(RED BOLD "[Semantic Analyser] Line %d: Index %d is out of bounds for array %s.\n" RESET, indexNode->rightMostChild->leaf.tok->linenum, index, name);
                    SEMANTIC_ERROR = true;
                }
            }
        }

        exprNode->type = varRecord->type.array.arrType;
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
                return exprNode->type = INT;
            } else if (leftType == DOUBLE && rightType == DOUBLE) {
                return exprNode->type = DOUBLE;
            } else if (leftType == ERROR || rightType == ERROR) {
                return exprNode->type = ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Line %d: Type Mismatch. Invalid types for %s.\n" RESET, exprNode->leaf.tok->linenum, token_types[exprNode->leaf.tok->tok]);
                SEMANTIC_ERROR = true;
                return exprNode->type = ERROR;
            }
            break;
        }
        case DIV: {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            if ((leftType == INT || leftType == DOUBLE) && (rightType == INT || rightType == DOUBLE)) {
                return exprNode->type = DOUBLE;
            } else if (leftType == ERROR || rightType == ERROR) {
                return exprNode->type = ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Line %d: Type Mismatch. Invalid types for DIV.\n" RESET, exprNode->leaf.tok->linenum);
                SEMANTIC_ERROR = true;
                return exprNode->type = ERROR;
            }
            break;
        }
        case AND: 
        case OR: 
        {
            int leftType = typeExtractor(exprNode->leftMostChild, symbolTableNode);
            int rightType = typeExtractor(exprNode->rightMostChild, symbolTableNode);

            if (leftType == BOOL && rightType == BOOL) {
                return exprNode->type = BOOL;
            } else if (leftType == ERROR || rightType == ERROR) {
                return exprNode->type = ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Line %d: Type Mismatch. Invalid types for %s.\n" RESET, exprNode->leaf.tok->linenum, token_types[exprNode->leaf.tok->tok]);
                SEMANTIC_ERROR = true;
                return exprNode->type = ERROR;
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
                return exprNode->type = BOOL;
            } else if (leftType == DOUBLE && rightType == DOUBLE) {
                return exprNode->type = BOOL;
            } else if (leftType == ERROR || rightType == ERROR) {
                return exprNode->type = ERROR;
            } else {
                printf(RED BOLD "[Semantic Analyser] Line %d: Type Mismatch. Invalid types for %s.\n" RESET,exprNode->leaf.tok->linenum, token_types[exprNode->leaf.tok->tok]);
                SEMANTIC_ERROR = true;
                return exprNode->type = ERROR;
            }
            break;
        }
        case NUM: {
            return exprNode->type = INT;
            break;
        }
        case RNUM: {
            return exprNode->type = DOUBLE;
            break;
        }
        case TRUE:
        case FALSE: {
            return exprNode->type = BOOL;
            break;
        }
        case ID: {
            char* name = exprNode->leaf.tok->lexeme;
            unsigned int hashVal = hash(name);
            Record* varRecord = variableExists(symbolTableNode, name, hashVal);
            if (varRecord == NULL) {
                printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, exprNode->leaf.tok->linenum, name);
                SEMANTIC_ERROR = true;
                return exprNode->type = ERROR;
            } else {
                return exprNode->type = varRecord->type.varType;
            }
            break;
        }
        default: {
            // Should never reach here
            return exprNode->type = ERROR;
            break;
        }
    }
}

void createWhileExprIDListRec(RecordList* recordList, ASTNode* exprNode, SymbolTableNode* symbolTableNode) {
    if (exprNode == NULL) {
        return;
    }

    if (exprNode->numChildren == 2) {
        createWhileExprIDListRec(recordList, exprNode->leftMostChild, symbolTableNode);
        createWhileExprIDListRec(recordList, exprNode->rightMostChild, symbolTableNode);
    } else {
        if (strcmp(exprNode->label, "ID") == 0) {
            char* name = exprNode->leaf.tok->lexeme;
            Record* varRecord = variableExists(symbolTableNode, name, hash(name));
            if (varRecord == NULL) {
                return;
            }

            RecordListNode* newNode = malloc(sizeof(RecordListNode));
            newNode->record = varRecord;
            if (recordList->size == 0) {
                recordList->head = recordList->tail = newNode;
            } else {
                recordList->tail->next = newNode;
                recordList->tail = newNode;
            }
            recordList->size++;
        } else if (strcmp(exprNode->label, "ARRAY_INDEX") == 0) {
            char* name = exprNode->leftMostChild->leaf.tok->lexeme;
            Record* varRecord = variableExists(symbolTableNode, name, hash(name));
            if (varRecord == NULL) {
                return;
            }

            RecordListNode* newNode = malloc(sizeof(RecordListNode));
            newNode->record = varRecord;
            if (recordList->size == 0) {
                recordList->head = recordList->tail = newNode;
            } else {
                recordList->tail->next = newNode;
                recordList->tail = newNode;
            }
            recordList->size++;
        } else {
            return;
        }
    }

    return;
}

RecordList* createWhileExprIDList(ASTNode* exprNode, SymbolTableNode* symbolTableNode) {
    RecordList* recordList = malloc(sizeof(RecordList));
    recordList->head = recordList->tail = NULL;
    recordList->size = 0;

    createWhileExprIDListRec(recordList, exprNode, symbolTableNode);
    return recordList;
}

void populateSymbolTable(SymbolTableNode* symbolTableNode, ASTNode* statement, int level) {
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

    int lastLineNum = 0;
    while (statement != NULL) {
        switch (statement->label[0]) {
            case 'G': { // GET_VALUE
                ASTNode* idNode = statement->leftMostChild;
                char* name = idNode->leaf.tok->lexeme;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = idNode->leaf.tok->linenum;
                }
                lastLineNum = idNode->leaf.tok->linenum;

                // Check if variable exists
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                    break;
                } else if (varRecord->iterator) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Cannot get value of iterator %s \n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                    break;
                }

                varRecord->assigned++;
                break;
            }
            case 'P': { // PRINT
                ASTNode* printNode = statement->leftMostChild;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = printNode->leaf.tok->linenum;
                }
                lastLineNum = printNode->leaf.tok->linenum;

                if (strcmp(printNode->label, "ID") == 0) { // ID
                    char* name = printNode->leaf.tok->lexeme;
                    Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                    if (varRecord == NULL) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, printNode->leaf.tok->linenum, name);
                        SEMANTIC_ERROR = true;
                    }

                    if (printNode->next == NULL) { break; }

                    // Case where there is a subscript
                    ASTNode* indexNode = printNode->next;
                    if (varRecord->type.varType != ARR) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Variable %s is not an array\n" RESET, printNode->leaf.tok->linenum, name);
                        SEMANTIC_ERROR = true;
                    } else {
                        indexNode->type = INT;
                        indexNode->rightMostChild->type = INT;
                        
                        // If array is static, check if the index is within bounds
                        if (!varRecord->type.array.isLeftID && !varRecord->type.array.isRightID && strcmp(indexNode->rightMostChild->label, "NUM") == 0) {
                            int left = varRecord->type.array.left;
                            if (varRecord->type.array.leftNegative) {
                                left = -left;
                            }
                            int right = varRecord->type.array.right;
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
                                printf(RED BOLD "[Semantic Analyser] Line %d: Index %d is out of bounds for array %s.\n" RESET, indexNode->leftMostChild->leaf.tok->linenum, index, name);
                                SEMANTIC_ERROR = true;
                            }
                        } else if (strcmp(indexNode->rightMostChild->label, "ID") == 0) {
                            // Checking for existence of the index variable and the type
                            char* indexName = indexNode->rightMostChild->leaf.tok->lexeme;
                            Record* indexRecord = variableExists(symbolTableNode, indexName, hash(indexName));
                            if (indexRecord == NULL || strcmp(indexRecord->name, indexName) != 0) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, indexNode->rightMostChild->leaf.tok->linenum, indexName);
                                SEMANTIC_ERROR = true;
                            } else if (indexRecord->type.varType != INTEGER) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Index %s is not an integer\n" RESET, indexNode->leaf.tok->linenum, indexName);
                                SEMANTIC_ERROR = true;
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

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = idNode->leaf.tok->linenum;
                }
                lastLineNum = idNode->leaf.tok->linenum;

                // Check existence of idNode and store type
                VAR_TYPE idType;
                char* name = idNode->leaf.tok->lexeme;
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                } else {
                    idType = varRecord->type.varType;
                }

                if (varRecord != NULL && arrayAccess && idType != ARR) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Variable %s is not an array\n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                }

                if (varRecord != NULL && varRecord->iterator) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Cannot assign to iterator %s\n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                }

                if (varRecord == NULL) {
                    typeExtractor(statement->rightMostChild, symbolTableNode);
                    if (arrayAccess) {
                        ASTNode* indexNode = statement->leftMostChild->rightMostChild;
                        typeExtractor(indexNode->rightMostChild, symbolTableNode);
                    }

                    break;
                }

                switch (idType) {
                    case INT: {
                        // Check if RHS is an integer
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != INTEGER && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Type mismatch. Expected INTEGER type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
                        }
                        break;
                    }
                    case DOUBLE: {
                        // Check if RHS is a double
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != DOUBLE && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Type mismatch. Expected REAL type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
                        }
                        break;
                    }
                    case BOOL: {
                        // Check if RHS is a boolean
                        VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                        if (rhsType != BOOLEAN && rhsType != ERROR) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Type mismatch. Expected BOOLEAN type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
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
                                printf(RED BOLD "[Semantic Analyser] Line %d: Type mismatch. Expected %s on the RHS.\n" RESET, idNode->leaf.tok->linenum, typeStrings[arrayType]);
                                SEMANTIC_ERROR = true;
                            }

                            // Check if the index is within bounds
                            ASTNode* indexNode = statement->leftMostChild->rightMostChild;
                            VAR_TYPE indexType = typeExtractor(indexNode->rightMostChild, symbolTableNode);
                            if (indexType != INTEGER && indexType != ERROR) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Index is not an integer\n" RESET, indexNode->leaf.tok->linenum);
                                SEMANTIC_ERROR = true;
                            } else if (indexType == INTEGER) {
                                if (strcmp(indexNode->rightMostChild->label, "NUM") != 0) {
                                    break;
                                }

                                bool isStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;
                                if (!isStatic) { break; }

                                int index = indexNode->rightMostChild->leaf.tok->num;
                                int left = varRecord->type.array.left;
                                int right = varRecord->type.array.right;

                                if (indexNode->leftMostChild->leaf.tok->tok == MINUS) {
                                    index = -index;
                                }

                                if (varRecord->type.array.leftNegative) {
                                    left = -left;
                                }

                                if (varRecord->type.array.rightNegative) {
                                    right = -right;
                                }

                                if (index < left || index > right) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Index %d is out of bounds for array %s.\n" RESET, indexNode->leftMostChild->leaf.tok->linenum, index, name);
                                    SEMANTIC_ERROR = true;
                                }
                            }
                        } else {
                            // Check if RHS is an array
                            VAR_TYPE rhsType = typeExtractor(statement->rightMostChild, symbolTableNode);
                            if (rhsType != ARR && rhsType != ERROR) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Type mismatch. Expected ARRAY type on the RHS.\n" RESET, idNode->leaf.tok->linenum);
                                SEMANTIC_ERROR = true;
                                break;
                            } else if (rhsType == ERROR) {
                                break;
                            }

                            // Check if the dimensions match if both static
                            ASTNode* arrayNode = statement->rightMostChild->leftMostChild;
                            char* arrayName = arrayNode->leaf.tok->lexeme;
                            Record* arrRecord = variableExists(symbolTableNode, arrayName, hash(arrayName));
                            bool isLHSStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;
                            bool isRHSStatic = !arrRecord->type.array.isLeftID && !arrRecord->type.array.isRightID;

                            if (isLHSStatic && isRHSStatic) {
                                int leftLHS = varRecord->type.array.left;
                                int rightLHS = varRecord->type.array.right;
                                int leftRHS = arrRecord->type.array.left;
                                int rightRHS = arrRecord->type.array.right;

                                if (varRecord->type.array.leftNegative) {
                                    leftLHS = -leftLHS;
                                }
                                if (varRecord->type.array.rightNegative) {
                                    rightLHS = -rightLHS;
                                }

                                if (arrRecord->type.array.leftNegative) {
                                    leftRHS = -leftRHS;
                                }
                                if (arrRecord->type.array.rightNegative) {
                                    rightRHS = -rightRHS;
                                }

                                int sizeLHS = rightLHS - leftLHS + 1;
                                int sizeRHS = rightRHS - leftRHS + 1;

                                if (sizeLHS != sizeRHS) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Array sizes do not match.\n" RESET, idNode->leaf.tok->linenum);
                                    SEMANTIC_ERROR = true;
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

                // To check whether return variables have been assigned
                if (varRecord != NULL) {
                    varRecord->assigned++;
                }

                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                // Check if the module exists
                ASTNode* outputListNode = statement->leftMostChild;
                ASTNode* moduleNode = outputListNode->next;
                ASTNode* inputListNode = moduleNode->next;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = moduleNode->leaf.tok->linenum;
                }
                lastLineNum = moduleNode->leaf.tok->linenum;

                char* moduleName = moduleNode->leaf.tok->lexeme;
                GlobalRecord* moduleRecord = moduleExists(moduleName, hash(moduleName));
                if (moduleRecord == NULL) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Undefined module %s\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                    SEMANTIC_ERROR = true;
                    break;
                }

                if (moduleRecord->error) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Erroneous Module %s called\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                    SEMANTIC_ERROR = true;
                    break;
                }

                // Check if the module has been declared or defined
                if (!moduleRecord->declared && !moduleRecord->defined) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Module %s has not been declared or defined yet.\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                    SEMANTIC_ERROR = true;
                }

                // Check for recursion
                if (moduleRecord->called) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Recursion is not permitted.\n" RESET, moduleNode->leaf.tok->linenum);
                    SEMANTIC_ERROR = true;
                    break;
                }

                // Checking if the module declaration was redundant or not
                // Module is redundant if it has been both declared and defined when called for the first time
                if (moduleRecord->checkedRedundancy == false) {
                    if (moduleRecord->declared && moduleRecord->defined) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Redundant declaration of module %s.\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                        SEMANTIC_ERROR = true;
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
                bool errorPrinted = false;
                while (curr != NULL) {
                    // Check if there are too many input parameters
                    if (inputNode == NULL && !errorPrinted) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Too many input parameters for module %s\n" RESET, curr->leaf.tok->linenum, moduleName);
                        SEMANTIC_ERROR = true;
                        errorPrinted = true;;
                    }

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
                            printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s.\n" RESET, curr->leaf.tok->linenum, name);
                            SEMANTIC_ERROR = true;
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            if (inputNode != NULL) {
                                inputNode = inputNode->next;
                            }
                            continue;
                        }
                    }
                    

                    VAR_TYPE inputType = typeExtractor(curr, symbolTableNode);
                    if (inputNode != NULL && inputNode->type.varType == ARR) {
                        if (inputType != ARR) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Input parameter type of %s does not match with formal parameter. Expected ARRAY type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme);
                            SEMANTIC_ERROR = true;
                        } else if (inputType == ARR && isMinus) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Unary minus operation not allowed on array %s.\n" RESET, curr->leaf.tok->linenum, inputNode->name);
                            SEMANTIC_ERROR = true;
                        } else {
                            // Check if the array types match
                            VAR_TYPE arrayType = inputNode->type.array.arrType; 
                            VAR_TYPE inputArrayType = varRecord->type.array.arrType;

                            if (arrayType != inputArrayType) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Input parameter type of %s does not match with formal parameter. Expected array of %s type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, typeStrings[arrayType]);
                                SEMANTIC_ERROR = true;
                            }
                            
                            // Check if the array dimensions match if both are static
                            bool inputNodeIsStatic = !inputNode->type.array.isLeftID && !inputNode->type.array.isRightID;
                            bool varRecordIsStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;
                            
                            if (inputNodeIsStatic && varRecordIsStatic) {
                                int inputNodeLeft = inputNode->type.array.left;
                                int inputNodeRight = inputNode->type.array.right;
                                int varRecordLeft = varRecord->type.array.left;
                                int varRecordRight = varRecord->type.array.right;

                                if (inputNode->type.array.leftNegative) {
                                    inputNodeLeft = -inputNodeLeft;
                                }
                                if (inputNode->type.array.rightNegative) {
                                    inputNodeRight = -inputNodeRight;
                                }

                                if (varRecord->type.array.leftNegative) {
                                    varRecordLeft = -varRecordLeft;
                                }
                                if (varRecord->type.array.rightNegative) {
                                    varRecordRight = -varRecordRight;
                                }

                                int inputNodeSize = inputNodeRight - inputNodeLeft + 1;
                                int varRecordSize = varRecordRight - varRecordLeft + 1;

                                if (inputNodeSize != varRecordSize) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Input parameter type of %s does not match with formal parameter. Array sizes mismatch. Expected array of [%d..%d].\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, inputNodeLeft, inputNodeRight);
                                    SEMANTIC_ERROR = true;
                                }
                            }
                        }
                    } else if (inputNode != NULL && inputType != inputNode->type.varType && inputType != ERROR) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Input parameter type of %s does not match with formal parameter. Type mismatch. Expected %s type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, typeStrings[inputNode->type.varType]);
                        SEMANTIC_ERROR = true;
                    }

                    if (isMinus) {
                        curr = curr->parent;
                    }
                    curr = curr->next;
                    if (inputNode != NULL) {
                        inputNode = inputNode->next;
                    }
                }

                if (inputNode != NULL) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Too few input parameters for module %s\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                    SEMANTIC_ERROR = true;
                }

                // Check if the output parameters match
                Record* outputNode = moduleRecord->outputList;
                curr = outputListNode->leftMostChild;
                errorPrinted = false;
                while (curr != NULL) {
                    // Check if there are any more output parameters expected
                    if (outputNode == NULL && !errorPrinted) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Too many output parameters for module %s\n" RESET, curr->leaf.tok->linenum, moduleName);
                        SEMANTIC_ERROR = true;
                        errorPrinted = true;
                    }

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
                            printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, curr->leaf.tok->linenum, name);
                            SEMANTIC_ERROR = true;
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            if (outputNode != NULL) {
                                outputNode = outputNode->next;
                            }
                            continue;
                        } else if (varRecord->iterator) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Iterator variable %s cannot be used as output parameter\n" RESET, curr->leaf.tok->linenum, name);
                            SEMANTIC_ERROR = true;
                            if (isMinus) {
                                curr = curr->parent;
                            }
                            curr = curr->next;
                            if (outputNode != NULL) {
                                outputNode = outputNode->next;
                            }
                            continue;
                        }
                    }
                    
                    // To check whether return variables have been assigned
                    if (varRecord != NULL) {
                        varRecord->assigned++;
                    }

                    VAR_TYPE outputType = typeExtractor(curr, symbolTableNode);
                    if (outputNode != NULL && outputNode->type.varType == ARR) {
                        if (outputType != ARR) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Output parameter type of %s does not match with formal parameter. Type mismatch. Expected ARRAY type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme);
                            SEMANTIC_ERROR = true;
                        } else if (outputType == ARR && isMinus) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Unary minus operation not allowed on array %s.\n" RESET, curr->leaf.tok->linenum, outputNode->name);
                            SEMANTIC_ERROR = true;
                        } else {
                            // Check if the array types match
                            VAR_TYPE arrayType = outputNode->type.array.arrType; 
                            VAR_TYPE outputArrayType = varRecord->type.array.arrType;

                            if (arrayType != outputArrayType) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Output parameter type of %s does not match with formal parameter. Array type mismatch. Expected array of %s type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, typeStrings[arrayType]);
                                SEMANTIC_ERROR = true;
                            }
                            
                            // Check if the array dimensions match if both are static
                            bool outputNodeIsStatic = !outputNode->type.array.isLeftID && !outputNode->type.array.isRightID;
                            bool varRecordIsStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;

                            if (outputNodeIsStatic && varRecordIsStatic) {
                                int outputNodeLeft = outputNode->type.array.left;
                                int outputNodeRight = outputNode->type.array.right;
                                int varRecordLeft = varRecord->type.array.left;
                                int varRecordRight = varRecord->type.array.right;

                                if (outputNode->type.array.leftNegative) {
                                    outputNodeLeft = -outputNodeLeft;
                                }
                                if (outputNode->type.array.rightNegative) {
                                    outputNodeRight = -outputNodeRight;
                                }

                                if (varRecord->type.array.leftNegative) {
                                    varRecordLeft = -varRecordLeft;
                                }
                                if (varRecord->type.array.rightNegative) {
                                    varRecordRight = -varRecordRight;
                                }

                                int outputNodeSize = outputNodeRight - outputNodeLeft + 1;
                                int varRecordSize = varRecordRight - varRecordLeft + 1;

                                if (outputNodeSize != varRecordSize) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Output parameter type of %s does not match with formal parameter. Array sizes mismatch. Expected array of [%d..%d].\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, outputNodeLeft, outputNodeRight);
                                    SEMANTIC_ERROR = true;
                                }
                            }
                        }
                    } else if (outputNode != NULL && outputType != outputNode->type.varType && outputType != ERROR) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Output parameter type of %s does not match with formal parameter. Type mismatch. Expected %s type.\n" RESET, moduleNode->leaf.tok->linenum, curr->leaf.tok->lexeme, typeStrings[outputNode->type.varType]);
                        SEMANTIC_ERROR = true;
                    }

                    if (isMinus) {
                        curr = curr->parent;
                    }
                    curr = curr->next;
                    if (outputNode != NULL) {
                        outputNode = outputNode->next;
                    }
                }

                if (outputNode != NULL) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Too few output parameters for module %s\n" RESET, moduleNode->leaf.tok->linenum, moduleName);
                    SEMANTIC_ERROR = true;
                }

                break;
            }
            case 'D': { // DECLARE_STMT
                ASTNode* assignListNode = statement->leftMostChild;
                ASTNode* dataTypeNode = statement->rightMostChild;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = statement->leaf.tok->linenum;
                }
                lastLineNum = statement->leaf.tok->linenum;

                ASTNode* curr = assignListNode->leftMostChild;
                while (curr != NULL) {
                    // Checking if already exists
                    char* name = curr->leaf.tok->lexeme;
                    unsigned int hashVal = hash(name);

                    // In outputList
                    if (variableExists(symbolTableNode->funcOutputST, name, hashVal) != NULL) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Redeclaration of output variable %s\n" RESET, curr->leaf.tok->linenum, name);
                        SEMANTIC_ERROR = true;
                        curr = curr->next;
                        continue;
                    }

                    // Otherwise, check in current symbol table
                    Record* varRecord = findVariableInsertion(symbolTableNode, name, hashVal);
                    if (varRecord == NULL) {
                        varRecord = generateRecord(symbolTableNode, curr, dataTypeNode, &symbolTableNode->nextOffset, false);
                        symbolTableNode->hashTable[hashVal] = varRecord;
                    } else if (strcmp(varRecord->name, name) == 0) {
                        printf(RED BOLD "[Semantic Analyser] Line %d: Redeclaration of variable %s\n" RESET, curr->leaf.tok->linenum, name);
                        SEMANTIC_ERROR = true;
                    } else {
                        varRecord->next = generateRecord(symbolTableNode, curr, dataTypeNode, &symbolTableNode->nextOffset, false);
                    }
                    
                    curr = curr->next;
                }
                break;
            }
            case 'S': { // SWITCH
                // ID in SWITCH
                ASTNode* idNode = statement->leftMostChild;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = idNode->leaf.tok->linenum;
                }
                lastLineNum = idNode->leaf.tok->linenum;

                VAR_TYPE switchType;
                char* name = idNode->leaf.tok->lexeme;
                Record* varRecord = variableExists(symbolTableNode, name, hash(name));
                if (varRecord != NULL) {
                    switchType = varRecord->type.varType;
                } else {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Undefined variable %s\n" RESET, idNode->leaf.tok->linenum, name);
                    SEMANTIC_ERROR = true;
                }

                int scopeStart = statement->scope.scopeStart;
                int scopeEnd = statement->scope.scopeEnd;
                switch (switchType) {
                    case INT: {
                        // Checking if default statement exists
                        ASTNode* defaultCase = statement->rightMostChild;
                        if (statement->rightMostChild->leaf.tok->tok != DEFAULT) {
                            printf(RED BOLD "[Semantic Analyser] Lines %d-%d: Default statement required for the switch statement of type INTEGER\n" RESET, scopeStart, scopeEnd);
                            SEMANTIC_ERROR = true;
                            defaultCase = NULL;
                        }

                        CaseLabel** caseLabelsTable = malloc(sizeof(CaseLabel*) * HASH_TABLE_SIZE);
                        memset(caseLabelsTable, 0, sizeof(CaseLabel*) * HASH_TABLE_SIZE);

                        // Iterating through all the case statements
                        ASTNode* caseStatements = statement->leftMostChild->next;
                        while (caseStatements != NULL) {
                            // Checking if the case statement is of type INTEGER
                            if (caseStatements != defaultCase && caseStatements->leftMostChild->leaf.tok->tok != NUM) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Case label not of type INTEGER \n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                                SEMANTIC_ERROR = true;
                            }

                            // Checking if the case label is unique
                            if (caseStatements != defaultCase) {
                                char* caseLabel = caseStatements->leftMostChild->leaf.tok->lexeme;
                                unsigned int hashVal = hash(caseLabel);
                                CaseLabel* caseLabelRecord = caseLabelsTable[hashVal];
                                if (caseLabelRecord == NULL) {
                                    caseLabelsTable[hashVal] = malloc(sizeof(CaseLabel));
                                    caseLabelsTable[hashVal]->label = caseLabel;
                                    caseLabelsTable[hashVal]->next = NULL;
                                } else {
                                    CaseLabel* prev = caseLabelRecord;
                                    while (caseLabelRecord != NULL) {
                                        if (strcmp(caseLabelRecord->label, caseLabel) == 0) {
                                            printf(RED BOLD "[Semantic Analyser] Line %d: Duplicate case label %s.\n" RESET, caseStatements->leftMostChild->leaf.tok->linenum, caseLabel);
                                            SEMANTIC_ERROR = true;
                                            break;
                                        }
                                        prev = caseLabelRecord;
                                        caseLabelRecord = caseLabelRecord->next;
                                    }
                                    prev->next = malloc(sizeof(CaseLabel));
                                    prev->next->label = caseLabel;
                                    prev->next->next = NULL;
                                }
                            }

                            // Populating the symbol table for the statements in the case
                            currChild->next = initSymbolTableNode();
                            currChild = currChild->next;
                            currChild->nestingLevel = level;
                            currChild->nextOffset = symbolTableNode->nextOffset;
                            currChild->parent = symbolTableNode;
                            currChild->funcOutputST = symbolTableNode->funcOutputST;
                            if (caseStatements == defaultCase) {
                                populateSymbolTable(currChild, caseStatements->leftMostChild, level + 1);
                            } else {
                                populateSymbolTable(currChild, caseStatements->leftMostChild->next, level + 1);
                            }

                            lastLineNum = currChild->scopeEnd;
                            caseStatements = caseStatements->next;
                        }
                        break;
                    }
                    case DOUBLE: {
                        printf(RED BOLD "[Semantic Analyser] Line %d: REAL type identifier not allowed in switch statement \n" RESET, idNode->leaf.tok->linenum);
                        SEMANTIC_ERROR = true;
                        break;
                    }
                    case BOOL: {
                        // Checking that there is no default statement
                        ASTNode* defaultStatement = NULL;
                        if (statement->rightMostChild->leaf.tok->tok == DEFAULT) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Default statement not allowed in switch statement of type BOOLEAN\n" RESET, statement->rightMostChild->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
                            defaultStatement = statement->rightMostChild;
                        }

                        bool trueDone = false;
                        bool falseDone = false;

                        // Iterating through all the case statements
                        ASTNode* caseStatements = statement->leftMostChild->next;
                        while (caseStatements != NULL) {
                            // Checking if the case statement is of type BOOLEAN
                            if (caseStatements != defaultStatement && caseStatements->leftMostChild->leaf.tok->tok != TRUE && caseStatements->leftMostChild->leaf.tok->tok != FALSE) {
                                printf(RED BOLD "[Semantic Analyser] Line %d: Case label not of type BOOLEAN \n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                                SEMANTIC_ERROR = true;
                            }

                            if (caseStatements != defaultStatement && caseStatements->leftMostChild->leaf.tok->tok == TRUE) {
                                if (trueDone) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Duplicate case label for TRUE \n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                                    SEMANTIC_ERROR = true;
                                } else {
                                    trueDone = true;
                                }
                            } else if (caseStatements != defaultStatement && caseStatements->leftMostChild->leaf.tok->tok == FALSE) {
                                if (falseDone) {
                                    printf(RED BOLD "[Semantic Analyser] Line %d: Duplicate case label for FALSE\n" RESET, caseStatements->leftMostChild->leaf.tok->linenum);
                                    SEMANTIC_ERROR = true;
                                } else {
                                    falseDone = true;
                                }
                            }

                            // Populating the symbol table for the statements in the case
                            currChild->next = initSymbolTableNode();
                            currChild = currChild->next;
                            currChild->nestingLevel = level;
                            currChild->nextOffset = symbolTableNode->nextOffset;
                            currChild->parent = symbolTableNode;
                            currChild->funcOutputST = symbolTableNode->funcOutputST;
                            if (caseStatements == defaultStatement) {
                                populateSymbolTable(currChild, caseStatements->leftMostChild, level + 1);
                            } else {
                                populateSymbolTable(currChild, caseStatements->leftMostChild->next, level + 1);
                            }

                            lastLineNum = currChild->scopeEnd;
                            caseStatements = caseStatements->next;
                        }

                        if (!trueDone) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Missing case label for TRUE .\n" RESET, statement->rightMostChild->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
                        }
                        if (!falseDone) {
                            printf(RED BOLD "[Semantic Analyser] Line %d: Missing case label for FALSE.\n" RESET, statement->rightMostChild->leaf.tok->linenum);
                            SEMANTIC_ERROR = true;
                        }
                        break;
                    }
                    case ARR: {
                        printf(RED BOLD "[Semantic Analyser] Line %d: ARRAY type identifier not allowed in switch statement\n" RESET, idNode->leaf.tok->linenum);
                        SEMANTIC_ERROR = true;
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
                currChild->nestingLevel = level;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;
                currChild->funcOutputST = symbolTableNode->funcOutputST;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = statement->leftMostChild->leaf.tok->linenum;
                }
                lastLineNum = statement->leftMostChild->leaf.tok->linenum;

                // ID IN FOR LOOP 
                ASTNode* idNode = statement->leftMostChild; 
                char* name = idNode->leaf.tok->lexeme;
                unsigned int hashVal = hash(name);
                Record* varRecord = malloc(sizeof(Record));
                currChild->hashTable[hashVal] = varRecord;
                strcpy(varRecord->name, name);
                varRecord->iterator = true;
                varRecord->width = SIZEOF_INT;
                varRecord->offset = currChild->nextOffset;
                currChild->nextOffset += SIZEOF_INT;
                varRecord->type.varType = INT;
                varRecord->next = NULL;

                // Bound checking for the FOR loop
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

                if (left > right) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Invalid Bounds in FOR loop.\n" RESET, idNode->leaf.tok->linenum);
                    SEMANTIC_ERROR = true;
                }

                // Statements inside the for loop
                populateSymbolTable(currChild, statement->leftMostChild->next->next, level + 1);
                lastLineNum = currChild->scopeEnd;

                // Setting the scope fetched earlier from the AST
                currChild->scopeStart = statement->scope.scopeStart;
                currChild->scopeEnd = statement->scope.scopeEnd;
                break;
            }
            case 'W': { // WHILE
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nestingLevel = level;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;
                currChild->funcOutputST = symbolTableNode->funcOutputST;

                // Setting scopeStart and lastLineNum
                if (symbolTableNode->scopeStart == 0) {
                    symbolTableNode->scopeStart = statement->leftMostChild->leaf.tok->linenum;
                }
                lastLineNum = statement->leftMostChild->leaf.tok->linenum;

                // Check whether the expression is a boolean expression or not
                ASTNode* exprNode = statement->leftMostChild;
                VAR_TYPE exprType = typeExtractor(exprNode, symbolTableNode);

                if (exprType != BOOL && exprType != ERROR) {
                    printf(RED BOLD "[Semantic Analyser] Line %d: Expression in while loop not of type BOOLEAN\n" RESET, exprNode->leaf.tok->linenum);
                    SEMANTIC_ERROR = true;
                }

                // For checking whether any of the variables in the expression are changing or not
                RecordList* exprIDList = createWhileExprIDList(exprNode, symbolTableNode);
                int* currCount = NULL;
                if (exprIDList > 0) {
                    currCount = malloc(sizeof(int) * exprIDList->size);
                }
                RecordListNode* curr = exprIDList->head;


                for (int i = 0; i < exprIDList->size; ++i) {
                    currCount[i] = curr->record->assigned;
                    curr = curr->next;
                }

                // Statements inside the while loop
                populateSymbolTable(currChild, statement->leftMostChild->next, level + 1);
                lastLineNum = currChild->scopeEnd;

                // Setting the scope fetched earlier from the AST
                currChild->scopeStart = statement->scope.scopeStart;
                currChild->scopeEnd = statement->scope.scopeEnd;

                // Checking whether any of the variables in the expression are changing or not
                curr = exprIDList->head;
                bool changed = false;
                for (int i = 0; i < exprIDList->size; ++i) {
                    if (currCount[i] < curr->record->assigned) {
                        changed = true;
                    }
                    curr = curr->next;
                }

                if (!changed && exprIDList->size > 0) {
                    printf(RED BOLD "[Semantic Analyser] Lines %d-%d: No variable in the conditional expression for while loop is changing.\n" RESET, statement->scope.scopeStart, statement->scope.scopeEnd);
                    SEMANTIC_ERROR = true;
                }
                break;
            }
        }
        statement = statement->next;
    }

    currChild->next = NULL;
    symbolTableNode->children = sentinel->next;
    symbolTableNode->scopeEnd = lastLineNum;
    free(sentinel);
    return;
}

void calculateActivationRecordSize(GlobalRecord* funcRecord, SymbolTableNode* symbolTableNode) {
    if (symbolTableNode == NULL) {
        return;
    }

    funcRecord->activationRecordSize = max(funcRecord->activationRecordSize, symbolTableNode->nextOffset);

    SymbolTableNode* currChild = symbolTableNode->children;
    while (currChild != NULL) {
        calculateActivationRecordSize(funcRecord, currChild);
        currChild = currChild->next;
    }

    return;
}

void addModuleDeclarationToSymbolTable(ASTNode* moduleDeclarationNode) {
    char* name = moduleDeclarationNode->leaf.tok->lexeme;
    GlobalRecord* funcRecord = moduleExists(name, hash(name));

    if (funcRecord == NULL) {
        printf(RED BOLD "[Semantic Analyser] Module %s declared but never defined\n" RESET, name);
        SEMANTIC_ERROR = true;
        return;
    }

    if (funcRecord->declared) {
        printf(RED BOLD "[Semantic Analyser] Line %d: Module %s redeclared\n" RESET, moduleDeclarationNode->leftMostChild->leaf.tok->linenum, name);
        SEMANTIC_ERROR = true;
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
    ASTNode* statementsNode = moduleNode->rightMostChild->leftMostChild;

    // Checking for redundancy of declaration
    if (funcRecord->declared && !funcRecord->checkedRedundancy) {
        funcRecord->checkedRedundancy = true;
        printf(RED BOLD "[Semantic Analyser] Line %d: Redundant declaration of module %s.\n" RESET, moduleNode->leftMostChild->leaf.tok->linenum, name);
        SEMANTIC_ERROR = true;
    }

    funcRecord->called = true;
    populateSymbolTable(funcRecord->funcST, statementsNode, 2);
    funcRecord->called = false;

    calculateActivationRecordSize(funcRecord, funcRecord->funcST);
    calculateActivationRecordSize(funcRecord, funcRecord->inputST);
    calculateActivationRecordSize(funcRecord, funcRecord->outputST);

    // Setting the scope fetched earlier from the AST
    funcRecord->funcST->scopeStart = moduleNode->rightMostChild->scope.scopeStart;
    funcRecord->funcST->scopeEnd = moduleNode->rightMostChild->scope.scopeEnd;
    funcRecord->inputST->scopeStart = moduleNode->rightMostChild->scope.scopeStart;
    funcRecord->inputST->scopeEnd = moduleNode->rightMostChild->scope.scopeEnd;
    funcRecord->outputST->scopeStart = moduleNode->rightMostChild->scope.scopeStart;
    funcRecord->outputST->scopeEnd = moduleNode->rightMostChild->scope.scopeEnd;

    // Check if all the output variables have been assigned some value or not
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        Record* varRecord = funcRecord->outputST->hashTable[i];
        if (varRecord == NULL) { continue; }
        while (varRecord != NULL) {
            if (varRecord->assigned == 0) {
                printf(RED BOLD "[Semantic Analyser] Line %d: Output variable %s not assigned a value before return from module %s \n" RESET, funcRecord->funcST->scopeEnd, varRecord->name, name);
                SEMANTIC_ERROR = true;
            }
            varRecord = varRecord->next;
        }
    }

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
            printf(RED BOLD "[Semantic Analyser] Line %d: Overloading of module %s .\n" RESET, moduleSignatureNode->leftMostChild->leaf.tok->linenum, name);
            SEMANTIC_ERROR = true;
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
    funcRecord->error = false;
    funcRecord->activationRecordSize = 0;
    funcRecord->funcST = initSymbolTableNode();
    funcRecord->funcST->nestingLevel = 1;
    funcRecord->inputST = initSymbolTableNode();
    funcRecord->funcST->funcInputST = funcRecord->inputST;
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

void printSymbolTableRec(SymbolTableNode* symbolTableNode, char* moduleName, FILE* fp) {
    if (symbolTableNode == NULL) {
        return;
    }

    int level = symbolTableNode->nestingLevel;
    
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTableNode->hashTable[i] == NULL) { continue; }
        Record* varRecord = symbolTableNode->hashTable[i];
        int offset;
        while (varRecord != NULL) {
            char scope[25];
            sprintf(scope, "%d-%d", symbolTableNode->scopeStart, symbolTableNode->scopeEnd);
            fprintf(fp, "%-25s%-25s%-25s%-7d", varRecord->name, moduleName, scope, varRecord->width);
            switch (varRecord->type.varType) {
                case INT: {
                    fprintf(fp, "%-10s%-20s%-50s%-20s", 
                    "NO", 
                    "**",
                    "**",
                    "INTEGER"
                    );
                    break;
                }
                case DOUBLE: {
                    fprintf(fp, "%-10s%-20s%-50s%-20s", 
                    "NO", 
                    "**",
                    "**",
                    "REAL"
                    );
                    break;
                }
                case BOOL: {
                    fprintf(fp, "%-10s%-20s%-50s%-20s", 
                    "NO", 
                    "**",
                    "**",
                    "BOOLEAN"
                    );
                    break;
                }
                case ARR: {
                    char* typeStrings[] = {
                        "INTEGER",
                        "REAL",
                        "BOOLEAN"
                    };
                    bool isStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;
                    char firstRange[25] = {};
                    char range[50] = {};
                    if (isStatic) {
                        if (varRecord->type.array.leftNegative) {
                            sprintf(firstRange, "[-%d", varRecord->type.array.left);
                        } else {
                            sprintf(firstRange, "[%d", varRecord->type.array.left);
                        }
                        strcat(firstRange, "..");
                        if (varRecord->type.array.rightNegative) {
                            sprintf(range, "%s-%d]", firstRange, varRecord->type.array.right);
                        } else {
                            sprintf(range, "%s%d]", firstRange, varRecord->type.array.right);
                        }
                    } else if (varRecord->type.array.isLeftID && varRecord->type.array.isRightID) {
                        if (varRecord->type.array.leftNegative) {
                            sprintf(firstRange, "[-%s", varRecord->type.array.leftID);
                        } else {
                            sprintf(firstRange, "[%s", varRecord->type.array.leftID);
                        }
                        strcat(firstRange, "..");
                        if (varRecord->type.array.rightNegative) {
                            sprintf(range, "%s-%s]", firstRange, varRecord->type.array.rightID);
                        } else {
                            sprintf(range, "%s%s]", firstRange, varRecord->type.array.rightID);
                        }
                    } else if (varRecord->type.array.isLeftID) {
                        if (varRecord->type.array.leftNegative) {
                            sprintf(firstRange, "[-%s", varRecord->type.array.leftID);
                        } else {
                            sprintf(firstRange, "[%s", varRecord->type.array.leftID);
                        }
                        strcat(range, "..");
                        if (varRecord->type.array.rightNegative) {
                            sprintf(range, "%s-%d]", firstRange, varRecord->type.array.right);
                        } else {
                            sprintf(range, "%s%d]", firstRange, varRecord->type.array.right);
                        }
                    } else {
                        if (varRecord->type.array.leftNegative) {
                            sprintf(firstRange, "[-%d", varRecord->type.array.left);
                        } else {
                            sprintf(firstRange, "[%d", varRecord->type.array.left);
                        }
                        strcat(range, "..");
                        if (varRecord->type.array.rightNegative) {
                            sprintf(range, "%s-%s]", firstRange, varRecord->type.array.rightID);
                        } else {
                            sprintf(range, "%s%s]", firstRange, varRecord->type.array.rightID);
                        }
                    }

                    fprintf(fp, "%-10s%-20s%-50s%-20s", 
                    "YES", 
                    (isStatic ? "Static" : "Dynamic"),
                    range,
                    typeStrings[varRecord->type.array.arrType]
                    );
                    break;
                }
                default: {
                    // Should never reach here
                    break;
                }
            }
            fprintf(fp, "%-10d%-10d\n", varRecord->offset, level);
            varRecord = varRecord->next;
        }
    }

    if (symbolTableNode->children == NULL) {
        return;
    }

    SymbolTableNode* child = symbolTableNode->children;
    while (child != NULL) {
        printSymbolTableRec(child, moduleName, fp);
        child = child->next;
    }

    return;
}

void printSymbolTable(char* filename) {
    FILE* fp = fopen(filename, "w");
    fprintf(fp, "%-25s%-25s%-25s%-7s%-10s%-20s%-50s%-20s%-10s%-15s\n",
        "Variable_name",
        "Scope (Module Name)",
        "Scope (Line Numbers)",
        "Width",
        "isArray",
        "Static or Dynamic",
        "Range",
        "Type of Element",
        "Offset",
        "Nesting Level");
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTable->global[i] == NULL) { continue; }
        GlobalRecord* funcRecord = symbolTable->global[i];
        while (funcRecord != NULL) {
            printSymbolTableRec(funcRecord->inputST, funcRecord->name, fp);
            printSymbolTableRec(funcRecord->outputST, funcRecord->name, fp);
            printSymbolTableRec(funcRecord->funcST, funcRecord->name, fp);
            funcRecord = funcRecord->next;
        }
    }
    printf(GREEN BOLD "Symbol Table generated successfully\n" RESET);
    fclose(fp);

    return;
}

void printSymbolTableArrayRec(char* name, SymbolTableNode* symbolTableNode) {
    if (symbolTableNode == NULL) {
        return;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        Record* varRecord = symbolTableNode->hashTable[i];
        while (varRecord != NULL) {
            if (varRecord->type.varType != ARR) {
                varRecord = varRecord->next;
                continue;
            }
            printf("%-25s", name);
            char scope[10];
            sprintf(scope, "%d-%d", symbolTableNode->scopeStart, symbolTableNode->scopeEnd);
            printf("%-12s", scope);
            printf("%-25s", varRecord->name);
            
            bool isStatic = !varRecord->type.array.isLeftID && !varRecord->type.array.isRightID;
            if (isStatic) {
                printf("%-15s", "Static Array");
            } else {
                printf("%-15s", "Dynamic Array");
            }

            char* typeStrings[] = {
                "INTEGER",
                "REAL",
                "BOOLEAN",
            };
            printf("%-10s", typeStrings[varRecord->type.array.arrType]);

            printf("[");

            if (varRecord->type.array.leftNegative) {
                printf("-");
            }

            if (varRecord->type.array.isLeftID) {
                printf("%s", varRecord->type.array.leftID);
            } else {
                printf("%d", varRecord->type.array.left);
            }

            printf("..");

            if (varRecord->type.array.rightNegative) {
                printf("-");
            }
            
            if (varRecord->type.array.isRightID) {
                printf("%s", varRecord->type.array.rightID);
            } else {
                printf("%d", varRecord->type.array.right);
            }

            printf("]\n");

            varRecord = varRecord->next;
        }
    }

    return;
}

void printSymbolTableArray(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        GlobalRecord* funcRecord = symbolTable->global[i];
        while (funcRecord != NULL) {
            printSymbolTableArrayRec(funcRecord->name, funcRecord->inputST);
            printSymbolTableArrayRec(funcRecord->name, funcRecord->outputST);
            printSymbolTableArrayRec(funcRecord->name, funcRecord->funcST);
            funcRecord = funcRecord->next;
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
                printf(RED BOLD "[Semantic Analyser] Module %s declared but not defined.\n" RESET, funcRecord->name);
                SEMANTIC_ERROR = true;
            }
            funcRecord = funcRecord->next;
        }
    }

    return;
}