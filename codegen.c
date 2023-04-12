/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codegen.h"


int labelCount = 0;
loopSt* lStack;
int boolPrints = 0;
int integer_count = 0;
int real_count = 0;
int bool_count = 0;

char *getNewLabelVariable() {
    char *label = (char*)malloc(sizeof(char) * 10);
    snprintf(label, 10, "__label_%d", labelCount);
    labelCount++;
    return label;
}

void codeGenerator(QuadrupleTable *qt, char *output) {
    FILE *codefile = fopen(output, "w");
    lStack = initLoopStack();

    initASMFile(codefile);    
    // //fprintf(codefile, "\tFILL_STACK\n");
    fprintf(codefile, "\tmov rbp, rsp\n");
    fprintf(codefile, "\tsub rsp, 800\n");
    Quadruple* currQuad = qt->head;

    while(currQuad!=NULL) {
        
        switch(currQuad->op) {
            case PLUS_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertArithmeticOperation(codefile, currQuad, '+', 'F');
                } else {
                    insertArithmeticOperation(codefile, currQuad, '+', 'I');
                }
                break;
            }
            case MINUS_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertArithmeticOperation(codefile, currQuad, '-', 'F');
                } else {
                    insertArithmeticOperation(codefile, currQuad, '-', 'I');
                }
                break;
            }
            case MUL_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertArithmeticOperation(codefile, currQuad, '*', 'F');
                } else {
                    insertArithmeticOperation(codefile, currQuad, '*', 'I');
                }
                break;
            }
            case DIV_OP: {
                insertArithmeticOperation(codefile, currQuad, '/', 'F');
                break;
            }
            case LT_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, '<', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, '<', 'I');
                }
                break;
            }
            case LE_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, 'L', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, 'L', 'I');
                }
                break;
            }
            case GT_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, '>', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, '>', 'I');
                }
                break;
            }
            case GE_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, 'G', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, 'G', 'I');
                }
                break;
            }
            case EQ_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, '=', 'F');
                } else if(currQuad->arg1Type == INT && currQuad->arg2Type == INT){
                    insertRelationalOperation(codefile, currQuad, '=', 'I');
                } else {
                    insertRelationalOperation(codefile, currQuad, '=', 'B');
                }
                break;
            }
            case NE_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertRelationalOperation(codefile, currQuad, 'N', 'F');
                } else if(currQuad->arg1Type == INT && currQuad->arg2Type == INT){
                    insertRelationalOperation(codefile, currQuad, 'N', 'I');
                } else {
                    insertRelationalOperation(codefile, currQuad, 'N', 'B');
                }
                break;
            }
            case AND_OP: {
                insertLogicalOperation(codefile, currQuad, '&');
                break;
            }
            case OR_OP: {
                insertLogicalOperation(codefile, currQuad, '|');
                break;
            }
            case UMINUS_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertUnaryMinusOperation(codefile, currQuad, 'F');
                } else {
                    insertUnaryMinusOperation(codefile, currQuad, 'I');
                }
                break;
            }
            case ASSIGN_VAR_OP: {
                if(currQuad->arg1Type == DOUBLE && currQuad->arg2Type == DOUBLE) {
                    insertAssignmentOperation(codefile, currQuad, 'F');
                } else if(currQuad->arg1Type == INT && currQuad->arg2Type == INT){
                    insertAssignmentOperation(codefile, currQuad, 'I');
                } else {
                    insertAssignmentOperation(codefile, currQuad, 'B');
                }
                break;
            }
            case PRINT_ID_OP: {
                if (currQuad->arg1Type == DOUBLE) {
                    insertPrintStatement(codefile, currQuad, 'F');
                } else if (currQuad->arg1Type == INT) {
                    insertPrintStatement(codefile, currQuad, 'I');
                } else {
                    insertPrintStatement(codefile, currQuad, 'B');
                }
                break;
            }
            case FOR_OP:{
                insertForStatement(codefile, currQuad);
                break;
            }
            case END_OP:{
                if(lStack->size != 0){
                    insertForEnd(codefile, currQuad);
                }
                // When it's the end of file
                else{
                    destroyLoopStack(lStack);
                }
                break;
            }
            case GET_VALUE_OP: {
                if (currQuad->result->type.varType == DOUBLE) {
                    insertGetValueStatement(codefile, currQuad, 'F');
                } else if (currQuad->result->type.varType == INT) {
                    insertGetValueStatement(codefile, currQuad, 'I');
                } else if(currQuad->result->type.varType == BOOL){
                    insertGetValueStatement(codefile, currQuad, 'B');
                } 
                // else {
                //     insertGetValueInArray(codefile, currQuad);
                // }
                break;
            }
            default: {
                printf("Not handled yet.\n");
                break;
            }
        }
        
        currQuad = currQuad->next;
    }
    fprintf(codefile, "\tmov rsp, rbp\n");
    fprintf(codefile, "\tpop rbp\n");
    fprintf(codefile, "\tmov rax, 0\n");
    // //fprintf(codefile, "\tEMPTY_STACK\n");
    fprintf(codefile, "\tret\n");
    fclose(codefile);
}

