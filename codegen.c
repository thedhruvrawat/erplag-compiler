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
    char *label = (char*)malloc(sizeof(char) * 15);
    snprintf(label, 15, "__label_%d", labelCount);
    labelCount++;
    return label;
}

void codeGenerator(QuadrupleTable *qt, char *output) {
    FILE *codefile = fopen(output, "w");
    lStack = initLoopStack();

    initASMFile(codefile);    
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
                if(currQuad->arg1Type == DOUBLE) {
                    insertUnaryMinusOperation(codefile, currQuad, 'F');
                } else {
                    insertUnaryMinusOperation(codefile, currQuad, 'I');
                }
                break;
            }
            case ASSIGN_VAR_OP: {
                if(currQuad->arg1Type == DOUBLE) {
                    insertAssignmentOperation(codefile, currQuad, 'F');
                } else if(currQuad->arg1Type == INT){
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
            case PRINT_ARR_ELE_OP: {
                if(currQuad->arg1ID->type.array.arrType == INTEGER) {
                    insertPrintArrayElementOperation(codefile, currQuad, 'I');
                } else if(currQuad->arg1ID->type.array.arrType == BOOLEAN) {
                    insertPrintArrayElementOperation(codefile, currQuad, 'B');
                }
            } break;
            case FOR_OP:{
                insertForStatement(codefile, currQuad);
                break;
            }
            case WHILE_EXPR_OP: {
                insertWhileLabelStatement(codefile, currQuad);
                break;
            }
            case WHILE_OP: {
                insertWhileStatement(codefile, currQuad);
                break;
            }
            case WHILE_END_OP: {
                insertWhileEndStatement(codefile, currQuad);
                break;
            }
            case FOR_END_OP:{
                insertForEnd(codefile,currQuad);

                // From END_OP
                // if((lStack->size != 0))
                // {
                //     if(lStack->type == 'F'){
                //         insertForEnd(codefile, currQuad);
                //     }
                //     if(lStack->type == 'S'){
                //         insertSwitchEnd(codefile, currQuad);
                //     }
                // }
                // // When it's the end of file
                // else{
                //     // Commented to prevent seg fault for other END_OP
                //     // destroyLoopStack(lStack);
                // }
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
                else {
                    insertGetArrayValue(codefile, currQuad, 'N');
                }
                break;
            }
            case ARRAY_ACCESS_OP: {
                if (currQuad->result->type.varType == DOUBLE) {
                    insertArrayAccessStatement(codefile, currQuad, 'F');
                } else if (currQuad->result->type.varType == INT) {
                    insertArrayAccessStatement(codefile, currQuad, 'I');
                } else if(currQuad->result->type.varType == BOOL){
                    insertArrayAccessStatement(codefile, currQuad, 'B');
                } 
                else {
                    insertGetArrayValue(codefile, currQuad, 'N');
                }
                break;
            } break;
            case SWITCH_OP:{
                insertSwitchStatement(codefile,currQuad);
                break;
            }
            case CASE_OP:{
                insertCaseStatement(codefile,currQuad);
                break;
            }
            case CASE_END_OP:{
                insertCaseEnd(codefile,currQuad);
                break;
            }
            case SWITCH_END_OP:{
                insertSwitchEnd(codefile, currQuad);
                break;
            }
            case MODULE_OP:{
                /* -> <<module compute_expr>> takes input[ a: integer, b:integer, c:boolean];   returns [d:integer, e:boolean];<-
                Function Call =>[u,v] := use module compute_expr[x, y, z]
                */
                char* name = currQuad->moduleName;
                fprintf(codefile, "%s:\n", name);
                GlobalRecord* moduleRecord = moduleExists(name, hash(name));

                // Getting Actual Input parameters stored in stack and storing them into Formal Parameters
                // fprintf(codefile, "\tPUSH rbp\n");
                moduleRecord->tempInteger = integer_count;
                fprintf(codefile, "\tsection .bss\n");
                fprintf(codefile, "\t\ttemp_integer__%d resq 1\n", integer_count++);
                fprintf(codefile, "\t\ttemp_integer__%d resq 1\n", integer_count++);
                fprintf(codefile, "\tsection .text\n");
                fprintf(codefile, "\tPOP rcx\n");
                fprintf(codefile, "\tMOV [temp_integer__%d], rcx\n", moduleRecord->tempInteger);
                fprintf(codefile, "\tMOV [temp_integer__%d], rbp\n", moduleRecord->tempInteger + 1);
                fprintf(codefile, "\tMOV rbp, rsp\n");

                fprintf(codefile, "\t; Popping Actual Input Param into Formal Param's Offset\n");

                RecordListNode* inputNode = currQuad->inputList->head;
                while (inputNode != NULL) {
                    if (inputNode->type == ARR) {
                        // TODO
                    } else {
                        fprintf(codefile, "\tPOP rax\n");
                        fprintf(codefile, "\tMOV QWORD[rbp - %d], rax\n", inputNode->record->offset * 16);
                    }
                    inputNode = inputNode->next;
                }


                fprintf(codefile, "\tSUB rsp, %d\n", moduleRecord->activationRecordSize * 32);

                break;
            }
            case MODULE_END_OP:{ // MODULE_OP_END
                char* name = currQuad->moduleName;
                GlobalRecord* moduleRecord = moduleExists(name, hash(name));

                fprintf(codefile, "\tADD rsp, %d\n", moduleRecord->activationRecordSize * 32);

                RecordListNode* outputNode = currQuad->outputList->tail;
                while (outputNode != NULL) {
                    if (outputNode->type == ARR) {
                        // todo
                    } else {
                        fprintf(codefile, "\tMOV rax, QWORD[rbp - %d]\n", outputNode->record->offset * 16);
                        fprintf(codefile, "\tPUSH rax\n");
                    }
                    outputNode = outputNode->prev;
                }

                fprintf(codefile, "\tMOV rcx, [temp_integer__%d]\n", moduleRecord->tempInteger);
                fprintf(codefile, "\tPUSH rcx\n");
                fprintf(codefile, "\tMOV rbp, [temp_integer__%d]\n", moduleRecord->tempInteger + 1);
                fprintf(codefile, "\tRET\n");
                break;
            }
            case MODULE_USE_OP:{
                fprintf(codefile, "\tFILL_STACK\n");
                RecordListNode* inputNode = currQuad->inputList->tail;
                while (inputNode != NULL) {
                    if (inputNode->type == ARR) {
                        // TODO
                    } else {
                        if (inputNode->isID) {
                            if (inputNode->isMinus && inputNode->type == DOUBLE) {
                                fprintf(codefile, "\tMOVSD xmm0, QWORD [rbp-%d]\n", (inputNode->record->offset * 16));
                                fprintf(codefile, "\tPXOR xmm1, xmm1\n");
                                fprintf(codefile, "\tSUBS xmm1, xmm0\n");
                                fprintf(codefile, "\tMOVSD xmm0, xmm1\n");
                                fprintf(codefile, "\tMOVQ rax, xmm0\n");
                            } else {
                                fprintf(codefile, "\tMOV rax, QWORD [rbp-%d]\n", (inputNode->record->offset * 16));
                            }
                        } else if (inputNode->type == INT) {
                            fprintf(codefile, "\tMOV rax, %d\n", inputNode->num);
                        } else if (inputNode->type == BOOL) {
                            fprintf(codefile, "\tMOV rax, %d\n", inputNode->boolean);
                        } else if (inputNode->type == DOUBLE) {
                            char *arg1Label = getNewLabelVariable();
                            fprintf(codefile,"\tsection .data\n");
                            fprintf(codefile,"\t%s: dq %f\n", arg1Label, inputNode->rnum);
                            fprintf(codefile,"\tsection .text\n");
                            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
                            if (inputNode->isMinus) {
                                fprintf(codefile, "\tPXOR xmm1, xmm1\n");
                                fprintf(codefile, "\tSUBS xmm1, xmm0\n");
                                fprintf(codefile, "\tMOVSD xmm0, xmm1\n");
                            }
                            fprintf(codefile, "\tMOVQ rax, xmm0\n");
                        }

                        if (inputNode->type != DOUBLE && inputNode->isMinus) {
                            fprintf(codefile, "\tNEG rax\n");
                        }

                        fprintf(codefile, "\tPUSH rax\n");
                    }
                    inputNode = inputNode->prev;
                }

                char* moduleName = currQuad->moduleName;
                fprintf(codefile, "\tCALL %s\n", moduleName);

                RecordListNode* outputNode = currQuad->outputList->head;
                while (outputNode != NULL) {
                    if (outputNode->type == ARR) {
                        // TODO
                    } else {
                        fprintf(codefile, "\tPOP rax\n");
                        if (outputNode->isID) {
                            // if (outputNode->type == DOUBLE) {
                            //     fprintf(codefile, "\tMOVSD xmm0, rax\n");
                            //     fprintf(codefile, "\tMOVSD QWORD [rbp-%d], xmm0\n", (outputNode->record->offset * 16));
                            // } else {
                                fprintf(codefile, "\tMOV QWORD [rbp-%d], rax\n", (outputNode->record->offset * 16));
                            // }
                        }
                    }
                    outputNode = outputNode->next;
                }

                fprintf(codefile, "\tEMPTY_STACK\n");
                break;
            }
            case DRIVER_OP:{
                    fprintf(codefile, "main:\n");
                    fprintf(codefile, "\tpush rbp\n");
                    fprintf(codefile, "\tmov rbp, rsp\n");
                        // //fprintf(codefile, "\tFILL_STACK\n");
                    fprintf(codefile, "\tmov rbp, rsp\n");
                    fprintf(codefile, "\tsub rsp, %d\n", stackEnd * 32);
                    break;
                
            }
            case DRIVER_END_OP:{
                fprintf(codefile, "\tjmp exit\n");

                fprintf(codefile, "InputIsNotIntegerError:\n");
                fprintf(codefile, "\tmov rdi, InputNotInteger\n");
                fprintf(codefile, "\tcall printf\n");
                fprintf(codefile, "\tjmp exit\n");

                fprintf(codefile, "InputIsNotRealError:\n");
                fprintf(codefile, "\tmov rdi, InputNotReal\n");
                fprintf(codefile, "\tcall printf\n");
                fprintf(codefile, "\tjmp exit\n");

                fprintf(codefile, "InputIsNotBooleanError:\n");
                fprintf(codefile, "\tmov rdi, InputNotBoolean\n");
                fprintf(codefile, "\tcall printf\n");
                fprintf(codefile, "\tjmp exit\n");

                fprintf(codefile, "DivBy0:\n");
                fprintf(codefile, "\tMOV rdi, DivBy0Exception\n");
                fprintf(codefile, "\tcall printf\n");
                fprintf(codefile, "\tJMP exit\n");

                fprintf(codefile, "exit:\n");
                fprintf(codefile, "\tmov rsp, rbp\n");
                fprintf(codefile, "\tpop rbp\n");
                fprintf(codefile, "\tMOV rax, 0\n");
                // //fprintf(codefile, "\tEMPTY_STACK\n");
                fprintf(codefile, "\tret\n");
                break;
            }
            case DEFAULT_OP:{
                insertDefaultStatement(codefile,currQuad);
            }
            default: {
                printf("Not handled yet.\n");
                break;
            }
        }
        
        currQuad = currQuad->next;
    }
    
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
}

