#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lexer.h"

// Buffers
#define BUF_SIZE 512
char buf[BUF_SIZE * 2];

// Line Number
unsigned int LINE_NUM = 1;

// Pointers for the buffer (DFA)
int begin = 0;
int forward = 0;
int state = 0;

// Buffer to store tokens
TOKEN tokenBuffer[20];

void bufferLoader(FILE* fp, bool firstPart) {
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
        token->lexeme[pos++] = buf[begin++];
    }

    token->linenum = pos;

    return token;
}

int main(int argc, char* argv[]) {
    // if (argc != 2) {
    //     printf("Usage: ./a.out <filename>\n");
    //     return 1;
    // }

    FILE* fp = fopen("testtest", "r");

    int temp = 0;

    // TODO : MODULO of forward pointer

    bufferLoader(fp, true);
    while (true) {
        char curr = buf[forward];
        // if (curr == '\n') { 
        //     LINE_NUM++;
        // }
        switch(state) {
            case 0:
                if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z')) {
                    state = 1;
                } else if (curr == ' ' || curr == '\t' || curr == '\n') {
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
                tokenBuffer[temp++] = *token;
                forward--;
            case 4:
                state = 0;
                break;
            
        }

        forward++;


        if(forward==BUF_SIZE) break;
    }

    for (int i = 0; i < 20; ++i) {
        printf("%s\n", tokenBuffer[i].lexeme);
    }

    fclose(fp);
}