void initASMFile(FILE *codefile) {
    fprintf(codefile, "extern printf\n");
    fprintf(codefile, "extern scanf\n");

    fprintf(codefile, "%%macro EMPTY_STACK 0\n");
    fprintf(codefile, "\tpop rsi\n");
    fprintf(codefile, "\tpop rdi\n");
    fprintf(codefile, "\tpop rbp\n");
    fprintf(codefile, "\tpop rsp\n");
    fprintf(codefile, "\tpop rdx\n");
    fprintf(codefile, "\tpop rcx\n");
    fprintf(codefile, "\tpop rbx\n");
    fprintf(codefile, "\tpop rax\n");
    fprintf(codefile, "%%endmacro\n\n");

    fprintf(codefile, "%%macro FILL_STACK 0\n");
    fprintf(codefile, "\tpush rax\n");
    fprintf(codefile, "\tpush rbx\n");
    fprintf(codefile, "\tpush rcx\n");
    fprintf(codefile, "\tpush rdx\n");
    fprintf(codefile, "\tpush rsp\n");
    fprintf(codefile, "\tpush rbp\n");
    fprintf(codefile, "\tpush rdi\n");
    fprintf(codefile, "\tpush rsi\n");
    fprintf(codefile, "%%endmacro\n\n");

    fprintf(codefile, "section .data\n");
    initStrings(codefile);
    

    fprintf(codefile, "section .text\n");
    fprintf(codefile, "\tglobal main\n");
    fprintf(codefile, "main:\n");
    fprintf(codefile, "\tpush rbp\n");
}

void initStrings(FILE *codefile) {
    fprintf(codefile, "\tinput: db \"%%d\", 0\n");
    fprintf(codefile, "\toutputInt: db \"Output: %%d\", 10, 0\n");
    fprintf(codefile, "\toutputReal: db \"Output: %%lf\", 10, 0\n");
    fprintf(codefile, "\toutputTrue: db \"Output: true\", 10, 0\n");
    fprintf(codefile, "\toutputFalse: db \"Output: false\", 10, 0\n");
    fprintf(codefile, "\tinputInt: db \"Input: Enter an integer value \", 10, 0\n");
    fprintf(codefile, "\tinputReal: db \"Input: Enter a real value \", 10, 0\n");
    fprintf(codefile, "\tinputBool: db \"Input: Enter a boolean value \", 10, 0\n");
    fprintf(codefile, "\tnewline: db \"\", 10, 0\n");
    fprintf(codefile, "\tOutOfBoundError: db \"RUNTIME ERROR: Array index out of bounds\", 10, 0\n");
    fprintf(codefile, "\tTypeMismatchError: db \"RUNTIME ERROR: Type Mismatch Error\", 10, 0\n");
    fprintf(codefile, "\tDivBy0Exception: db \"EXCEPTION: Division by 0\", 10, 0\n\n");
    fprintf(codefile, "\ttypeInteger: db \"integer\", 0\n");
    fprintf(codefile, "\ttypeReal: db \"real\", 0\n");
    fprintf(codefile, "\ttypeBoolean: db \"boolean\", 0\n");
}

