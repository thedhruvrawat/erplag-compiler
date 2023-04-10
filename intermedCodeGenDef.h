/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include "symbolTable.h"

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
    ASSIGN_VAR_OP,
    ASSIGN_ARRAY_OP,
    ASSIGN_ARRAY_ACCESS_OP,
    MODULE_OP,
    DRIVER_OP,
    GET_VALUE_OP,
    PRINT_ID_OP,
    PRINT_ARR_ELE_OP,
    MODULE_USE_OP,
    SWITCH_OP,
    CASE_OP,
    FOR_OP,
    WHILE_OP,
    START_OP,
    END_OP
} OPERATOR;


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

extern QuadrupleTable* quadTable;
