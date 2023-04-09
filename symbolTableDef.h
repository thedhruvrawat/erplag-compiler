/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#include <stdbool.h>
#include <stdlib.h>
#include "ast.h"

#define HASH_TABLE_SIZE 1021
#define SIZEOF_INT 2L
#define SIZEOF_REAL 4L
#define SIZEOF_BOOL 1L

typedef enum {
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    GT_OP,
    LT_OP,
    LE_OP,
    GE_OP,
    EQ_OP,
    NE_OP,
    AND_OP,
    OR_OP,
    UPLUS_OP,
    UMINUS_OP,
    ASSIGN_QOP
} OPERATOR;

typedef struct Type {
    VAR_TYPE varType;
    struct array {
        VAR_TYPE arrType;
        bool isLeftID;
        bool leftNegative;
        bool isRightID;
        bool rightNegative;
        union {
            char leftID[20];
            int left;
        };
        union {
            char rightID[20];
            int right;
        };
    } array;
} Type;

typedef struct Record {
    char name[20];
    int assigned;
    bool iterator;
    Type type;
    unsigned int offset;
    struct Record* next;
} Record;

typedef struct RecordListNode {
    Record* record;
    struct RecordListNode* next;
} RecordListNode;

typedef struct RecordList {
    RecordListNode* head;
    RecordListNode* tail;
    int size;
} RecordList;

typedef struct Quadruple {
    OPERATOR op;
    bool isArg1ID;
    bool isArg2ID;
    VAR_TYPE arg1Type;
    VAR_TYPE arg2Type;
    union {
        int arg1Num;
        double arg1Real;
        bool arg1Bool;
        Record* arg1ID;
        RecordList* inputList;
    };
    union {
        int arg2Num;
        double arg2Real;
        bool arg2Bool;
        Record* arg2ID;
        RecordList* outputList;
    };

    Record* result;
    struct Quadruple* next;
} Quadruple;

typedef struct QuadrupleTable {
    int currentNumber;
    Quadruple* head;
    Quadruple* tail;
    int size;
} QuadrupleTable;

typedef struct CaseLabel {
    char* label;
    struct CaseLabel* next;
} CaseLabel;

typedef struct SymbolTableNode {
    Record* hashTable[HASH_TABLE_SIZE];
    struct SymbolTableNode* children;
    int scopeStart;
    int scopeEnd;
    unsigned int nextOffset;
    // QuadrupleTable* quadTable;
    struct SymbolTableNode* funcOutputST;
    struct SymbolTableNode* next;
    struct SymbolTableNode* parent;
} SymbolTableNode;

typedef struct DeferCheckNode {
    SymbolTableNode* symbolTableNode;
    ASTNode* statement;
    struct DeferCheckNode* next;
} DeferCheckNode;

typedef struct GlobalRecord { // These are Records at the first layer/row of Symbol Table. They contain only function Symbol Tables as per current grammar but would change if grammar allows Global Variable
    char name[20];
    int linenum;
    bool driver;
    bool called;
    bool declared;
    bool defined;
    bool error;
    bool checkedRedundancy;
    Record* inputList;
    unsigned int inputListSize;
    Record* outputList;
    unsigned int outputListSize;
    SymbolTableNode* funcST;
    SymbolTableNode* outputST;
    struct GlobalRecord* next;
} GlobalRecord;

typedef struct SymbolTable {
    GlobalRecord* global[HASH_TABLE_SIZE];
    int height;
} SymbolTable;

typedef struct SymbolTableStackNode {
    ASTNode* node;
    unsigned int offset;
    struct SymbolTableStackNode* next;
} SymbolTableStackNode;

typedef struct SymbolTableStack {
    SymbolTableStackNode* top;
    int size;
} SymbolTableStack;

extern SymbolTable* symbolTable;

#endif