/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdbool.h>
#define HASH_TABLE_SIZE 1021

typedef struct Type {
    int tok;
    union {
        struct array {
            bool isLeftID;
            bool isRightID;
            union {
                char leftID[20];
                int left;
            };
            union {
                char rightID[20];
                int right;
            };
        } array;
        struct function {
            struct Type* inputList;
            unsigned int inputListSize;
            struct Type* outputList;
            unsigned int outputListSize;
        } function;
    };
} Type;

typedef struct Record {
    Type type;
    unsigned int offset;
} Record;

typedef struct HashTable {
    Record* hashTable[HASH_TABLE_SIZE];
    unsigned int maxOffset;
} HashTable;

typedef struct SymbolTableNode {
    HashTable* table;
    HashTable** children;
} SymbolTableNode;

typedef struct SymbolTable {
    SymbolTableNode* global;
    int depth;
} SymbolTable;