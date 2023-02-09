#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lexer.h"
#include <string.h>

// Buffers
#define BUF_SIZE 512
#define TOK_BUF_SIZE 512

char buf[BUF_SIZE * 2];

// Line Number
unsigned int LINE_NUM = 1;

// Pointers for the buffer (DFA)
int begin = 0;
int forward = 0;
int state = 0;

// Buffer to store tokens
TOKEN tokenBuffer[TOK_BUF_SIZE];

void bufferLoader(FILE* fp, bool firstPart) {
    // Setting the buffer end as EOF if the number of characters read is less than BUF_SIZE
    if (firstPart) {
        int val = fread(buf, sizeof(char), BUF_SIZE, fp);
        if (val < BUF_SIZE) {
            buf[val] = EOF;
        }
    } else {
        int val = fread(buf + BUF_SIZE, sizeof(char), BUF_SIZE, fp);
        if (val < BUF_SIZE) {
            buf[val + BUF_SIZE] = EOF;
        }
    }

    return;
}

TOKEN* createToken() {
    TOKEN* token = malloc(sizeof(TOKEN));
    int pos = 0;
    while (begin < forward) {
        token->lexeme[pos++] = buf[begin % (2 * BUF_SIZE)];
        begin++;
    }
    token->linenum = LINE_NUM;
    
    return token;
}

int main(int argc, char* argv[]) {
    int count = 0;
    // if (argc != 2) {
    //     printf("Usage: ./a.out <filename>\n");
    //     return 1;
    // }

    FILE* fp = fopen("testtest", "r");
    int temp = 0;

    bufferLoader(fp, true);
    bool lastBufLoad = 0;
    bool twice = false;

    while (true) {
        count++;
        char curr = buf[forward % (2 * BUF_SIZE)];

        switch(state) {
            case 0:
                if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z')) {
                    state = 1;
                } else if (curr == ' ' || curr == '\t' || curr == '\n') {
                    if (curr == '\n') { 
                        LINE_NUM++;
                    }
                    begin++;
                } else {
                    state = 4;
                }
                break;
            case 1:
                if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z') || (curr >= '0' && curr <= '9')) {
                    state = 1;
                } else {
                    state = 2;
                    forward--;
                }
                break;
            case 2:
                state = 0;
                TOKEN* token = createToken();
                token->tok = ID;
                tokenBuffer[temp++] = *token;
                forward--;
            case 4:
                state = 0;
                break;
        }
        forward++;

        // Loading the buffer when required
        if((forward % (2 * BUF_SIZE)) == (BUF_SIZE - 1) && !lastBufLoad) {
            bufferLoader(fp, false);
            lastBufLoad = 1;
        } else if((forward % (2 * BUF_SIZE)) == (2 * BUF_SIZE - 1) && lastBufLoad) { 
            bufferLoader(fp, true);
            lastBufLoad = 0;
        }

        // Checking if we have reached the end of file
        // twice variable helps to exit the program by keeping track of the number of times EOF encountered
        if (curr == EOF || twice) {
            if (twice) {
                break;
            } else {
                twice = true;
            }
        }
    }
    
    for (int i = 0; i < temp; ++i) {
        printf("%d. Line: %d,\t%s: %s\n", i, tokenBuffer[i].linenum, token_types[tokenBuffer[i].tok], tokenBuffer[i].lexeme);
    }

    fclose(fp);
}
