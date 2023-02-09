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

int COUNT = 0;

// Buffer to store tokens
TOKEN tokenBuffer[TOK_BUF_SIZE];

void bufferLoader(FILE* fp, bool firstPart) {
    COUNT++;
    if (firstPart) {
        fread(buf, sizeof(char), BUF_SIZE, fp);
    } else {
        fread(buf + BUF_SIZE, sizeof(char), BUF_SIZE, fp);
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
    // begin %= (2 * BUF_SIZE);
    // forward %= (2 * BUF_SIZE);
    
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

    // TODO : MODULO of forward pointer

    bufferLoader(fp, true);
    while (true) {
        count++;
        char curr = buf[forward % (2 * BUF_SIZE)];
        // if (curr == '\n') { 
        //     LINE_NUM++;
        // }
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
        // if(forward==BUF_SIZE) break; //|| forward=='\0'
        if((forward % (2 * BUF_SIZE)) == (BUF_SIZE - 1)) 
            bufferLoader(fp, false);
        else if((forward % (2 * BUF_SIZE)) == (2 * BUF_SIZE - 1)) 
            bufferLoader(fp, true);
        if(count==4000) {
            printf("here\n");
            break;
        }
    }
    
    for (int i = 0; i < TOK_BUF_SIZE; ++i) {
        printf("%d. Line: %d,\t%s: %s\n", i, tokenBuffer[i].linenum, token_types[tokenBuffer[i].tok], tokenBuffer[i].lexeme);
    }

    printf("%d\n", COUNT);
    fclose(fp);
}
