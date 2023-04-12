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

char *getNewLabelVariable() {
    char *label = (char*)malloc(sizeof(char) * 10);
    snprintf(label, 10, "__label_%d", labelCount);
    labelCount++;
    return label;
}

void codeGenerator(QuadrupleTable *qt, char *output) {
    FILE *codefile = fopen(output, "w");

    initASMFile(codefile);    
    // fprintf(codefile, "\tFILL_STACK\n");
    fprintf(codefile, "\tmov rbp, rsp\n");
    Quadruple* currQuad = qt->head;

    while(currQuad!=NULL) {
        
        switch(currQuad->op) {
            case PLUS_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertArithmeticOperation(codefile, currQuad, '+', 'F');
                } else {
                    insertArithmeticOperation(codefile, currQuad, '+', 'I');
                }
                break;
            }
            case MINUS_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertArithmeticOperation(codefile, currQuad, '-', 'F');
                } else {
                    insertArithmeticOperation(codefile, currQuad, '-', 'I');
                }
                break;
            }
            case MUL_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
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
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, '<', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, '<', 'I');
                }
                break;
            }
            case LE_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, 'L', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, 'L', 'I');
                }
                break;
            }
            case GT_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, '>', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, '>', 'I');
                }
                break;
            }
            case GE_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, 'G', 'F');
                } else {
                    insertRelationalOperation(codefile, currQuad, 'G', 'I');
                }
                break;
            }
            case EQ_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, '=', 'F');
                } else if(currQuad->arg1Type == INTEGER && currQuad->arg2Type == INTEGER){
                    insertRelationalOperation(codefile, currQuad, '=', 'I');
                } else {
                    insertRelationalOperation(codefile, currQuad, '=', 'B');
                }
                break;
            }
            case NE_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertRelationalOperation(codefile, currQuad, 'N', 'F');
                } else if(currQuad->arg1Type == INTEGER && currQuad->arg2Type == INTEGER){
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
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertUnaryMinusOperation(codefile, currQuad, 'F');
                } else {
                    insertUnaryMinusOperation(codefile, currQuad, 'I');
                }
                break;
            }
            case ASSIGN_VAR_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertAssignmentOperation(codefile, currQuad, 'F');
                } else if(currQuad->arg1Type == INTEGER && currQuad->arg2Type == INTEGER){
                    insertAssignmentOperation(codefile, currQuad, 'I');
                } else {
                    insertAssignmentOperation(codefile, currQuad, 'B');
                }
                break;
            }
            case ASSIGN_ARRAY_ACCESS_OP: {
                if(currQuad->result->type.array.arrType == INTEGER){
                    insertArrayAssignmentOperation(codefile, currQuad, 'I');
                } else if(currQuad->result->type.array.arrType == BOOLEAN){
                    insertArrayAssignmentOperation(codefile, currQuad, 'B');
                }
            } break;
            case PRINT_ID_OP: {
                if(currQuad->arg1Type == REAL && currQuad->arg2Type == REAL) {
                    insertPrintStatement(codefile, currQuad, 'F');
                } else if(currQuad->arg1Type == INTEGER && currQuad->arg2Type == INTEGER){
                    insertPrintStatement(codefile, currQuad, 'I');
                } else {
                    insertPrintStatement(codefile, currQuad, 'B');
                }
                break;
            }
            case PRINT_ARR_ELE_OP: {
                if(currQuad->arg1ID->type.array.arrType == INTEGER) {
                    insertPrintArrayElementOperation(codefile, currQuad, 'I');
                } else if(currQuad->arg1ID->type.array.arrType == BOOLEAN) {
                    insertPrintArrayElementOperation(codefile, currQuad, 'B');
                }
            } break;
            default: {
                printf("Not handled yet.\n");
                break;
            }
        }
        
        currQuad = currQuad->next;
    }

    fprintf(codefile, "\tpop rbp\n");
    fprintf(codefile, "\tmov rax, 0\n");
    // fprintf(codefile, "\tEMPTY_STACK\n");
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
    fprintf(codefile, "\toutput: db \"Output: %%d\", 0\n");
    fprintf(codefile, "\tnewline: db \"\", 10, 0\n");
    fprintf(codefile, "\tOutOfBoundError: db \"RUNTIME ERROR: Array index out of bounds\", 10, 0\n");
    fprintf(codefile, "\tTypeMismatchError: db \"RUNTIME ERROR: Type Mismatch Error\", 10, 0\n");
    fprintf(codefile, "\tDivBy0Exception: db \"EXCEPTION: Division by 0\", 10, 0\n\n");
    fprintf(codefile, "\ttypeInteger: db \"integer\", 0\n");
    fprintf(codefile, "\ttypeReal: db \"real\", 0\n");
    fprintf(codefile, "\ttypeBoolean: db \"boolean\", 0\n");
}

