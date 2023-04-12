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
#include <time.h>
#include "Set.h"
#include "colorCodes.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbolTable.h"
#include "codegen.h"

/**
 * @brief Removes the comments from the input file and outputs the same to console
 *
 * @param testCaseFile
 */
void removeComments(char* testCaseFile)
{
    FILE *input, *output;
    input = fopen(testCaseFile, "r");
    // output = fopen(output, "w");
    if (input == NULL) {
        printf(RED BOLD "Error in opening Test Case File\n" RESET);
        exit(1);
    }
    // if(output == NULL) printf(RED BOLD "Error in opening Output File\n" RESET);
    int state = 0;
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        switch (state) {
        case 0: {
            if (ch == '*') {
                state = 1;
            } else {
                printf("%c", ch);
                state = 0;
            }
            break;
        }
        case 1: {
            if (ch == '*') {
                state = 2;
            } else {
                printf("*%c", ch);
                state = 0;
            }
            break;
        }
        case 2: {
            if (ch == '*') {
                state = 3;
            } else if (ch == '\n' || ch == '\b' || ch == '\t') {
                printf("%c", ch);
                state = 2;
            } else {
                state = 2;
            }
            break;
        }
        case 3: {
            if (ch == '*') {
                state = 0;
            } else if (ch == '\n' || ch == '\b' || ch == '\t') {
                state = 2;
                printf("%c", ch);
            } else {
                state = 2;
            }
            break;
        }
        }
    }
    fclose(input);
    printf(GREEN BOLD "\nFile: %s cleaned of comments\n" RESET, testCaseFile);
    return;
}

/**
 * @brief Takes input file and prints the token list in a formatted manned by invoking the lexer
 *
 * @param userCode
 */
void printTokenList(char* userCode)
{
    FILE* lexerIP = fopen(userCode, "r");
    if (lexerIP == NULL) {
        printf(RED BOLD "File Not Found.\n" RESET);
        exit(1);
    }
    setupLexer(lexerIP);
    TOKEN* newToken;
    int pos = 0;
    printf(UNDERLINE "%-10s%-25s%-10s\n" RESET, "Line No.", "Lexeme", "Token Type");
    while ((newToken = getNextToken())->tok != EOF_SYMBOL) {
        // Not checking for lexeme type as requirement is to print lexeme as it is
        printf("%-10d%-25s%-10s\n", newToken->linenum, newToken->lexeme, *(token_types + (newToken->tok)));
    }
    cleanLexer();
    fclose(lexerIP);
    return;
}

/**
 * @brief Given the input file, invokes the parser and lexer to parse the code and outputs the parse tree to file
 *
 * @param sourceCode
 * @param parseTreeOutput
 */
ParseTree* parseInput(char* sourceCode)
{
    ParseTree* pt = parserMain(sourceCode);
    cleanParser();
    cleanLexer();
    return pt;
}

