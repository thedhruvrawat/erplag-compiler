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
ParseTree* parseInput(char* sourceCode, char* parseTreeOutput)
{
    ParseTree* pt = parserMain(sourceCode, parseTreeOutput);
    cleanParser();
    cleanLexer();
    return pt;
}

int main(int argc, char* argv[])
{
    printf("\e[1;1H\e[2J");
    if (argc != 4) {
        printf(YELLOW BOLD "Usage: ./stage1exe <testcase.txt> <parsetreeOutFile.txt> <size_of_buffer>\n" RESET);
        return 1;
    }

    char* userCode = argv[1];
    char* treeOutput = argv[2];

    if (strlen(argv[3]) >= 17) {
        printf(RED BOLD "Buffer Size too large.\n" RESET);
        return 1;
    }
    long long buf_size = atoll(argv[3]);
    if (buf_size <= 0) {
        printf(RED BOLD "Invalid buffer size. Should be a positive number.\n" RESET);
        return 1;
    } else if (buf_size <= 20) {
        printf(YELLOW BOLD "WARNING: Buffer size not adequate for literals of size 20.\n" RESET);
    } else if (buf_size >= (1 << 20)) {
        printf(RED BOLD "Buffer Size too large.\n" RESET);
        return 1;
    }
    setBufferSize(buf_size); // Sets BufferSize to the CLI argument

    printf("---------------------- COMPILER DETAILS GROUP 2 ----------------------\n");
    printf("Team Members: Dhruv, Vaibhav, Shreyas, Swastik, Chirag\n");
    printf("----------------------------------------------------------------------\n");
    printf(GREEN BOLD "i. FIRST and FOLLOW sets automated\n");
    printf("ii. Both lexical and syntax analysis modules implemented\n");
    printf("iii. Modules work with all given testcases\n");
    printf("iv. Parse tree constructed and saved in file\n\n" RESET);

    int choice = 0;
    char userInput[80];
    while (true) {
        printf(UNDERLINE "Please choose an option:\n" RESET);
        printf(BOLD "[0] Exit\n");
        printf("[1] Remove comments and print on console\n");
        printf("[2] Print the token list generated by the lexer on console\n");
        printf("[3] Parse the user source code and save parse tree\n");
        printf("[4] Print total time taken by stage 1\n" RESET);

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
            removeComments(userCode);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 2: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            printTokenList(userCode);
            printf(GREEN BOLD "Token Stream Over\n" RESET);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 3: {
            printf("\e[1;1H\e[2J");
            printf("-------------------------------------------------------------------------------------\n");
            parseInput(userCode, treeOutput);
            printf(GREEN BOLD "Parsing Over\n" RESET);
            printf("-------------------------------------------------------------------------------------\n");
            break;
        }
        case 4: {
            printf("\e[1;1H\e[2J");
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            printf("-------------------------------------------------------------------------------------\n");
            start_time = clock();
            ParseTree* pt = parseInput(userCode, treeOutput);
            end_time = clock();
            total_CPU_time = (double)(end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf(YELLOW BOLD "TOTAL CPU TIME: %f\tTOTAL CPU TIME IN SECONDS: %f sec\n" RESET, total_CPU_time, total_CPU_time_in_seconds);
            printf("-------------------------------------------------------------------------------------\n");
            // TESTING AST CREATION;;; TO BE REMOVED LATER
            if (PARSER_ERROR || LEXER_ERROR) {
                printf(RED BOLD "Cannot generate AST due to lexical/syntactical errors.\n" RESET);
                break;
            }
            ASTCreator(pt);
            generateSymbolTable(tree);
            printSymbolTable();
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