#include "lexer.h"

int main(int argc, char* argv[]){
    if (argc != 2) {
        printf(YELLOW BOLD "Usage: ./a.out <filename>\n" RESET);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf(RED BOLD "File Not Found.\n" RESET);
        exit(1);
    }

    setupLexer(fp);

    TOKEN* newToken;
    TOKEN* arr[1024];
    int pos = 0;
    while((newToken = getNextToken())!=NULL){
        
        // Not checking for lexeme type as requirement is to print lexeme as it is
        printf("Line:%d, %s: %s\n", newToken->linenum, *(token_types + (newToken->tok)), newToken->lexeme);
        printf("TOKEN object location: %p\n", newToken);
        arr[pos++] = newToken;
    
    }        
    
    fclose(fp);
    exit(0);
}