void initStrings(FILE *codefile) {
    fprintf(codefile, "\tinputInt: db \"%%d\", 0\n");
    fprintf(codefile, "\tinputReal: db \"%%lf\", 0\n");
    fprintf(codefile, "\tinputBool: db \"%%d\", 0\n");
    fprintf(codefile, "\toutputInt: db \"Output: %%d\", 10, 0\n");
    fprintf(codefile, "\toutputReal: db \"Output: %%lf\", 10, 0\n");
    fprintf(codefile, "\toutputTrue: db \"Output: true\", 10, 0\n");
    fprintf(codefile, "\toutputFalse: db \"Output: false\", 10, 0\n");
    fprintf(codefile, "\tinputIntPrompt: db \"Input: Enter an integer value \", 10, 0\n");
    fprintf(codefile, "\tinputRealPrompt: db \"Input: Enter a real value \", 10, 0\n");
    fprintf(codefile, "\tinputBoolPrompt: db \"Input: Enter a boolean value \", 10, 0\n");
    fprintf(codefile, "\tnewline: db \"\", 10, 0\n");
    fprintf(codefile, "\tOutOfBoundError: db \"\033[1;31mRUNTIME ERROR: Array index out of bounds\033[0m\", 10, 0\n");
    fprintf(codefile, "\tTypeMismatchError: db \"\033[1;31mRUNTIME ERROR: Type Mismatch Error\033[0m\", 10, 0\n");
    fprintf(codefile, "\tDivBy0Exception: db \"\033[1;31mEXCEPTION: Division by 0\033[0m\", 10, 0\n\n");
    fprintf(codefile, "\tInputNotInteger: db \"\033[1;31mRUNTIME ERROR: Expected INTEGER: Given input is not an integer\033[0m\", 10, 0\n\n");
    fprintf(codefile, "\tInputNotReal: db \"\033[1;31mRUNTIME ERROR: Expected REAL: Given input is not a real value\033[0m\", 10, 0\n\n");
    fprintf(codefile, "\tInputNotBoolean: db \"\033[1;31mRUNTIME ERROR: Expected BOOLEAN: Given input is not boolean\033[0m\", 10, 0\n\n");
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
        fprintf(codefile, "\tMOV rdi, inputIntPrompt\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_integer__%d]\n", integer_count);    

        fprintf(codefile, "\tMOV rdi, inputInt\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        // cmp eax, 1 jne notinterror
        fprintf(codefile, "\tCMP EAX, 1\n");
        fprintf(codefile, "\tJNE InputIsNotIntegerError\n");
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
        fprintf(codefile, "\tMOV rdi, inputRealPrompt\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_real__%d]\n", real_count);    

        fprintf(codefile, "\tMOV rdi, inputReal\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tCMP EAX, 1\n");
        fprintf(codefile, "\tJNE InputIsNotRealError\n");
        fprintf(codefile, "\tMOVSD xmm0, qword [temp_real__%d]\n", real_count);
        fprintf(codefile, "\tMOVSD qword[rbp-%d], xmm0\n", resultOffset*16);
        real_count++;
    }
    else if (type == 'B')
    {
        fprintf(codefile, "\t; Getting a boolean\n");
        fprintf(codefile, "\tsection .bss\n");
        fprintf(codefile, "\t\ttemp_boolean__%d resq 1\n", bool_count);
        fprintf(codefile, "\tsection .text\n");
        fprintf(codefile, "\tMOV rdi, inputBoolPrompt\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");

        fprintf(codefile, "\tLEA rsi, [temp_boolean__%d]\n", bool_count);    

        fprintf(codefile, "\tMOV rdi, inputBool\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tCMP EAX, 1\n");
        fprintf(codefile, "\tJNE InputIsNotBooleanError\n");
        fprintf(codefile, "\tMOV rax, qword [temp_boolean__%d]\n", bool_count);
        fprintf(codefile, "\tMOV qword[rbp-%d], rax\n", resultOffset*16);
        bool_count++;
    }
    //fprintf(codefile, "\tEMPTY_STACK\n");
}

