/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "symbolTableDef.h"

unsigned int hash(const char *str);
SymbolTableNode* initSymbolTableNode(void);
SymbolTable* initSymbolTable(void);
void pushToSymbolTableStack(SymbolTableStack* st, ASTNode* node, unsigned int offset);
SymbolTableStackNode* peekSymbolTableStack(SymbolTableStack* st);
void popSymbolTableStack(SymbolTableStack* st);
SymbolTableStack* initSymbolTableStack(void);
Record* generateRecord(ASTNode* idNode, ASTNode* dataTypeNode, int* nextOffset);
GlobalRecord* findFunction(char* name, unsigned int hashVal);
Record* variableExists(SymbolTableNode* symbolTableNode, char* name, unsigned int hashVal);
GlobalRecord* moduleExists(char* name, unsigned int hashVal);
Record* findVariableInsertion(SymbolTableNode* symbolTableNode, char* name, unsigned int hashVal);
void populateInputOutputList(GlobalRecord* funcRecord, ASTNode* inputList, ASTNode* outputList);
void populateSymbolTableRec(SymbolTableNode* symbolTableNode, ASTNode* statement);
void populateSymbolTable(GlobalRecord* funcRecord, SymbolTableNode* symbolTableNode, ASTNode* moduleDefNode);
void addModuleDeclarationToSymbolTable(ASTNode* moduleDeclarationNode);
void addFunctionToSymbolTable(ASTNode* moduleNode);
void printSymbolTableRec(SymbolTableNode* symbolTableNode);
void printSymbolTable(char* filename);
void printQuadrupleTable(void);
void generateSymbolTable(AST* ast);

#endif