void insertGetValueStatement(FILE *codefile, Quadruple *q, char type) {
    //fprintf(codefile, "\tFILL_STACK\n");
    int resultOffset = q->result->offset;
    if(type == 'I') {
        fprintf(codefile, "\t; Getting an integer\n");
        fprintf(codefile, "\tsection .bss\n");
        fprintf(codefile, "\t\ttemp_integer__%d resq 1\n", integer_count);
        fprintf(codefile, "\tsection .text\n");
        fprintf(codefile, "\tMOV rdi, inputInt\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_integer__%d]\n", integer_count);    

        fprintf(codefile, "\tMOV rdi, input\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tMOV rax, qword [temp_integer__%d]\n", integer_count);
        fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
        integer_count++;
    }
    else if (type == 'F')
    {
        fprintf(codefile, "\t; Getting a real\n");
        fprintf(codefile, "\tsection .bss\n");
        fprintf(codefile, "\t\ttemp_real__%d resq 1\n", real_count);
        fprintf(codefile, "\tsection .text\n");
        fprintf(codefile, "\tMOV rdi, inputReal\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_real__%d]\n", real_count);    

        fprintf(codefile, "\tMOV rdi, input\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tMOV rax, qword [temp_real__%d]\n", real_count);
        fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
        real_count++;
    }
    else if (type == 'B')
    {
        fprintf(codefile, "\t; Getting a boolean\n");
        fprintf(codefile, "\tsection .bss\n");
        fprintf(codefile, "\t\ttemp_boolean__%d resq 1\n", bool_count);
        fprintf(codefile, "\tsection .text\n");
        fprintf(codefile, "\tMOV rdi, inputBool\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_boolean__%d]\n", bool_count);    

        fprintf(codefile, "\tMOV rdi, input\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tMOV rax, qword [temp_boolean__%d]\n", bool_count);
        fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
        bool_count++;
    }
    //fprintf(codefile, "\tEMPTY_STACK\n");
}

void insertPrintStatement(FILE *codefile, Quadruple *q, char type) {
    //fprintf(codefile, "\tFILL_STACK\n");
    int arg1Offset = -1;
    if(q->isArg1ID) {
        arg1Offset = q->arg1ID->offset;
    }
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\t; Printing an integer ID\n");
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);  
            fprintf(codefile, "\tMOV rsi, rax\n");    
            fprintf(codefile, "\tMOV rdi, outputInt\n");
            fprintf(codefile, "\tmov rax, 0\n");
        } else {
            fprintf(codefile, "\t; Printing an integer\n");
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);    
            fprintf(codefile, "\tMOV rdi, outputInt\n");
            fprintf(codefile, "\tmov rsi, rax\n");
        }
    } else if(type == 'F') {
        if(q->isArg1ID) {
            fprintf(codefile, "\t; Printing a real ID\n");
            fprintf(codefile, "\tMOV rdi, outputReal\n");
            fprintf(codefile, "\tMOV rsi, QWORD[rbp-%d]\n", arg1Offset*16);            
        } else {
            fprintf(codefile, "\t; Printing a real number\n");
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOV rdi, outputReal\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
            fprintf(codefile,"\tMOVQ rsi, xmm0\n");
        }
    } else {
        if(q->isArg1ID) {
            fprintf(codefile, "\t; Printing a boolean ID\n");
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tCMP rax, 0\n");
            fprintf(codefile, "\tJE boolVarIsFalse__%d\n", boolPrints);
            fprintf(codefile, "\tMOV rdi, outputTrue\n");
            fprintf(codefile, "\tjmp continue_post_bool__%d\n", boolPrints);
            fprintf(codefile, "boolVarIsFalse__%d:\n", boolPrints);
            fprintf(codefile, "\tMOV rdi, outputFalse\n");
            fprintf(codefile, "continue_post_bool__%d:\n", boolPrints);
        } else {
            fprintf(codefile, "\t; Printing a boolean\n");
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Bool);
            fprintf(codefile, "\tCMP rax, 0\n");
            fprintf(codefile, "\tJE boolIsFalse__%d\n", boolPrints);
            fprintf(codefile, "\tMOV rdi, outputTrue\n");
            fprintf(codefile, "\tjmp continue_post_bool__%d\n", boolPrints);
            fprintf(codefile, "boolIsFalse__%d:\n", boolPrints);
            fprintf(codefile, "\tMOV rdi, outputFalse\n");
            fprintf(codefile, "continue_post_bool__%d:\n", boolPrints);
        }
        boolPrints++;
    }
    fprintf(codefile, "\tcall printf\n");
    // fprintf(codefile, "\tMOV rdi, newline\n");
    // fprintf(codefile, "\tcall printf\n");
    //fprintf(codefile, "\tEMPTY_STACK\n");
}

