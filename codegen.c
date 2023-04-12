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

    initASMFile(codefile);    
    // //fprintf(codefile, "\tFILL_STACK\n");
    fprintf(codefile, "\tmov rbp, rsp\n");
    fprintf(codefile, "\tsub rsp, 16\n");
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
            case MODULE_OP:{
                /* <<module compute_expr>> takes input[ a: integer, b:integer, c:boolean];
                returns [d:integer, e:boolean];
                Function Call =>[u,v] := use module compute_expr[x, y, z]
                */

                fprintf("\n\t;Function %s\n", currQuad->moduleName);
                fprintf("%s:\n", currQuad->moduleName);

                // Getting Actual Input parameters stored in stack and storing them into Formal Parameters
                fprintf(codefile, "\tPUSH rbp\n");
                fprintf(codefile, "\tMOV rbp, rsp\n");

                // Pop the Actual Input parameters of the function into the Formal Parameters offset
                RecordListNode* currRecordNode = currQuad->inputList->head;
                for(int i = 0;i < currQuad->inputList->size; i++){

                    Record* record = currRecordNode->record;
                    VAR_TYPE type = record->type.varType;

                    switch(type){
                        case INT:{
                        }
                        case BOOL:{
                            fprintf(codefile, "\tPOP rax\n");
                            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n",record->offset*16);
                            break;
                        }
                        case ARRAY:{
                            // Base Address
                            fprintf(codefile, "\tPOP rax\n");
                            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n",record->offset*16);

                            // Left Range
                            fprintf(codefile, "\tPOP rax\n");
                            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", record->offset*16+16); // Do I need to subtract -> NO, as offsets keep on increasing

                            // Right Range
                            fprintf(codefile, "\tPOP rax\n");
                            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", record->offset*16+32);
                            break;
                        }
                        case DOUBLE:{
                            fprintf(codefile, "\tPOP xmm0\n");
                            fprintf(codefile, "\tMOVUPS QWORD[rbp-%d], xmm0\n", record->offset);
                            break;
                        }
                        default:{
                            printf("Error: Type incorrect in actual param list Pop\n"); // This shouldn't happen as semantic check done
                            break;
                        }
                    }
                    currRecordNode = currRecordNode->next;
                }
                // TODO - Actual Function Procedure to be written

                // Push the Formal Output parameters of the function into the Stack in reverse order
                currRecordNode = currQuad->outputList->tail;
                for(int i = 0;i < currQuad->outputList->size; i++){
                    Record* record = currRecordNode->record;
                    VAR_TYPE type = record->type.varType;

                    switch(type){
                        case INT:{
                        }
                        case BOOL:{
                            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", record->offset);
                            fprintf(codefile, "\tPUSH rax\n");
                            break;
                        }
                        case DOUBLE:{
                            fprintf(codefile, "\tMOV xmm0, QWORD[rbp-%d]\n", record->offset);
                            fprintf(codefile, "\tPUSH xmm0\n");
                            break;
                        }
                        default:{
                            printf("Error: Type incorrect in formal param list Push\n"); // This shouldn't happen as semantic check done
                            break;
                        }
                    }
                    currRecordNode = currRecordNode->prev;
                }

                // Should I mess with the rsp. 
                fprintf(codefile, "\tMOV rsp, rbp\n");
                fprintf(codefile, "\tPOP rbp\n");
                fprintf(codefile, "\tRET\n");
                
                break;
            }
            case MODULE_USE_OP:{
                // Format of quadruple is currQuad->inputList : input parameters, currQuad->outputList : output parameters, assume that label is given interm of functions parameter
                
                // Push all previous registers in use
                fprintf(codefile, "\n\t; Function Call\n");
                fprintf(codefile, "\tFILL_STACK\t\tPush All registers in use for CALL of function\n");

                // Push Actual Input Parameters of the Function in Stack   
                fprintf(codefile, "\t;Pushing Actual Input Parameters\n");                                      /* This code needs to be written at function call making sprintf("\tpush ebp ;Base Pointer must be preserved across calls\n"); sprintf("\tmov ebp, rsp; Base Pointer currently points to the stack\n"); */
                RecordListNode * currRecordNode = currQuad->inputList->tail;
                for(int i = 0; i < currQuad->inputList->size; i++){ // Inserting from back to front, Easier to take input for function Call
                    
                    Record* record = currRecordNode->record;
                    VAR_TYPE type = record->type.varType;
                    switch(type){
                        case INT:{ // If INT/BOOL directly put in the register
                        }
                        case BOOL:{
                            // How do I know rbp is correct symbol table base pointer for our offset variable
                            fprintf(codefile, "\tMOV rax, QWORD [rbp-%d]\n", (record->offset * 16));
                            fprintf(codefile, "\tPUSH rax\n");
                            break;
                        }
                        case ARRAY:{ // If Array Store Base Address, Range Low and High
                            // POP Pattern -> Base, Left Range, Right Range So, in reverse should be the Pushing order

                            // Push Right Range
                            if(record->type.array.isRightID){
                                // Find record of Right ID
                                Record * RightRec = variableExists(currQuad->symbolTableNode, record->type.array.rightID, hash(record->type.array.rightID));
                                if(RightRec == NULL) {printf("ERROR\n");} // This shouldn't happen

                                fprintf(codefile, "\tMOV rax, QWORD [ebp-%d]\n", (RightRec->offset * 16)); // Move the value of the Identifier to rax
                                if(record->type.array.rightNegative){
                                    fprintf(codefile,"\tMOV rcx, -1\n");
                                    fprintf(codefile, "\tIMUL rax, rcx\n");
                                } 
                            } else {
                                if(record->type.array.rightNegative) // Put the value of the identifier to rax
                                    fprintf(codefile, "\tMOV rax,-%d\n", record->type.array.right);
                                else
                                    fprintf(codefile, "\tMOV rax,%d\n", record->type.array.right);
                            }
                            fprintf(codefile, "\tPUSH rax\n");

                            // Push Left Range
                            if(record->type.array.isLeftID){
                                // Find record of Left ID
                                Record * LeftRec = variableExists(currQuad->symbolTableNode, record->type.array.leftID, hash(record->type.array.leftID));
                                if(LeftRec == NULL) {printf("ERROR\n");} // This shouldn't happen

                                fprintf(codefile, "\tMOV rax, qword[ebp-%d]\n", (LeftRec->offset * 16)); // Move the value of the Identifier to rax
                                if(record->type.array.leftNegative){
                                    fprintf(codefile,"\tMOV rcx, -1\n");
                                    fprintf(codefile, "\tIMUL rax, rcx\n");
                                }
                            } else {
                                if(record->type.array.leftNegative)
                                    fprintf(codefile, "\tMOV rax, -%d\n", record->type.array.left);
                                else
                                    fprintf(codefile, "\tMOV rax,%d\n", record->type.array.left);
                            }
                            fprintf(codefile, "\tPUSH rax\n");

                            // Push Base Address at the end, as first to pop
                            fprintf(codefile, "\tMOV rax,%d\n", (record->offset * 16));
                            fprintf(codefile, "\tPUSH rax\n");
                            break;
                        }
                        case DOUBLE:{
                            // To do 
                            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n");
                            fprintf(codefile, "\tPUSH xmm0\n");
                            // Below is same just more explicit
                            // fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", record->offset * 16);
                            // fprintf(codefile, "\tsub rsp, 16;\n"); //Allocate space on stack
                            // fprintf(codefile, "\tMOVUPS [rsp], xmm0\n"); // Push the value into stack
                            break;
                        }
                        default:{
                            printf("Error in Popping of function parameters\n");
                            break;
                        }
                    }
                    currRecordNode = currRecordNode->prev;
                } 

                // Call the Procedure for the function
                // Note Actual Input parameters are present in the rsp passed to it
                char * label = currQuad->moduleName; 
                // To Do -> Get label of the procedure to be called
                fprintf(codefile, "\tCALL\t%s\t\t;Calling Function\n", label);
                // Note Formal Output parameters are provided by the Called procedure by stack in the correct order

                // Pop i.e. get output parameters to their actual locations
                fprintf(codefile, ";Popping Formal Input Parameters\n");  
                currRecordNode = currQuad->outputList->head;
                for(int i = 0;i < currQuad->outputList->size; i++){
                    Record* record = currRecordNode->record;
                    VAR_TYPE type = record->type.varType;
                    switch(type){
                        case INT:{
                        }
                        case BOOL:{
                            fprintf(codefile, "\tMOV rax,[rsp-%d]\n", (16*(i + 1))); // Get value of the formal output parameter
                            fprintf(codefile, "\tMOV [%d], rax\n", record->offset * 16); // Store it in the actual parameter
                            break;
                        }
                        case DOUBLE:{
                            fprintf(codefile, "\tMOVUPS xmm0, [rsp-%d]\n", (16*(i + 1)));
                            // Do I need to free stack ?
                            fprintf(codefile, "\tMOVUPS [rbp-%d], xmm0\n", record->offset * 16);
                            break;
                        }
                        default:{
                            printf("Error in Popping of function parameters\n");
                            break;
                        }
                    }
                    currRecordNode = currRecordNode->next;
                }
                fprintf(codefile, "\t; Pop back all registers in use\n");
                fprintf(codefile, "\tEMPTY_STACK\n");
                // Pop all previous registers in use
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
    fprintf(codefile, "\tMOV rax, 0\n");
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
    fprintf(codefile, "\toutputReal: db \"Output: %%lf\", 0\n");
    fprintf(codefile, "\toutputTrue: db \"Output: true\", 0\n");
    fprintf(codefile, "\toutputFalse: db \"Output: false\", 0\n");
    fprintf(codefile, "\tinputInt: db \"Input: Enter an integer value \", 0\n");
    fprintf(codefile, "\tinputReal: db \"Input: Enter a real value \", 0\n");
    fprintf(codefile, "\tinputBool: db \"Input: Enter a boolean value \", 0\n");
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
        fprintf(codefile, "\tMOV rdi, input\n");
        fprintf(codefile, "\tMOV rsi, qword[rbp-%d]\n", resultOffset*16);
        fprintf(codefile, "\tCALL scanf\n");
    }
    else if (type == 'B')
    {
        fprintf(codefile, "\t; Getting a boolean\n");
        fprintf(codefile, "\tMOV rdi, input\n");
        fprintf(codefile, "\tMOV rsi, qword[rbp-%d]\n", resultOffset*16);
        fprintf(codefile, "\tCALL scanf\n");
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
        fprintf(codefile, "\tMOV QWORD[rbp-%d], xmm0\n", resultOffset*16);        
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
    fprintf(codefile,"\t CMP rcx, rax \n");
    fprintf(codefile,"\t JL exit \n");
    fprintf(codefile,"\t CMP rcx, rbx \n");
    fprintf(codefile,"\t JG exit \n");
}