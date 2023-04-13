/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef CODEGEN_H
#define CODEGEN_H

#include "intermedCodeGen.h"
#include "codegenDef.h"

char *getNewLabelVariable();
void codeGenerator(QuadrupleTable *qt, char *output);
void initASMFile(FILE *fp);
void initStrings(FILE *fp);
void insertPrintStatement(FILE *codefile, Quadruple *q, char type);
void insertPrintArrayElementOperation(FILE *codefile, Quadruple *q, char type);
void insertArrayAssignmentOperation(FILE *codefile, Quadruple *q, char type);
void insertGetValueStatement(FILE *codefile, Quadruple *q, char type);
void insertArithmeticOperation(FILE *codefile, Quadruple *q, char op, char type);
void insertRelationalOperation(FILE *codefile, Quadruple *q, char op, char type);
void insertLogicalOperation(FILE *codefile, Quadruple *q, char op);
void insertUnaryMinusOperation(FILE *codefile, Quadruple *q, char type);
void insertAssignmentOperation(FILE *codefile, Quadruple *q, char type);
void insertForStatement(FILE *codefile, Quadruple* q);
void insertForEnd(FILE *codefile, Quadruple* q);
void insertWhileLabelStatement(FILE* codefile, Quadruple* q);
void insertWhileStatement(FILE* codefile, Quadruple* q);
void insertWhileEndStatement(FILE* codefile, Quadruple* q);
void insertSwitchStatement(FILE *codefile, Quadruple* q);
void insertGetArrayValue(FILE *codefile, Quadruple *q, char type);
void insertArrayAccessStatement(FILE *codefile, Quadruple *q, char type);

loopStNode *getLoopStackNode(char *label);
loopSt *initLoopStack(void);
loopStNode *peekLoopStack(loopSt *st);
void popLoopStack(loopSt *st);
void pushLoopStack(loopSt *st, char* label);
void destroyLoopStack(loopSt* st);
bool isLoopStackEmpty(loopSt * st);
// void setStackType(loopSt *st, char type);
void insertSwitchStatement(FILE* codefile,Quadruple *q);
void insertSwitchEnd(FILE *codefile, Quadruple* q);
void insertCaseStatement(FILE *codefile, Quadruple* q);
void insertDefaultStatement(FILE *codefile, Quadruple* q);
void insertCaseEnd(FILE *codefile, Quadruple* q);
void insertGetArrayValue(FILE *codefile, Quadruple *q, char type);
void insertDynamicArrayDeclare(FILE *codefile, Quadruple *q);
void insertArrayAccessStatement(FILE *codefile, Quadruple *q, char type);
#endif