void insertArithmeticOperation(FILE *codefile, Quadruple *q, char op, char type) {
    //fprintf(codefile, "\tFILL_STACK\n");
    int arg1Offset = -1, arg2Offset = -1, resultOffset = -1;
    if(q->isArg1ID) { //Is a Record
        arg1Offset = q->arg1ID->offset;
    }
    if(q->isArg2ID) { //Is a Record
        arg2Offset = q->arg2ID->offset;
    }
    resultOffset = q->result->offset;
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
        }
        if(q->isArg2ID) {
            fprintf(codefile, "\tMOV rbx, qword[rbp-%d]\n", arg2Offset*16);
        } else {
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg2Num);
        }
        switch(op) {
            case '+': {
                fprintf(codefile, "\t; ADDITION\n");
                fprintf(codefile, "\tADD rax, rbx\n");
                break;
            }
            case '-': {
                fprintf(codefile, "\t; SUBTRACTION\n");
                fprintf(codefile, "\tSUB rax, rbx\n");
                break;
            }
            case '*': {
                fprintf(codefile, "\t; MULTIPLICATION\n");
                fprintf(codefile, "\tIMUL rax, rbx\n");
                break;
            }
            case '/': {
                fprintf(codefile, "\t; DIVISION\n");
                fprintf(codefile, "\tIDIV rax, rbx\n");
                break;
            }
        }
        fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);
    } else { //Floating point arithmetic
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", arg1Offset*16);            
        } else {
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
        }
        if(q->isArg2ID) {
            fprintf(codefile, "\tMOVSD xmm1, QWORD[rbp-%d]\n", arg2Offset*16);            
        } else {
            char *arg2Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg2Label, q->arg2Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm1, [%s]\n", arg2Label);
        }
        switch(op) {
            case '+': {
                fprintf(codefile, "\t; ADDITION\n");
                fprintf(codefile, "\tADDSD xmm0, xmm1\n");
                break;
            }
            case '-': {
                fprintf(codefile, "\t; SUBTRACTION\n");
                fprintf(codefile, "\tSUBSD xmm0, xmm1\n");
                break;
            }
            case '*': {
                fprintf(codefile, "\t; MULTIPLICATION\n");
                fprintf(codefile, "\tMULSD xmm0, xmm1\n");
                break;
            }
            case '/': {
                fprintf(codefile, "\t; DIVISION\n");
                fprintf(codefile, "\tDIVSD xmm0, xmm1\n");
                break;
            }
        }
        fprintf(codefile, "\tMOVSD QWORD[rbp-%d], xmm0\n", resultOffset*16);        
    }    
    //fprintf(codefile, "\tEMPTY_STACK\n");
}


