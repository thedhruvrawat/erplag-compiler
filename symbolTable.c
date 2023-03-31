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

SymbolTable* symbolTable;

// Fowler–Noll–Vo hash function
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

Record* generateRecord(ASTNode* idNode, ASTNode* dataTypeNode, int* nextOffset) {
    char* name = idNode->parseTreeNode->tok->lexeme;
    Record* newRec = malloc(sizeof(Record));
    strcpy(newRec->name, name);
    newRec->offset = *nextOffset;
    newRec->linenum = idNode->parseTreeNode->tok->linenum;
    newRec->next = NULL;

    switch (dataTypeNode->parseTreeNode->tokenID) {
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
            switch (dataTypeNode->rightMostChild->parseTreeNode->tokenID) {
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

            if (leftIDNum->parseTreeNode->tokenID == ID) {
                newRec->type.array.isLeftID = true;
                strcpy(newRec->type.array.leftID, leftIDNum->parseTreeNode->tok->lexeme);
            } else {
                newRec->type.array.isLeftID = false;
                newRec->type.array.left = leftIDNum->parseTreeNode->tok->num;
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

            if (rightIDNum->parseTreeNode->tokenID == ID) {
                newRec->type.array.isRightID = true;
                strcpy(newRec->type.array.rightID, rightIDNum->parseTreeNode->tok->lexeme);
            } else {
                newRec->type.array.isRightID = false;
                newRec->type.array.right = rightIDNum->parseTreeNode->tok->num;
            }

            // The static flag of the ast node itself (required during code generation (maybe))
            dataTypeNode->isStatic = !(newRec->type.array.isLeftID || newRec->type.array.isRightID);

            // Calculating next offset
            if (dataTypeNode->isStatic) {
                int left = newRec->type.array.left * (newRec->type.array.leftNegative ? -1 : 1);
                int right = newRec->type.array.right * (newRec->type.array.rightNegative ? -1 : 1);

                if (left > right) {
                    printf("Specified subrange for array \"%s\" are illegal. (%d > %d)\n", name, left, right);
                    return NULL;
                }
                *nextOffset += elementSize * (right - left);
            } else {
                // Adding the size of pointer
                *nextOffset += sizeof(void*);
            }

            // TODO: Check whether both the dynamic bounds have been declared and are of integer type
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
bool variableExists(SymbolTableNode* symbolTableNode, char* name){
    int hashVal = hash(name);
    // iF variable doesn't exists, hashTable[hashVal] is 0/NULL
    return (symbolTableNode->hashTable[hashVal] != NULL);
}

Record* findVariable(SymbolTableNode* symbolTableNode, char* name, unsigned int hashVal) {
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

    // Creating the inputList
    int inputListSize = 0;
    Record* sentinel = malloc(sizeof(Record));
    Record* curr = sentinel;
    ASTNode* inputNode = inputList->leftMostChild;
    while (inputNode != NULL) {
        Record* newRecord = generateRecord(inputNode->leftMostChild, inputNode->rightMostChild, offset);
        curr->next = newRecord;
        curr = curr->next;
        inputNode = inputNode->next;
        inputListSize++;
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
        Record* newRecord = generateRecord(outputNode->leftMostChild, outputNode->rightMostChild, offset);
        curr->next = newRecord;
        curr = curr->next;
        outputNode = outputNode->next;
        outputListSize++;
    }
    curr->next = NULL;
    funcRecord->outputList = sentinel->next;
    funcRecord->outputListSize = outputListSize;

    free(sentinel);
    return;
}

void populateSymbolTableRec(SymbolTableNode* symbolTableNode, ASTNode* statement) {
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
                ASTNode* id = statement->leftMostChild; // ID node of GET_VALUE()
                // Do we need to check if it is ID node or not ? i.e. It SHOULDN'T be ARRAY, INTEGER,..
                char * idLabel = id->parseTreeNode->tok->lexeme;
                if(variableExists(symbolTableNode, idLabel)){ // Existence Check if already declared or not
                    // TODO : GET_VALUE fetches user input into the variable
                } else {
                    printf("Variable %s is not defined previously at line %d\n",idLabel, id->parseTreeNode->tok->linenum);
                }
                break;
            }
            case 'P': { // PRINT
                ASTNode* id = statement->leftMostChild; // ID node of PRINT()
                char* idLabel = id->parseTreeNode->tok->lexeme;
                if(variableExists(symbolTableNode, idLabel)){
                } else {
                    printf("Variable %s  is not defined previously at line %d\n",idLabel, id->parseTreeNode->tok->linenum);
                }
                break;
            }
            case 'A': { // ASSIGN_STMT
                // Left Most Child is always an identifier 
                ASTNode* assignID = statement->leftMostChild;
                char* assignIDLabel = assignID->parseTreeNode->tok->lexeme;
                // DEBUGGING
                // printf("DEBUGGING : assingID lexeme is %s\n", assignIDLabel);
                if(variableExists(symbolTableNode, assignIDLabel)){
                } else {
                    printf("Variable %s is not defined previously at line %d\n",assignIDLabel, assignID->parseTreeNode->tok->linenum);
                    break;
                }
                break;
            }
            case 'M': { // MODULE_REUSE_STMT
                // MODULE_REUSE_STMT has an actual para list
                break;
            }
            case 'D': { // DECLARE_STMT
                ASTNode* assignListNode = statement->leftMostChild;
                ASTNode* dataTypeNode = statement->rightMostChild;

                ASTNode* curr = assignListNode->leftMostChild;
                while (curr != NULL) {
                    // Checking if already exists
                    char* name = curr->parseTreeNode->tok->lexeme;
                    unsigned int hashVal = hash(name);
                    Record* varRecord = findVariable(symbolTableNode, name, hashVal);
                    if (varRecord == NULL) {
                        varRecord = generateRecord(curr, dataTypeNode, &symbolTableNode->nextOffset);
                        symbolTableNode->hashTable[hashVal] = varRecord;
                    } else if (strcmp(varRecord->name, name) == 0) {
                        printf("Redeclaration of variable %s at line %d\n", name, curr->parseTreeNode->tok->linenum);
                    } else {
                        varRecord->next = generateRecord(curr, dataTypeNode, &symbolTableNode->nextOffset);
                    }
                    
                    curr = curr->next;
                }
                break;
            }
            case 'S': { // SWITCH
                break;
            }
            case 'F': { // FOR
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;

                // ID IN FOR LOOP 
                ASTNode* idNode = statement->leftMostChild; 
                char* name = idNode->parseTreeNode->tok->lexeme;
                unsigned int hashVal = hash(name);
                Record* varRecord = malloc(sizeof(Record));
                currChild->hashTable[hashVal] = varRecord;
                strcpy(varRecord->name, name);
                varRecord->offset = currChild->nextOffset;
                currChild->nextOffset += sizeof(int);
                varRecord->linenum = idNode->parseTreeNode->tok->linenum;
                varRecord->type.varType = INT;
                varRecord->next = NULL;

                // Statements inside the for loop
                populateSymbolTableRec(currChild, statement->rightMostChild);
                break;
            }
            case 'W': { // WHILE
                currChild->next = initSymbolTableNode();
                currChild = currChild->next;
                currChild->nextOffset = symbolTableNode->nextOffset;
                currChild->parent = symbolTableNode;

                // TODO: Check whether the expression is a boolean expression or not

                // Statements inside the while loop
                populateSymbolTableRec(currChild, statement->rightMostChild);
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

void populateSymbolTable(GlobalRecord* funcRecord, SymbolTableNode* symbolTableNode, ASTNode* moduleDefNode) {
    // Adding inputList variables to the symbolTableNode
    Record* inputListNode = funcRecord->inputList;
    while (inputListNode != NULL) {
        char* name = inputListNode->name;
        unsigned int hashVal = hash(name);
        Record* varRecord = findVariable(symbolTableNode, name, hashVal);
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, inputListNode, sizeof(Record));
            varRecord->next = NULL;
            symbolTableNode->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf("Redeclaration of variable %s in input list of module %s at line %d\n", name, funcRecord->name, inputListNode->linenum);
        } else {
            varRecord->next = malloc(sizeof(Record));
            varRecord = varRecord->next;
            memcpy(varRecord, inputListNode, sizeof(Record));
            varRecord->next = NULL;
        }

        inputListNode = inputListNode->next;
    }

    // Adding outputList variables to the symbolTableNode
    Record* outputListNode = funcRecord->outputList;
    while (outputListNode != NULL) {
        char* name = outputListNode->name;
        unsigned int hashVal = hash(name);
        Record* varRecord = findVariable(symbolTableNode, name, hashVal);
        if (varRecord == NULL) {
            varRecord = malloc(sizeof(Record));
            memcpy(varRecord, outputListNode, sizeof(Record));
            varRecord->next = NULL;
            symbolTableNode->hashTable[hashVal] = varRecord;
        } else if (strcmp(varRecord->name, name) == 0) {
            printf("Redeclaration of variable %s in output list of module %s at line %d\n", name, funcRecord->name, outputListNode->linenum);
        } else {
            varRecord->next = malloc(sizeof(Record));
            varRecord = varRecord->next;
            memcpy(varRecord, outputListNode, sizeof(Record));
            varRecord->next = NULL;
        }

        outputListNode = outputListNode->next;
    }

    // Adding the statements recursively
    populateSymbolTableRec(symbolTableNode, moduleDefNode->leftMostChild);

    return;
}

void addModuleDeclarationToSymbolTable(ASTNode* moduleDeclarationNode) {
    char* name = moduleDeclarationNode->parseTreeNode->tok->lexeme;
    unsigned int hashVal = hash(name);

    GlobalRecord* funcRecord;
    if (symbolTable->global[hashVal] != NULL) {
        funcRecord = findFunction(name, hashVal);
        if (strcmp(funcRecord->name, name) == 0) {
            printf("Redeclaration of module %s at line %d.\n", name, moduleDeclarationNode->parseTreeNode->tok->linenum);
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
    funcRecord->linenum = moduleDeclarationNode->parseTreeNode->tok->linenum;
    funcRecord->called = false;
    funcRecord->defined = false;
    funcRecord->driver = false;
    funcRecord->funcST = NULL;
    funcRecord->inputList = NULL;
    funcRecord->outputList = NULL;
    funcRecord->inputListSize = 0;
    funcRecord->outputListSize = 0;
    funcRecord->next = NULL;

    return;
}

void addFunctionToSymbolTable(ASTNode* moduleNode) {
    bool driver = (strcmp(moduleNode->label, "DRIVER") == 0);
    char* name;
    ASTNode* idNode;

    if (driver) {
        name = "DRIVER";
        idNode = moduleNode;
    } else {
        idNode = moduleNode->leftMostChild;
        name = idNode->parseTreeNode->tok->lexeme;
    }
    unsigned int hashVal = hash(name);

    GlobalRecord* funcRecord;

    if (symbolTable->global[hashVal] != NULL) {
        funcRecord = findFunction(name, hashVal);
        if (strcmp(funcRecord->name, name) != 0) {
            funcRecord->next = malloc(sizeof(GlobalRecord));
            funcRecord = funcRecord->next;
        } else if (funcRecord->defined) {
            printf("Redefinition of module %s at line %d.\n", name, idNode->parseTreeNode->tok->linenum);
            return;
        }
    } else {
        funcRecord = malloc(sizeof(GlobalRecord));
        symbolTable->global[hashVal] = funcRecord;
    }


    strcpy(funcRecord->name, name);
    funcRecord->linenum = idNode->parseTreeNode->tok->linenum;
    funcRecord->called = true;
    funcRecord->defined = true;
    funcRecord->driver = driver;
    funcRecord->funcST = initSymbolTableNode();
    funcRecord->inputList = NULL;
    funcRecord->outputList = NULL;
    funcRecord->next = NULL;

    if (!driver) {
        ASTNode* inputList = idNode->next;
        ASTNode* outputList = inputList->next;
        ASTNode* moduleDef = outputList->next;
        populateInputOutputList(funcRecord, inputList, outputList);
        populateSymbolTable(funcRecord, funcRecord->funcST, moduleDef); 
    } else {
        populateSymbolTable(funcRecord, funcRecord->funcST, moduleNode->leftMostChild);
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
                    printf("ARRAY\t\t");

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
            }
            varRecord = varRecord->next;
        }
    }

    printf("\nPrinting Children\n");

    SymbolTableNode* child = symbolTableNode->children;
    while (child != NULL) {
        printSymbolTableRec(child);
        child = child->next;
    }

    printf("\n\n");

    return;
}

void printSymbolTable(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        if (symbolTable->global[i] == NULL) { continue; }
        GlobalRecord* funcRecord = symbolTable->global[i];
        while (funcRecord != NULL) {
            printf("%-5dFunction: %s\n", i, funcRecord->name);
            printSymbolTableRec(funcRecord->funcST);
            funcRecord = funcRecord->next;
        }
    }

    return;
}

void generateSymbolTable(AST* ast) {
    symbolTable = initSymbolTable();

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

    // TODO: check for redundant definitions
    // Construct a declared flag and use that appropriately

    return;
}