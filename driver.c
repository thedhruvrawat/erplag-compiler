#include "lexer.h"

int main(int argc, char* argv[]){
    if (argc != 2) {
        printf("Usage: ./a.out <filename>\n");
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("File Not Found.\n");
        exit(1);
    }

    setupLexer(fp);

    TOKEN* newToken;
    TOKEN* arr[1024];
    int pos = 0;
    while((newToken = getNextToken())!=NULL){
           
        if(newToken->tok == NUM){
            printf("Line:%d, %s: %d\n", newToken->linenum, *(token_types + (newToken->tok)), newToken->num);
        }
        else if(newToken->tok == RNUM){
            printf("Line:%d, %s: %f\n", newToken->linenum, *(token_types + (newToken->tok)), newToken->rnum);
        }
        else{
            printf("Line:%d, %s: %s\n", newToken->linenum, *(token_types + (newToken->tok)), newToken->lexeme);
        }
        printf("TOKEN object location: %p\n", newToken);
        arr[pos++] = newToken;
    
    }        
    
    fclose(fp);
    exit(0);
}