void insertRelationalOperation(FILE *codefile, Quadruple *q, char op, char type) {
    int arg1Offset = -1, arg2Offset = -1, resultOffset = -1;
    if(q->isArg1ID) { //Is a Record
        arg1Offset = q->arg1ID->offset;
    }
    if(q->isArg2ID) { //Is a Record
        arg2Offset = q->arg2ID->offset;
    }
    resultOffset = q->result->offset;
    fprintf(codefile, "\tMOV rcx, 0\n"); //False
    fprintf(codefile, "\tMOV rdx, 1\n"); //True
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
        }
        if(q->isArg2ID) {
            fprintf(codefile, "\tMOV rbx, qword[rbp-%d]\n", arg2Offset*16);
        } else {
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg2Num);
        }
        switch(op) {
            case '<': {
                fprintf(codefile, "\t; LESS THAN\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVL rcx, rdx\n");
                break;
            }
            case 'L': {
                fprintf(codefile, "\t; LESS THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVLE rcx, rdx\n");
                break;
            }
            case '>': {
                fprintf(codefile, "\t; GREATER THAN\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVG rcx, rdx\n");
                break;
            }
            case 'G': {
                fprintf(codefile, "\t; GREATER THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVGE rcx, rdx\n");
                break;
            }
            case '=': {
                fprintf(codefile, "\t; EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVE rcx, rdx\n");
                break;
            }
            case 'N': {
                fprintf(codefile, "\t; NOT EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVNE rcx, rdx\n");
                break;
            }
        }
        fprintf(codefile, "\tMOV qword[rbp-%d], rcx\n", resultOffset*16);
    } else if (type == 'F') { //Floating point arithmetic
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", arg1Offset*16);            
        } else {
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
        }
        if(q->isArg2ID) {
            fprintf(codefile, "\tMOVSD xmm1, QWORD[rbp-%d]\n", arg2Offset*16);            
        } else {
            char *arg2Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg2Label, q->arg2Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm1, [%s]\n", arg2Label);
        }
        switch(op) {
            case '<': {
                fprintf(codefile, "\t; LESS THAN\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVL rcx, rdx\n");
                break;
            }
            case 'L': {
                fprintf(codefile, "\t; LESS THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVLE rcx, rdx\n");
                break;
            }
            case '>': {
                fprintf(codefile, "\t; GREATER THAN\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVG rcx, rdx\n");
                break;
            }
            case 'G': {
                fprintf(codefile, "\t; GREATER THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVGE rcx, rdx\n");
                break;
            }
            case '=': {
                fprintf(codefile, "\t; EQUAL TO\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVE rcx, rdx\n");
                break;
            }
            case 'N': {
                fprintf(codefile, "\t; NOT EQUAL TO\n");
                fprintf(codefile, "\tCMP xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVNE rcx, rdx\n");
                break;
            }
        }
        fprintf(codefile, "\tMOV QWORD[rbp-%d], rcx\n", resultOffset*16);        
    } else { // Boolean
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
        }
        if(q->isArg2ID) {
            fprintf(codefile, "\tMOV rbx, qword[rbp-%d]\n", arg2Offset*16);
        } else {
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg2Num);
        }
        switch(op) {
            case '=': {
                fprintf(codefile, "\t; EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVE rcx, rdx\n");
                break;
            }
            case 'N': {
                fprintf(codefile, "\t; NOT EQUAL TO\n");
                fprintf(codefile, "\tCMP rax, rbx\n");
                fprintf(codefile, "\tCMOVNE rcx, rdx\n");
                break;
            }
        }
        fprintf(codefile, "\tMOV qword[rbp-%d], rcx\n", resultOffset*16);
    }
}

void insertLogicalOperation(FILE *codefile, Quadruple *q, char op) { //only Boolean operations
    int arg1Offset = -1, arg2Offset = -1, resultOffset = -1;
    if(q->isArg1ID) { //Is a Record
        arg1Offset = q->arg1ID->offset;
    }
    if(q->isArg2ID) { //Is a Record
        arg2Offset = q->arg2ID->offset;
    }
    resultOffset = q->result->offset;
    fprintf(codefile, "\tMOV rcx, 0\n"); //False
    fprintf(codefile, "\tMOV rdx, 1\n"); //True
    if(q->isArg1ID) {
        fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
    } else {
        fprintf(codefile, "\tMOV rax, %d\n", q->arg1Bool);
    }
    if(q->isArg2ID) {
        fprintf(codefile, "\tMOV rbx, qword[rbp-%d]\n", arg2Offset*16);
    } else {
        fprintf(codefile, "\tMOV rbx, %d\n", q->arg2Bool);
    }
    switch(op) {
        case '&': {
            fprintf(codefile, "\t; LOGICAL AND\n");
            fprintf(codefile, "\tAND rax, rbx\n");
            // fprintf(codefile, "\tCMP rax, 2\n");
            // fprintf(codefile, "\tCMOVE rcx, rdx\n");
            break;
        }
        case '|': {
            fprintf(codefile, "\t; LOGICAL OR\n");
            fprintf(codefile, "\tOR rax, rbx\n");
            // fprintf(codefile, "\tCMP rax, 0\n");
            // fprintf(codefile, "\tCMOVNE rcx, rdx\n");
            break;
        }
    }
    fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
}

