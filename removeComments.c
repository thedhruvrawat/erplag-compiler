#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "structs.h"

int removeComments(char *testCaseFile, char* outputFile) {
    FILE *input, *output;
    input = fopen(testCaseFile, "r");
    output = fopen(output, "w");
    if(input == NULL) printf(RED BOLD "Error in opening Test Case File\n" RESET);
    if(output == NULL) printf(RED BOLD "Error in opening Output File\n" RESET);
    int state = 0;
    char ch;
    while ((ch = fgetc(input))!=EOF) {        
        switch(state) {
            case 0: {
                if(ch == '*') {
                    state = 1;
                } else {
                    fprintf(output, "%c", ch);
                    state = 0;
                }
                break;
            }
            case 1: {
                if(ch == '*') {
                    state = 2;
                } else {
                    fprintf(output, "*%c", ch);
                    state = 0;
                }
                break;
            }
            case 2: {
                if(ch == '*') {
                    state = 3;
                } else if(ch == '\n' || ch == '\b' || ch == '\t') {
                    fprintf(output, "%c", ch);
                    state = 2;
                } else {
                    state = 2;
                }
                break;
            }
            case 3: {
                if(ch == '*') {
                    state = 0;
                } else if(ch == '\n' || ch == '\b' || ch == '\t') {
                    state = 2;
                    fprintf(output, "%c", ch);
                } else {
                    state = 2;
                }
                break;
            }
        }
    }
    printf(GREEN BOLD "File: %s cleaned of comments, stored in %s" RESET, testCaseFile, outputFile);
    return 0;
}