void insertPrintStatement(FILE *codefile, Quadruple *q, char type) {
    int arg1Offset = -1;
    if(q->isArg1ID) {
        arg1Offset = q->arg1ID->offset;
    }
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\t; Printing an integer\n");
            fprintf(codefile, "\tMOV rdi, output\n");
            fprintf(codefile, "\tmov rsi, qword[rbp-%d]\n", arg1Offset*16);
        } else {
            fprintf(codefile, "\t; Printing an integer\n");
            fprintf(codefile, "\tMOV rdi, output\n");
            fprintf(codefile, "\tmov rsi, %d\n", q->arg1Num);
        }
    }
    fprintf(codefile, "\tcall printf\n");
    fprintf(codefile, "\tMOV rdi, newline\n");
    fprintf(codefile, "\tcall printf\n");
}

void insertPrintArrayElementOperation(FILE *codefile, Quadruple *q, char type){
    /*
        Arg1 -> Array Record
        Arg2 ->
        Two cases:
        1. index is NUM
        2. index is ID
    */

   int range_low = 0, range_high = 0;
   int lower_bound_offset = -1, upper_bound_offset = -1;

    

    int arg1Offset = -1, arg2Offset = -1;
    int arg2IDOffset = -1;
    if(q->isArg1ID){
        arg1Offset = q->arg1ID->offset; // Base Offset of the Array        
    } else {
        printf("Array Record not found\n");
    }

    char* newLabel = NULL;

    if(q->isArg2ID){ // Dynamic reference
        arg2IDOffset = q->arg2ID->offset;
        fprintf(codefile, "\t;Accessing variable offset array element\n");
        fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value
    } else {
        arg2Offset = q->arg2Num; // index value of array element
        fprintf(codefile, "\t;Accessing variable offset array element\n");
        fprintf(codefile, "\tMOV rax, %d\n", arg2Offset); // rax contains index value
    }

    if(q->arg1ID->type.array.isLeftID){ // Left Dynamic Array
        char lower_bound_var[20];
        strcpy(lower_bound_var, q->arg1ID->type.array.leftID);
        Record* lower_bound = variableExists(q->symbolTableNode, lower_bound_var, hash(lower_bound_var));
        if(lower_bound != NULL){
            lower_bound_offset = lower_bound->offset;
            fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", lower_bound_offset); // lower bound value stored in rdx
        } else {
            printf("Lower bound variable not found\n");
        }
    } else { // Left static array
        range_low = q->arg1ID->type.array.left;
        if(q->arg1ID->type.array.leftNegative) range_low *= -1;
        fprintf(codefile, "\tMOV rdx, %d\n", range_low); // lowerbound value stored in rdx
    }

    fprintf(codefile, "\tMOV rbx, rax\n"); // rbx contains index value
    fprintf(codefile, "\tSUB rbx, rdx\n"); // Relative index value stored in rbx
    fprintf(codefile, "\tCMP rbx, 0\n"); // Relative offset should be greater than zero
    newLabel = getNewLabelVariable();
    fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
    fprintf(codefile, "\tMOV rax, 60\n");
    fprintf(codefile, "\tXOR rdi, rdi\n");
    fprintf(codefile, "\tsyscall\n");
    fprintf(codefile, "%s:\tXOR rdx, rdx\n", newLabel); // Lower bound limit satisfied

    if(q->arg1ID->type.array.isRightID){ // Right Dynamic Array
        char upper_bound_var[20];
        strcpy(upper_bound_var, q->arg1ID->type.array.rightID);
        Record *upper_bound = variableExists(q->symbolTableNode, upper_bound_var, hash(upper_bound_var));
        if(upper_bound!=NULL){
            upper_bound_offset = upper_bound->offset;
            fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", upper_bound_offset); // upper bound value stored in rdx
        } else {
            printf("Upper bound variable not found");
        }
    } else { // Right Static array
        range_high = q->arg1ID->type.array.right;
        if(q->arg1ID->type.array.rightNegative) range_high *= -1;        
        fprintf(codefile, "\tMOV rdx, %d\n", range_high); // upperbound value stored in rdx
    }

    fprintf(codefile, "\tMOV rbx, rax\n"); // rbx contains index value
    fprintf(codefile, "\tSUB rbx, rdx\n"); // Relative index value stored in rbx
    fprintf(codefile, "\tCMP rbx, 0\n"); // Relative offset should be greater than zero
    newLabel = getNewLabelVariable();
    fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
    fprintf(codefile, "\tMOV rax, 60\n");
    fprintf(codefile, "\tXOR rdi, rdi\n");
    fprintf(codefile, "\tsyscall\n");
    fprintf(codefile, "%s:\tXOR rdx, rdx\n", newLabel); // Lower bound limit satisfied

    /*
    if(q->isArg2ID){ // Dynamic reference
        arg2IDOffset = q->arg2ID->offset;
        fprintf(codefile, "\t;Accessing variable offset array element\n");
        fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value

        char* newLabel = NULL;

        // Check the index bounds

        if(q->arg1ID->type.array.isRightID){ // Dynamic array with variable upper bound
            char upper_bound_var[20];
            strcpy(upper_bound_var, q->arg1ID->type.array.rightID);
            Record* upper_bound = variableExists(q->symbolTableNode, upper_bound_var, hash(upper_bound_var));
            if(upper_bound != NULL){
                upper_bound = upper_bound->offset;
                fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", upper_bound_offset); // upper bound value stored in rdx    
            } else {
                printf("Upper bound variable not found");
            }
        } else { // Dynamic array with static upper bound
            range_high = q->arg1ID->type.array.right;
            fprintf(codefile, "\tMOV rdx, %d\n", range_high); // Upperbound value stored in rdx
        }

        fprintf(codefile, "\tMOV rbx, rax\n"); // rbx contains index value
        fprintf(codefile, "\tSUB rdx, rbx\n"); // Relative index value stored in rdx
        fprintf(codefile, "\tCMP rdx, 0\n"); // Relative offset should be greater than zero
        newLabel = getNewLabelVariable();
        fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
        fprintf(codefile, "\tMOV rax, 60\n");
        fprintf(codefile, "\tXOR rdi, rdi\n");
        fprintf(codefile, "\tsyscall\n");
        fprintf(codefile, "%s:\tXOR rdx, rdx\n", newLabel); // Upper bound limit statisfied

        if(q->arg1ID->type.array.isLeftID){ // Dynamic array with variable lower bound
            char lower_bound_var[20];
            strcpy(lower_bound_var, q->arg1ID->type.array.leftID);
            Record* lower_bound = variableExists(q->symbolTableNode, lower_bound_var, hash(lower_bound_var));
            if(lower_bound != NULL){
                lower_bound_offset = lower_bound->offset;
                fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", lower_bound_offset); // lower bound value stored in rdx
                
            } else {
                printf("Lower bound variable not found\n");
            }
        } else { // Dynamic array with static lower bound
            range_low = q->arg1ID->type.array.left;
            fprintf(codefile, "\tMOV rdx, %d\n", range_low); // lowerbound value stored in rdx
        }

        fprintf(codefile, "\tMOV rbx, rax\n"); // rbx contains index value
        fprintf(codefile, "\tSUB rbx, rdx\n"); // Relative index value stored in rbx
        fprintf(codefile, "\tCMP rbx, 0\n"); // Relative offset should be greater than zero
        newLabel = getNewLabelVariable();
        fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
        fprintf(codefile, "\tMOV rax, 60\n");
        fprintf(codefile, "\tXOR rdi, rdi\n");
        fprintf(codefile, "\tsyscall\n");
        fprintf(codefile, "%s:\tXOR rdx, rdx\n", newLabel); // Lower bound limit satisfied

        
    } else { // static reference
        arg2Offset = q->arg2Num; // index value of array element
        fprintf(codefile, "\t;Accessing variable offset array element\n");
        fprintf(codefile, "\tMOV rax, %d\n", arg2Offset); // rax contains index value
        // Check the index bounds
        if(q->arg1ID->type.array.isRightID){ // Dynamic array with variable upper bound
            char upper_bound_var[20];
            strcpy(upper_bound_var, q->arg1ID->type.array.rightID);
            Record* upper_bound = variableExists(q->symbolTableNode, upper_bound_var, hash(upper_bound_var));
            if(upper_bound != NULL){
                upper_bound = upper_bound->offset;
                fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", upper_bound_offset); // upper bound value stored in rdx    
            } else {
                printf("Upper bound variable not found");
            }
        } else { // Dynamic array with static upper bound
            range_high = q->arg1ID->type.array.right;
            fprintf(codefile, "\tMOV rdx, %d\n", range_high); // Upperbound value stored in rdx
        }

        

        if(q->arg1ID->type.array.isLeftID){ // Dynamic array with variable lower bound

        } else { // Dynamic array with static lower bound

        }
    }

    */

    switch (type) {
        case 'I': {
            if(q->isArg2ID){ // Index is ID
                arg2IDOffset = q->arg2ID->offset; // Offset of variable which contains index value of array
                // fprintf(codefile, "\t;Accessing variable offset array element\n");
                // fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value
                fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
                fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
                fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
                fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement
                fprintf(codefile, "\tMOV rdi, output\n");
                fprintf(codefile, "\tMOV rbx, rbp\n");
                fprintf(codefile, "\tSUB rbx, rax\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");

            } else {
                arg2Offset = q->arg2Num - range_low; // Index value of array
                fprintf(codefile, "\t;Accessing array element\n");
                fprintf(codefile, "\tMOV rdi, output\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbp-%d]\n", (arg2Offset+arg1Offset)*16);
                // fprintf(codefile, "\tPUSH rbp\n");
            }
        } break;

        case 'B': {
            if(q->isArg2ID){ // Index is ID
                arg2IDOffset = q->arg2ID->offset; // Offset of variable which contains index value of array
                fprintf(codefile, "\t;Accessing variable offset array element\n");
                fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value
                fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
                fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
                fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
                fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement
                fprintf(codefile, "\tMOV rdi, output\n");
                fprintf(codefile, "\tMOV rbx, rbp\n");
                fprintf(codefile, "\tSUB rbx, rax\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");

            } else {
                arg2Offset = q->arg2Num; // Index value of array
                fprintf(codefile, "\tMOV rdi, output\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbp-%d]\n", (arg2Offset+arg1Offset)*16);
                // fprintf(codefile, "\tPUSH rbp\n");
            }
        }break;
    }

    fprintf(codefile, "\tCALL printf\n");
    fprintf(codefile, "\tMOV rdi, newline\n");
    fprintf(codefile, "\tCALL printf\n");
    // fprintf(codefile, "\tPOP rbp\n");
}