void insertUnaryMinusOperation(FILE *codefile, Quadruple *q, char type) { //only Boolean operations
    int arg1Offset = -1, resultOffset = -1;
    if(q->isArg1ID) { //Is a Record
        arg1Offset = q->arg1ID->offset;
    }
    resultOffset = q->result->offset;
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
        }
        fprintf(codefile, "\t; UNARY MINUS\n");
        fprintf(codefile, "\tNEG rax\n");
        fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);
    } else { //Floating point arithmetic
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", arg1Offset*16);            
        } else {
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
        }
        fprintf(codefile, "\t; UNARY MINUS\n");
        fprintf(codefile, "\tNEG xmm0\n");
        fprintf(codefile, "\tMOV QWORD[rbp-%d], xmm0\n", resultOffset*16);        
    }    
}

void insertAssignmentOperation(FILE *codefile, Quadruple *q, char type) { 
    //fprintf(codefile, "\tFILL_STACK\n");
    int arg1Offset = -1, resultOffset = -1;
    if(q->isArg1ID) { //Is a Record
        arg1Offset = q->arg1ID->offset;
    }
    resultOffset = q->result->offset;

    fprintf(codefile, "\t; ASSIGNMENT OPERATION (NON-ARRAY)\n");
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
        }
        fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);
    } else if(type == 'F'){ //Floating point 
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", arg1Offset*16);            
        } else {
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
        }
        fprintf(codefile, "\tMOV QWORD[rbp-%d], xmm0\n", resultOffset*16);        
    } else { //Boolean
        if(q->isArg1ID) {
            fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Bool);
        }
        fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
    }
    //fprintf(codefile, "\tEMPTY_STACK\n");
}

void dynArrBoundCheck(FILE *codefile, Quadruple* q){

    // rcx has the index, rax and rbx will store the lower and upper bound respectively
    // Go to label exit if not in bounds
    char *leftVar, *rightVar;

    if(q->arg1ID->type.array.isLeftID){
        strcpy(leftVar,q->arg1ID->type.array.leftID);
        int offset = q->arg1ID->offset;
        // Load lower bound into RAX
        fprintf(codefile,"\tMOV rax, [rbp - %d] \n", offset);
    }
    else{
        int leftBound = q->arg1ID->type.array.left;
        // Load lower bound into RAX
        fprintf(codefile,"\tMOV rax, %d \n", leftBound);
    }


    if(q->arg1ID->type.array.isRightID){
        strcpy(leftVar,q->arg1ID->type.array.rightID);
        int offset = q->arg1ID->offset;
        // Load upper bound into RBX
        fprintf(codefile,"\tMOV rbx, [rbp - %d] \n", offset);
    }
    else{
        int rightBound = q->arg1ID->type.array.right;
        // Load upper bound into RBX
        fprintf(codefile,"\tMOV rbx, %d \n", rightBound);
    }
    fprintf(codefile,"\tCMP rcx, rax \n");
    fprintf(codefile,"\tJL exit \n");
    fprintf(codefile,"\tCMP rcx, rbx \n");
    fprintf(codefile,"\tJG exit \n");
}