void insertGetArrayValue(FILE *codefile, Quadruple *q, char type) {
    // static array input
    printf("Getting into array value\n");
    VAR_TYPE arr_type = q->result->type.array.arrType;
    int ele_count = q->result->width - 8;
    if(ele_count >0){
        if (arr_type == INTEGER) ele_count = ele_count/2;
        if (arr_type == REAL) ele_count /= 4;
        else {}
    } else {
        printf("Array size is 0\n");
    }

    int resultOffset = q->result->offset;

    char* newLabel;

    if(arr_type == INT || arr_type == BOOL){
        printf("Getting Into Int\n");
        newLabel = getNewLabelVariable();
        fprintf(codefile, "\tMOV rcx, 0\n");
        fprintf(codefile, "\t; Getting an integer in array\n");
        fprintf(codefile, "\tsection .bss\n");
        fprintf(codefile, "\t\ttemp_integer__%d resq 1\n", integer_count);
        fprintf(codefile, "\tsection .text\n");
        fprintf(codefile, "%s:\tpush rax\n", newLabel);
        fprintf(codefile, "\tpush rcx\n");
        fprintf(codefile, "\tMOV rdi, inputIntPrompt\n");
        fprintf(codefile, "\txor rax, rax\n");
        fprintf(codefile, "\tCALL printf\n");
        fprintf(codefile, "\tPOP rcx\n");
        fprintf(codefile, "\tPOP rax\n");
        fprintf(codefile, "\tPUSH rax\n");
        fprintf(codefile, "\tPUSH rcx\n");

        fprintf(codefile, "\tLEA rsi, [temp_integer__%d]\n", integer_count);    

        fprintf(codefile, "\tMOV rdi, inputInt\n");
        fprintf(codefile, "\tMOV rax, 0\n");    
        
        fprintf(codefile, "\tCALL scanf\n");
        fprintf(codefile, "\tPOP rcx\n");
        fprintf(codefile, "\tPOP rax\n");
        // fprintf(codefile, "\tMOV qword[temp_integer__%d], rax\n");
        fprintf(codefile, "\tMOV rbx, 0\n");
        fprintf(codefile, "\tMOV rbx, qword [temp_integer__%d]\n", integer_count); // integer read stored in rbx
        fprintf(codefile, "\tMOV rax, %d\n", resultOffset); // array offset base
        // fprintf(codefile, "\tMOV rdx, %d\n", ele_count);
        // fprintf(codefile, "\tSUB rdx, rcx\n"); // rdx contains num offset in which number is to be added
        fprintf(codefile, "\tADD rax, rcx\n"); // rax contains final offset in which number is to be added
        fprintf(codefile, "\tMOV rdx, 16\n");
        fprintf(codefile, "\tMUL rdx\n"); // actual offset value stored in rax
        fprintf(codefile, "\tMOV rdx, rbp\n");
        fprintf(codefile, "\tSUB rdx, rax\n"); // 
        fprintf(codefile, "\tMOV qword[rdx], rbx\n");
        fprintf(codefile, "\tADD rcx, 1\n");
        fprintf(codefile, "\tCMP rcx, %d\n", ele_count);
        fprintf(codefile, "\tJL %s\n", newLabel);
        integer_count++;
    }
}