int main(int argc, char* argv[])
{
    printf("\e[1;1H\e[2J");
    if (argc != 3) {
        printf(YELLOW BOLD "Usage: ./stage1exe <testcase.txt> <outputFile.asm>\n" RESET);
        return 1;
    }

    char* userCode = argv[1];
    char* fileOutput = argv[2];

    setBufferSize(512); // Sets BufferSize to the CLI argument

    printf("---------------------- COMPILER DETAILS GROUP 2 ----------------------\n");
    printf("Team Members: Dhruv, Vaibhav, Shreyas, Swastik, Chirag\n");
    printf("----------------------------------------------------------------------\n");

    printf(GREEN BOLD "Level 4: Symbol table/AST/Type Checking/Semantic rules modules work/Handled static and dynamic arrays in Type Checking and Code Generation\n" RESET);

    int choice = 0;
    char userInput[80];
    while (true) {
        printf(UNDERLINE "Please choose an option:\n" RESET);
        printf(BOLD "[0] Exit\n");
        printf("[1] Lexer: Print the token list generated by the lexer\n");
        printf("[2] Parser: Parse to verify the syntactic correctness of the input source code and to produce parse tree\n");
        printf("[3] AST: Print the Abstract Syntax Tree\n");
        printf("[4] Memory: Display the amount of allocated memory and number of nodes to Parse Tree and AST\n");
        printf("[5] Symbol Table: Print the Symbol Table\n");
        printf("[6] Activation record size: Print the size of activation record for each module\n");
        printf("[7] Static and Dynamic Arrays: Print the type expressions and width of array variables\n");
        printf("[8] Error Reporting: Compile to verify the syntactic and semantic correctness of the input source code\n");
        printf("[9] Code Generation: Generate assembly code for the input source code\n" RESET);


        printf("Enter your choice: ");

        fgets(userInput, sizeof(userInput), stdin);
        if (sscanf(userInput, "%d", &choice) != 1) {
            choice = 5;
        }
        switch (choice) {
        case 0: {
            exit(1);
            break;
        }
        case 1: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            printTokenList(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            printf(GREEN BOLD "Token Stream Over\n" RESET);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 2: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            printParseTree(pt);
            cleanLexer();
            cleanParser();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 3: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            prettyPrintAST();
            cleanLexer();
            cleanParser();
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 4: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            printf("-------------------------------------------------------------------------------------\n");
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            int ASTMemSize = tree->size * sizeof(ASTNode);
            int ParseTreeMemSize = pt->sz * sizeof(ParseTreeNode);
            printf("%-15s Number of Nodes - %d\t", "Parse Tree:", pt->sz);
            printf("Allocated Memory - %d bytes\n", ParseTreeMemSize);
            printf("%-15s Number of Nodes - %d\t", "AST:", tree->size);
            printf("Allocated Memory - %d bytes\n", ASTMemSize);
            printf("Compression Ratio (Count): %.2lf%%\n", (1 - ((double) tree->size) / pt->sz) * 100);
            printf("Compression Ratio (Memory): %.2lf%%\n", (1 - ((double) ASTMemSize) / ParseTreeMemSize) * 100);
            printf("-------------------------------------------------------------------------------------\n");

            cleanLexer();
            cleanParser();
            break;
        }
        case 5: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            generateSymbolTable(tree);
            printSymbolTable("symbolTable.txt");
            printf("-------------------------------------------------------------------------------------\n");
            cleanLexer();
            cleanParser();
            break;
        }
        case 6: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            printf("-------------------------------------------------------------------------------------\n");
            printf(UNDERLINE BOLD "Semantic Analysis\n" RESET);
            generateSymbolTable(tree);
            if (!SEMANTIC_ERROR) {
                printf(GREEN BOLD "The code is semantically correct.\n" RESET);
            }
            printf("-------------------------------------------------------------------------------------\n");
            if (SEMANTIC_ERROR) {
                printf(RED BOLD "Cannot generate activation records due to semantic errors.\n" RESET);
                break;
            }
            printf(UNDERLINE BOLD "Printing the activation records: \n" RESET);
            for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
                GlobalRecord* moduleRecord = symbolTable->global[i];
                while (moduleRecord != NULL) {
                    printf("%-25s%d\n", moduleRecord->name, moduleRecord->activationRecordSize);
                    moduleRecord = moduleRecord->next;
                }
            }
            printf("-------------------------------------------------------------------------------------\n");

            cleanLexer();
            cleanParser();          
            break;
        }
        case 7: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            printf("-------------------------------------------------------------------------------------\n");
            printf(UNDERLINE BOLD "Semantic Analysis\n" RESET);
            generateSymbolTable(tree);
            if (!SEMANTIC_ERROR) {
                printf(GREEN BOLD "The code is semantically correct.\n" RESET);
            }
            printf("-------------------------------------------------------------------------------------\n");
            printf("Printing the type expressions of array variables\n");
            printSymbolTableArray();
            printf("-------------------------------------------------------------------------------------\n");
            cleanLexer();
            cleanParser();
            break;
        }
        case 8: {
            printf("\e[1;1H\e[2J");
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            printf("-------------------------------------------------------------------------------------\n");
            start_time = clock();
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                break;
            }
            ASTCreator(pt);
            printf("-------------------------------------------------------------------------------------\n");
            printf(UNDERLINE BOLD "Semantic Analysis\n" RESET);
            generateSymbolTable(tree);
            if (!SEMANTIC_ERROR) {
                printf(GREEN BOLD "The code is semantically correct.\n" RESET);
            }
            printf("-------------------------------------------------------------------------------------\n");
            end_time = clock();
            total_CPU_time = (double)(end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf(YELLOW BOLD "TOTAL CPU TIME: %f\tTOTAL CPU TIME IN SECONDS: %f sec\n" RESET, total_CPU_time, total_CPU_time_in_seconds);

            cleanLexer();
            cleanParser();
            break;
        }
        case 9: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            ParseTree* pt = parserMain(userCode);
            if (!LEXER_ERROR) {
                printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
            }
            if (!PARSER_ERROR) {
                printf(GREEN BOLD "Input source code is syntactically correct\n" RESET);
            }
            if (PARSER_ERROR || LEXER_ERROR) {
                break;
            }
            ASTCreator(pt);
            printf("-------------------------------------------------------------------------------------\n");
            printf(UNDERLINE BOLD "Semantic Analysis\n" RESET);
            generateSymbolTable(tree);
            if (!SEMANTIC_ERROR) {
                printf(GREEN BOLD "The code is semantically correct.\n" RESET);
            }
            printf("-------------------------------------------------------------------------------------\n");

            if (SEMANTIC_ERROR) {
                printf(RED BOLD "Cannot generate code due to semantic errors.\n" RESET);
                break;
            }

            createQuadrupleTable();
            printQuadrupleTable();
            printf(GREEN BOLD "Intermediate Code generated and printed to quadrupleTable.txt\n" RESET);
            codeGenerator(quadTable, fileOutput);
            printf(GREEN BOLD "Assembly Code generated and printed to code.asm\n" RESET);

            cleanLexer();
            cleanParser();
            break;
        }
        default: {
            printf(RED BOLD "Invalid choice! Try again... \n" RESET);
            break;
        }
        }
    }

    exit(0);
}