void insertForStatement(FILE *codefile, Quadruple* q){

    /* Working code for non-nested loop
        fprintf(codefile,"\tMOV rcx, %d\n",q->arg1Num); //Loop start num
        fprintf(codefile,"\tMOV rdx, %d\n",q->arg2Num); //Loop end num

        char *forBlockInit = getNewLabelVariable();
        fprintf(codefile,"%s \t: \n",forBlockInit);

        char *forBlockClose = getNewLabelVariable();
        
        pushLoopStack(lStack, forBlockClose);
        pushLoopStack(lStack, forBlockInit);

        fprintf(codefile,"\tCMP rcx, rdx \n");
        fprintf(codefile,"\tJG %s \n",forBlockClose);
    */

    char *forBlockInit = getNewLabelVariable();
    
    fprintf(codefile, "section .bss\n");
    char *lVar1 = (char*)malloc(sizeof(char) * 16);
    char *lVar2 = (char*)malloc(sizeof(char) * 16);
    snprintf(lVar1, 16, "var_%s_1", forBlockInit);
    snprintf(lVar2, 16, "var_%s_2", forBlockInit);
    fprintf(codefile,"%s resq 1\n",lVar1);
    fprintf(codefile,"%s resq 1\n",lVar2);
    fprintf(codefile, "section .text\n");
    fprintf(codefile,"\tMOV qword [%s], %d\n",lVar1,q->arg1Num); //Loop start num
    fprintf(codefile,"\tMOV qword [%s], %d\n",lVar2,q->arg2Num); //Loop end num

    fprintf(codefile,"%s \t: \n",forBlockInit);
    fprintf(codefile,"\tMOV rcx, qword[%s]\n",lVar1); 
    fprintf(codefile,"\tMOV rdx, qword[%s]\n",lVar2); 

    char *forBlockClose = getNewLabelVariable();
    
    pushLoopStack(lStack, forBlockClose);
    pushLoopStack(lStack, forBlockInit);
    // pushLoopStack(lStack, lVar2);
    pushLoopStack(lStack, lVar1);

    fprintf(codefile,"\tCMP rcx, rdx \n");
    fprintf(codefile,"\tJG %s \n",forBlockClose);
}

void insertForEnd(FILE *codefile, Quadruple* q){
    /* Working code for non-nested loop
    loopStNode* temp = peekLoopStack(lStack);
    char* loopStartLabel = temp->label;

    fprintf(codefile, "\tINC rcx \n");
    fprintf(codefile,"\tJMP %s \n",loopStartLabel);
    popLoopStack(lStack); // popped the start

    loopStNode* temp2 = peekLoopStack(lStack);
    char* loopEndLabel = temp2->label;
    fprintf(codefile,"%s: \n",loopEndLabel);
    popLoopStack(lStack);
    */
    char* lVar1 = peekLoopStack(lStack)->label;
    popLoopStack(lStack);

    // char* lVar2 = peekLoopStack(lStack)->label;
    // fprintf(codefile, "\tMOV qword[%s], rdx",lVar2);
    // popLoopStack(lStack);

    char* loopStartLabel = peekLoopStack(lStack)->label;

    fprintf(codefile,"\tMOV rcx, qword[%s]\n",lVar1); 
    fprintf(codefile, "\tINC rcx \n");
    fprintf(codefile, "\tMOV qword[%s], rcx\n",lVar1);
    fprintf(codefile,"\tJMP %s \n",loopStartLabel);
    popLoopStack(lStack); // popped the start

    char* loopEndLabel = peekLoopStack(lStack)->label;
    fprintf(codefile,"%s: \n",loopEndLabel);
    popLoopStack(lStack);
}

// Stack functions to store labels for loops (nested)

loopStNode *getLoopStackNode(char *label){
    loopStNode* node = malloc(sizeof(loopStNode));
    node->label = label;
    node->next = NULL;
    return node;
}

loopSt *initLoopStack(void){
    loopSt *st = malloc(sizeof(loopSt));
    st->top = NULL;
    st->size = 0;
    return st;
}
loopStNode *peekLoopStack(loopSt *st){
    return st->top;
}
void popLoopStack(loopSt *st){
    if (st->size == 0)
    {
        printf("Loop Stack is empty\n"); // fatal error as lexical should have checked
        return;
    }

    loopStNode *currTop = st->top;

    st->top = st->top->next;
    st->size--;
    if (st->size == 0) {
        free(currTop->label);
    }
    free(currTop);
    
}
void pushLoopStack(loopSt *st, char* label){
    loopStNode *newTop = getLoopStackNode(label);
    newTop->next = st->top;
    newTop->label = label;
    st->top = newTop;
    st->size++;
}

bool isLoopStackEmpty(loopSt * st){
    if(st->size)
        return false;
    else
        return true;
}

void destroyLoopStack(loopSt* st){
    while(!isLoopStackEmpty(st)){
        popLoopStack(st);
    }
    free(st);
}