void insertArrayAccessStatement(FILE *codefile, Quadruple *q, char type){
    int range_low = 0, range_high = 0;
    int lower_bound_offset = -1, upper_bound_offset = -1;

    int arg1Offset = -1, arg2Offset = -1;
    int arg2IDOffset = -1;
    if(q->isArg1ID){
        arg1Offset = q->arg1ID->offset; // Base Offset of the Array        
    } else {
        printf("Array Record not found\n");
    }

    if(q->isArg2ID && q->arg2ID->type.varType != INT){
        
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
    fprintf(codefile, "\tmov rdi, OutOfBoundError\n ");
    fprintf(codefile, "\tcall printf\n");
    fprintf(codefile, "\t jmp exit\n");
    fprintf(codefile, "%s:\t\n", newLabel); // Lower bound limit satisfied

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
    fprintf(codefile, "\tSUB rdx, rbx\n"); // Relative index value stored in rbx
    fprintf(codefile, "\tCMP rdx, 0\n"); // Relative offset should be greater than zero
    newLabel = getNewLabelVariable();
    fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
    fprintf(codefile, "\tmov rdi, OutOfBoundError\n ");
    fprintf(codefile, "\tcall printf\n");
    fprintf(codefile, "\t jmp exit\n");
    fprintf(codefile, "%s:\t\n", newLabel); // Lower bound limit satisfied

    if(q->arg1ID->type.array.isLeftID){
        fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", lower_bound_offset); // lower bound value stored in rdx
        fprintf(codefile, "\tSUB rax, rdx\n"); // rax contains relative offset
    } else {
        fprintf(codefile, "\tMOV rdx, %d\n", range_low); // lowerbound value stored in rdx
        fprintf(codefile, "\tSUB rax, rdx\n"); // rax contains relative offset
    }

    fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
    fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
    fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
    fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement

    switch (q->arg1ID->type.array.arrType) {
        case INT: {
            
                // arg2IDOffset = q->arg2ID->offset; // Offset of variable which contains index value of array
                // fprintf(codefile, "\t;Accessing variable offset array element\n");
                // fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value
                // fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
                // fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
                // fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
                // fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement
                fprintf(codefile, "\tMOV rdi, outputInt\n");
                fprintf(codefile, "\tMOV rbx, rbp\n");
                fprintf(codefile, "\tSUB rbx, rax\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");
                fprintf(codefile, "\tMOV QWORD[rbp-%d], rsi\n", q->result->offset*16);

            
        } break;

        case BOOL: {
            fprintf(codefile, "\tMOV rdi, outputFalse\n");
            fprintf(codefile, "\tMOV rbx, rbp\n");
            fprintf(codefile, "\tSUB rbx, rax\n");
            fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rsi\n", q->result->offset*16);

        }break;
    }


}

void insertPrintStatement(FILE *codefile, Quadruple *q, char type) {
    //fprintf(codefile, "\tFILL_STACK\n");
    int arg1Offset = -1;
    if(q->isArg1ID) {
        arg1Offset = q->arg1ID->offset;
    }
    
    if(type == 'I') {
        if(q->isArg1ID) {
            fprintf(codefile, "\t; Printing an integer\n");
            fprintf(codefile, "\tMOV rdi, outputInt\n");
            fprintf(codefile, "\tmov rsi, qword[rbp-%d]\n", arg1Offset*16);
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
            fprintf(codefile, "\tMOVSD xmm0, QWORD[rbp-%d]\n", arg1Offset*16);   
            fprintf(codefile, "\tMOVQ rsi, xmm0\n");   
            fprintf(codefile, "\tMOV rax, 1\n");         
        } else {
            fprintf(codefile, "\t; Printing a real number\n");
            char *arg1Label = getNewLabelVariable();
            fprintf(codefile,"\tsection .data\n");
            fprintf(codefile,"\t%s: dq %f\n", arg1Label, q->arg1Real);
            fprintf(codefile,"\tsection .text\n");
            fprintf(codefile,"\tMOV rdi, outputReal\n");
            fprintf(codefile,"\tMOVSD xmm0, [%s]\n", arg1Label);
            fprintf(codefile,"\tMOVQ rsi, xmm0\n");
            fprintf(codefile, "\tMOV rax, 1\n");         
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
}

void insertPrintArrayElementOperation(FILE *codefile, Quadruple *q, char type) {
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

    if(q->isArg2ID && q->arg2ID->type.varType != INT){
        
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
    fprintf(codefile, "\tmov rdi, OutOfBoundError\n ");
    fprintf(codefile, "\tcall printf\n");
    fprintf(codefile, "\t jmp exit\n");
    fprintf(codefile, "%s:\t\n", newLabel); // Lower bound limit satisfied

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
    fprintf(codefile, "\tSUB rdx, rbx\n"); // Relative index value stored in rbx
    fprintf(codefile, "\tCMP rdx, 0\n"); // Relative offset should be greater than zero
    newLabel = getNewLabelVariable();
    fprintf(codefile, "\tJGE %s\n", newLabel); // ### Error output to be added
    fprintf(codefile, "\tmov rdi, OutOfBoundError\n ");
    fprintf(codefile, "\tcall printf\n");
    fprintf(codefile, "\t jmp exit\n");
    fprintf(codefile, "%s:\t\n", newLabel); // Lower bound limit satisfied

    if(q->arg1ID->type.array.isLeftID){
        fprintf(codefile, "\tMOV rdx, QWORD[rbp-%d]\n", lower_bound_offset); // lower bound value stored in rdx
        fprintf(codefile, "\tSUB rax, rdx\n"); // rax contains relative offset
    } else {
        fprintf(codefile, "\tMOV rdx, %d\n", range_low); // lowerbound value stored in rdx
        fprintf(codefile, "\tSUB rax, rdx\n"); // rax contains relative offset
    }

    fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
    fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
    fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
    fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement

    switch (type) {
        case 'I': {
            
                // arg2IDOffset = q->arg2ID->offset; // Offset of variable which contains index value of array
                // fprintf(codefile, "\t;Accessing variable offset array element\n");
                // fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg2IDOffset*16); // rax contains index value
                // fprintf(codefile, "\tMOV rbx, %d\n", arg1Offset); // rbx contains the base index value
                // fprintf(codefile, "\tADD rax, rbx\n"); // rax contains array element offset value
                // fprintf(codefile, "\tMOV rdx, 16\n"); // Offset multiplier
                // fprintf(codefile, "\tMUL rdx\n"); // rax contains actual offset of arrayelement
                fprintf(codefile, "\tMOV rdi, outputInt\n");
                fprintf(codefile, "\tMOV rbx, rbp\n");
                fprintf(codefile, "\tSUB rbx, rax\n");
                fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");

            
        } break;

        case 'B': {
            fprintf(codefile, "\tMOV rdi, outputFalse\n");
            fprintf(codefile, "\tMOV rbx, rbp\n");
            fprintf(codefile, "\tSUB rbx, rax\n");
            fprintf(codefile, "\tMOV rsi, QWORD[rbx]\n");
            fprintf(codefile, "\tCMP rsi, 0\n");
            newLabel = getNewLabelVariable();
            fprintf(codefile, "\tje %s\n", newLabel);
            fprintf(codefile, "\tmov rdi, outputTrue\n");
            fprintf(codefile, "%s:\t\n", newLabel);
        }break;
    }

    fprintf(codefile, "\tCALL printf\n");
    // fprintf(codefile, "\tMOV rdi, newline\n");
    // fprintf(codefile, "\tCALL printf\n");
    // fprintf(codefile, "\tPOP rbp\n");
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
                //Never Reached
                fprintf(codefile, "\t; DIVISION\n");
                fprintf(codefile, "\tIDIV rax, rbx\n");
                break;
            }
        }
        fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);
    } else { // Floating point arithmetic
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
                fprintf(codefile, "\t; Check if denominator is 0\n");
                fprintf(codefile, "\tXORPS xmm3, xmm3\n");
                fprintf(codefile, "\tUCOMISS xmm1, xmm3\n");
                fprintf(codefile, "\tJE DivBy0\n");
                fprintf(codefile, "\t; DIVISION\n");
                fprintf(codefile, "\tDIVSD xmm0, xmm1\n");
                break;
            }
        }
        fprintf(codefile, "\tMOVSD QWORD[rbp-%d], xmm0\n", resultOffset*16);
        // fprintf(codefile, "\tJMP NoException\n");
        // fprintf(codefile, "NoException:\n");
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
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
                fprintf(codefile, "\tSETB CL\n");
                fprintf(codefile, "\tMOVZX RCX, CL\n");
                break;
            }
            case 'L': {
                fprintf(codefile, "\t; LESS THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
                fprintf(codefile, "\tSETNA CL\n");
                fprintf(codefile, "\tMOVZX RCX, CL\n");
                break;
            }
            case '>': {
                fprintf(codefile, "\t; GREATER THAN\n");
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
                fprintf(codefile, "\tSETA CL\n");
                fprintf(codefile, "\tMOVZX RCX, CL\n");
                break;
            }
            case 'G': {
                fprintf(codefile, "\t; GREATER THAN OR EQUAL TO\n");
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
                fprintf(codefile, "\tSETNB CL\n");
                fprintf(codefile, "\tMOVZX RCX, CL\n");
                break;
            }
            case '=': {
                fprintf(codefile, "\t; EQUAL TO\n");
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
                fprintf(codefile, "\tCMOVE rcx, rdx\n");
                break;
            }
            case 'N': {
                fprintf(codefile, "\t; NOT EQUAL TO\n");
                fprintf(codefile, "\tCOMISD xmm0, xmm1\n");
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
        fprintf(codefile, "\tPXOR xmm1, xmm1\n");
        fprintf(codefile, "\tSUBPS xmm1, xmm0\n");
        fprintf(codefile, "\tMOVSD QWORD[rbp-%d], xmm1\n", resultOffset*16);        
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
        fprintf(codefile, "\tMOVSD QWORD[rbp-%d], xmm0\n", resultOffset*16);        
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



void insertArrayAssignmentOperation(FILE *codefile, Quadruple *q, char type) {
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

    fprintf(codefile, "\t;Array Assignment\n");
    switch (type)
    {
    case 'I':
    {
        if(q->isArg1ID && q->isArg2ID){
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, ";arg1Offset %d\n", arg1Offset);
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

        } else if(!q->isArg1ID && q->isArg2ID){ // index in ID and value is INTEGER   arg1=value, arg2=index
            int arrOffset = 0;
            printf("ArrayElementOffset %d\n", arrOffset);
            fprintf(codefile, ";arg1Offset %d\n", arg1Offset);
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

        } else if(q->isArg1ID && !q->isArg2ID){
            resultOffset += q->arg2Num-range_low;
            printf("resultOffset %d\n", resultOffset);
            fprintf(codefile, ";arg1Offset %d\n", arg1Offset);
            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        } else if(!q->isArg1ID && !q->isArg2ID){
            resultOffset += q->arg2Num-range_low;
            fprintf(codefile, ";arg1Offset %d\n", arg1Offset);
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
            fprintf(codefile, "\tMOV rax, %f\n", q->arg1Real);
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
            resultOffset += q->arg2Num - range_low;
            printf("arg1Offset %d\n", arg1Offset);
            fprintf(codefile, "\tMOV rax, QWORD[rbp-%d]\n", arg1Offset*16);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        } else if(q->isArg1ID==false && q->isArg2ID==false){
            resultOffset += q->arg2Num - range_low;
            // printf("Array element offset: %d\n", resultOffset);
            fprintf(codefile, "\tMOV rax, %d\n", q->arg1Bool);
            fprintf(codefile, "\tMOV QWORD[rbp-%d], rax\n", resultOffset*16);

        }
    } break;

    default:
        break;
    }
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
    fprintf(codefile,";FOR loop start\n");
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
    fprintf(codefile,"\tMOV [rbp-%d],rcx\n",(q->result->offset)*16);

    char *forBlockClose = getNewLabelVariable();
    
    // setStackType(lStack,'F');
    pushLoopStack(lStack, forBlockClose);
    pushLoopStack(lStack, forBlockInit);
    // pushLoopStack(lStack, lVar2);
    pushLoopStack(lStack, lVar1);

    fprintf(codefile,"\tCMP rcx, rdx \n");
    fprintf(codefile,"\tJG %s \n", forBlockClose);
}

void insertWhileLabelStatement(FILE* codefile, Quadruple* q) {
    char *whileBlockInit = getNewLabelVariable();
    char *whileBlockClose = getNewLabelVariable();
    fprintf(codefile, "%s: \n", whileBlockInit);
    pushLoopStack(lStack, whileBlockInit);
    pushLoopStack(lStack, whileBlockClose);

    return;
}

void insertWhileStatement(FILE* codefile, Quadruple* q) {
    if (q->isArg1ID) {
        fprintf(codefile,"\tMOV rcx, [rbp - %d]\n", q->arg1ID->offset * 16);
    } else if (q->arg1Bool) {
        fprintf(codefile,"\tMOV rcx, 1\n");
    } else {
        fprintf(codefile,"\tMOV rcx, 0\n");
    }

    char* whileBlockClose = peekLoopStack(lStack)->label;
    fprintf(codefile,"\tCMP rcx, 0 \n");
    fprintf(codefile,"\tJE %s \n", whileBlockClose);

    return;
}

void insertWhileEndStatement(FILE* codefile, Quadruple* q) {
    char whileBlockClose[15];
    strcpy(whileBlockClose, peekLoopStack(lStack)->label);
    popLoopStack(lStack);
    char whileBlockInit[15];
    strcpy(whileBlockInit, peekLoopStack(lStack)->label);
    popLoopStack(lStack);

    printf("whileBlockInit: %p\n", whileBlockInit);
    fprintf(codefile,"\tJMP %s \n", whileBlockInit);
    fprintf(codefile,"%s: \n", whileBlockClose);

    return;
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
    fprintf(codefile,";FOR loop end\n");
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
    // st->type = 'N';
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
        // st->type = 'N';
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

// void setStackType(loopSt *st, char type){
//     st->type = type;
// }

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

void insertSwitchStatement(FILE *codefile, Quadruple* q){
    char* switchVar = malloc(sizeof(char)*21);
    switchVar = q->arg1ID->name;
    int switchVarOff = variableExists(q->symbolTableNode,switchVar,hash(switchVar))->offset;
    fprintf(codefile,"; SWITCH STATEMENT\n");
    fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", switchVarOff*16); 
    //INT value is stored in case of INT switch, else 0/1 (false/true)

    char* switchEndLabel = (char*)malloc(sizeof(char) * 20);
    strcpy(switchEndLabel,getNewLabelVariable());
    // setStackType(lStack,'S');
    pushLoopStack(lStack,switchEndLabel);
}

void insertSwitchEnd(FILE *codefile, Quadruple* q){
    char *switchEndLabel = peekLoopStack(lStack)->label;
    fprintf(codefile,"%s: \n",switchEndLabel);
    popLoopStack(lStack);
}

void insertCaseStatement(FILE *codefile, Quadruple* q){
    char* nextCaseStartLabel = getNewLabelVariable();
    pushLoopStack(lStack,nextCaseStartLabel);
    if(q->arg1Type == INT){
        fprintf(codefile,"\tMOV rbx, %d\n",q->arg1Num);
    }
    else{ //BOOL
        if(q->arg1Bool == true){
            fprintf(codefile,"\tMOV rbx, %d\n",1);
        }
        else{
            fprintf(codefile,"\tMOV rbx, %d\n",0);
        }
    }
    fprintf(codefile,"\tCMP rax,rbx\n");
    fprintf(codefile,"\tJNZ %s\n",nextCaseStartLabel);
}

void insertCaseEnd(FILE *codefile, Quadruple* q){
    char nextCaseStartLabel[20];
    strcpy(nextCaseStartLabel,peekLoopStack(lStack)->label);
    popLoopStack(lStack);
    char switchEndLabel[20];
    strcpy(switchEndLabel,peekLoopStack(lStack)->label);
    fprintf(codefile,"JMP %s\n",switchEndLabel);
    fprintf(codefile,"%s: \n",nextCaseStartLabel);
}

void insertDefaultStatement(FILE *codefile, Quadruple* q){
    
    char* nextCaseStartLabel = getNewLabelVariable();
    // // char nextCaseStartLabel[20];
    pushLoopStack(lStack,nextCaseStartLabel);
    // strcpy(nextCaseStartLabel,peekLoopStack(lStack)->label);
    // popLoopStack(lStack);
    // fprintf(codefile,"%s: \n",nextCaseStartLabel);
}

// Dynamic Array - TODO
// void insertDynamicArrayDeclare(FILE *codefile, Quadruple* q){
//     char leftID[20];
//     char rightID[20];
//     int leftIDOffset, rightIDOffset;

//     // Left dynamic
//     bool isLeftID = q->result->type.array.isLeftID;
//     bool isRightID = q->result->type.array.isRightID;
//     //
//     if(!isLeftID && !isRightID){
//         printf("Fatal error, staic array treated as mistake\n");
//     }
//     if(isLeftID){
//         //Left Dynamic
//         strcpy(leftID,q->result->type.array.leftID);
//         leftIDOffset = (variableExists(q->symbolTableNode,leftID,hash(leftID)))->offset;
//         fprintf(codefile, "\tMOV rax, qword[rbp-%d]\n", leftIDOffset*16);
        
//     }
//     else{
//         // Left Static
//         fprintf(codefile, "\tMOV rax, %d]\n", q->result->type.array.left); 
//     }
//     // Unary negative
//     if(q->result->type.array.leftNegative){
//         fprintf(codefile,"\tNEG rax\n");
//     } 


//     if(rightID){
//         //Right Dynamic
//         strcpy(rightID,q->result->type.array.rightID);
//         rightIDOffset = (variableExists(q->symbolTableNode,rightID,hash(rightID)))->offset;
//         fprintf(codefile, "\tMOV rbx, qword[rbp-%d]\n", rightIDOffset*16);
//     }
//     else{
//         //Right Static
//         fprintf(codefile, "\tMOV rbx, %d]\n", q->result->type.array.right); 
//     }

//     fprintf(codefile,"section .bss\n");

//     fprintf(codefile,"section .text\n");
// }