void insertArithmeticOperation(FILE *codefile, Quadruple *q, char op, char type) {
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
        fprintf(codefile, "\tMOV QWORD[rbp-%d], xmm0\n", resultOffset*16);        
    }    
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
            fprintf(codefile, "\tADD rax, rbx\n");
            fprintf(codefile, "\tCMP rax, 2\n");
            fprintf(codefile, "\tCMOVE rcx, rdx\n");
            break;
        }
        case '|': {
            fprintf(codefile, "\t; LOGICAL OR\n");
            fprintf(codefile, "\tSUB rax, rbx\n");
            fprintf(codefile, "\tCMP rax, 0\n");
            fprintf(codefile, "\tCMOVNE rcx, rdx\n");
            break;
        }
    }
    fprintf(codefile, "\tMOV qword[rbp-%d], rcx\n", resultOffset*16);
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

void insertAssignmentOperation(FILE *codefile, Quadruple *q, char type) { //only Boolean operations
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
    fprintf(codefile,"\t CMP rcx, rax \n");
    fprintf(codefile,"\t JL exit \n");
    fprintf(codefile,"\t CMP rcx, rbx \n");
    fprintf(codefile,"\t JG exit \n");
}



void insertArrayAssignmentOperation(FILE *codefile, Quadruple *q, char type){
    // Check for static array
    // int lower_bound_offset = 0;
    // int upper_bound_offset = 0;
    int range_low = 0;
    int range_high = 0;

    range_low = q->result->type.array.left;
    range_high = q->result->type.array.right;

    if(q->result->type.array.leftNegative) range_low *= -1;
    if(q->result->type.array.rightNegative) range_high *= -1;

    // if(q->result->type.array.isLeftID){
    //     char lower_bound_var[20];
    //     strcpy(lower_bound_var, q->result->type.array.leftID);
    //     Record* lower_bound = variableExists(q->symbolTableNode, lower_bound_var, hash(lower_bound_var));
    //     if(lower_bound != NULL){
    //         lower_bound_offset = lower_bound->offset;
    //     } else {
    //         printf("Lower bound variable not found\n");
    //     }
    // } else {
    //     range_low = q->result->type.array.left;
    // }

    // if(q->result->type.array.isRightID){
    //     char upper_bound_var[20];
    //     strcpy(upper_bound_var, q->result->type.array.rightID);
    //     Record *upper_bound = variableExists(q->symbolTableNode, upper_bound_var, hash(upper_bound_var));
    //     if(upper_bound!=NULL){
    //         upper_bound_offset = upper_bound->offset;
    //     } else {
    //         printf("Upper bound variable not found");
    //     }
    // } else {
    //     range_high = q->result->type.array.right;
    // }


    int arg1Offset= -1; int arg2Offset = -1;
    if(q->isArg1ID) { //Is a Record storing value to be assigned
        arg1Offset = q->arg1ID->offset;
    }
    if(q->isArg2ID) { //Is a Record storing element number to be accessed
        arg2Offset = q->arg2ID->offset;
    }
    int resultOffset = q->result->offset;


    switch (type)
    {
    case 'I':
    {
        if(q->isArg1ID && q->isArg2ID){
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Variable value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg1Offset*16); // final value of integer stored in rbx
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID==false && q->isArg2ID){ // index in ID and value is INTEGER   arg1=value, arg2=index
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Integer value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg1Num);
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID && q->isArg2ID==false){
            resultOffset += q->arg2Num;
            printf("arg1Offset %d\n", arg1Offset);
            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        } else if(q->isArg1ID==false && q->isArg2ID==false){
            resultOffset += q->arg2Num-range_low;
            // printf("Array element offset: %d\n", resultOffset);
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Num);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        }
    } break;
    
    case 'F': {
        if(q->isArg1ID && q->isArg2ID){
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Variable value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg1Offset*16); // final value of integer stored in rbx
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID==false && q->isArg2ID){ // index in ID and value is INTEGER   arg1=value, arg2=index
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Integer value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg1Num);
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID && q->isArg2ID==false){
            resultOffset += q->arg2Num;
            printf("arg1Offset %d\n", arg1Offset);
            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        } else if(q->isArg1ID==false && q->isArg2ID==false){
            resultOffset += q->arg2Num;
            // printf("Array element offset: %d\n", resultOffset);
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Real);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        }
    } break;
    case 'B': {
        if(q->isArg1ID && q->isArg2ID){
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Variable value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg1Offset*16); // final value of integer stored in rbx
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID==false && q->isArg2ID){ // index in ID and value is INTEGER   arg1=value, arg2=index
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, "\t;Array Assignment variable index, Integer value\n");
            fprintf(codefile, "\tMOV rbx, QWORD[rbp-%d]\n", arg2Offset*16); // Load index value in rbx
            fprintf(codefile, "\tMOV rax, 16\n"); 
            fprintf(codefile, "\tMUL rbx\n"); // stored real offset relative to real base offset 
            fprintf(codefile, "\tMOV rbx, %d\n", resultOffset*16); // Load base array offset in rbx
            fprintf(codefile, "\tADD rax, rbx\n"); // final calue of offset stored in rax
            fprintf(codefile, "\tMOV rbx, %d\n", q->arg1Bool);
            fprintf(codefile, "\tMOV rdx, rbp\n");
            fprintf(codefile, "\tSUB rdx, rax\n");
            fprintf(codefile, "\tMOV QWORD[rdx], rbx\n");

        } else if(q->isArg1ID && q->isArg2ID==false){
            resultOffset += q->arg2Num;
            printf("arg1Offset %d\n", arg1Offset);
            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        } else if(q->isArg1ID==false && q->isArg2ID==false){
            resultOffset += q->arg2Num;
            // printf("Array element offset: %d\n", resultOffset);
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Bool);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        }
    } break;

    default:
        break;
    }
}