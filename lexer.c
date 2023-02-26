#include "lexer.h"

// Buffers
#define BUF_SIZE 512
#define TOK_BUF_SIZE 512
#define MAX_LEXEME_SIZE 20

// make it on heap
static char buf[BUF_SIZE * 2];

// Line Number
static unsigned int LINE_NUM = 1;

// Pointers for the buffer (DFA)
static int begin = 0;
static int forward = 0;
static int state = 0;
static int errno = 0;

Trie* terminalTrie;

// Buffer to store tokens
static TOKEN* token;
// TOKEN tokenBuffer[TOK_BUF_SIZE];

//Program file pointer
static FILE* fp;

static int count = 0;
static int lastBufLoad = 0;
static bool twice = false;


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

void setLexeme(){
    if(token->tok == NUM){
        token->num = atoi(token->lexeme);
    }
    else if(token->tok == RNUM){
        token->rnum = atof(token->lexeme);
    }
}

TOKEN* createToken() {
    if(token == NULL){
        token = malloc(sizeof(TOKEN));
    }
    int pos = 0;
    char lexeme[MAX_LEXEME_SIZE];
    while (begin < forward) {
        lexeme[pos++] = buf[begin % (2 * BUF_SIZE)];
        begin++;
    }
    if(pos < (sizeof(lexeme)/sizeof(char))) lexeme[pos] = '\0';
    strcpy(token->lexeme,lexeme);

    token->linenum = LINE_NUM;
    token->tok = searchWord(terminalTrie, token->lexeme);
    return token;
}

/**
 * Initialise the lexer and its variables
 * @param fpointer (FILE*) File pointer for the program file to be tokenized
 * @return 0 on successful initalization, errno if any error ocurred
*/
int setupLexer(FILE* fpointer){
    fp = fpointer;
    terminalTrie = setupTrie();
    populateTerminalTrie(terminalTrie);
    if(terminalTrie == NULL){
        printf("Trie not set up");
    }
    bufferLoader(fp, true);

    return 0;
}


/**
 * Creates the token from the file;
 * @return TOKEN created or NULL if EOF encountered
*/
TOKEN* getNextToken(){
    char curr;
    bool tokenCreated = false;
    while(true){
        count++;
        curr = buf[forward % (2 * BUF_SIZE)];
        // printf("Char read: %c \t State: %d \t Forward: %d\n", curr, state, forward);
            // printf("Curr State: %d\n", state);

        switch(state) {
            case 0: { // Start State
                if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z')) {
                    state = 1;
                } else if (curr == ' ' || curr == '\t' || curr == '\n') {
                    if (curr == '\n') { 
                        LINE_NUM++;
                    }
                    begin++;
                } else if (curr >= '0' && curr <= '9') {
                    state = 3;
                } else if (curr == '+') {
                    state = 13; 
                } else if (curr == '-') {
                    state = 14;
                } else if (curr == '*') {
                    state = 15;
                } else if (curr == '/') {
                    state = 17;
                } else if (curr == '<') {
                    state = 18;
                } else if (curr == '>') {
                    state = 24;
                } else if (curr == '=') {
                    state = 30;
                } else if (curr == '!') {
                    state = 32;
                } else if (curr == ':') {
                    state = 34;
                } else if (curr == ';') {
                    state = 37;
                } else if (curr == ',') {
                    state = 38;
                } else if (curr == '[') {
                    state = 39;
                } else if (curr == ']') {
                    state = 40;
                } else if (curr == '(') {
                    state = 41;
                } else if (curr == ')') {
                    state = 42;
                } else if (curr == '.') {
                    state = 43;
                } else if(curr == EOF){
                    return NULL;
                } else {
                    state = 100;
                    forward--;
                    errno = 8;
                }
                break;
            }
            case 1: {
                if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z') || (curr >= '0' && curr <= '9')) {
                    state = 1;
                } else {
                    state = 2;
                    forward--;
                }
                break;
            }
            case 2: { // Accept State for ID
                state = 0;
                token = createToken();
                tokenCreated = true;;
                forward--;
                break;
            }
            case 3: {
                if (curr >= '0' && curr <= '9') {
                    state = 3;
                } else if (curr == '.') {
                    state = 5;
                } else {
                    state = 4;
                    forward--;
                }
                break;
            }
            case 4: { // Accept State for NUM
                state = 0;
                token = createToken();
                token->tok = NUM;
                setLexeme();
                forward--;
                tokenCreated = true;;
                break;
            }
            case 5: {
                if (curr == '.') {
                    state = 6;
                    forward -= 2;
                } else if (curr >= '0' && curr <= '9') {
                    state = 7;
                } else {
                    state = 100;
                    errno = 1; // char other than . or integer after .
                }
                break;
            }
            case 6: { // Accept State for NUM
                state = 0;
                token = createToken();
                token->tok = NUM;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 7: {
                if (curr >= '0' && curr <= '9') {
                    state = 7;
                } else if (curr == 'e' || curr == 'E') {
                    state = 9;
                } else {
                    state = 8;
                    forward--;
                }
                break;
            }
            case 8: { // Accept State for RNUM
                state = 0;
                token = createToken();
                token->tok = RNUM;
                setLexeme();
                forward--;
                tokenCreated = true;;
                break;
            } 
            case 9: {
                if (curr >= '0' && curr <= '9') {
                    state = 12;
                } else if (curr == '+' || curr == '-') {
                    state = 10;
                } else {
                    state = 100;
                    forward--; // char other than +/-/0-9 after e
                    errno = 2;
                }
                break;
            }
            case 10: {
                if (curr >= '0' && curr <= '9') {
                    state = 11;
                } else {
                    state = 100;
                    forward--;
                    errno = 3; // non-int after x.x e +/- 
                }
                break;
            }
            case 11: {
                if (curr >= '0' && curr <= '9') {
                    state = 11;
                } else {
                    state = 8;
                    forward--;
                }
                break;
            }
            case 12: {
                if (curr >= '0' && curr <= '9') {
                    state = 12;
                } else {
                    state = 8;
                    forward--;
                }
                break;
            }
            case 13: { // Accept State for PLUS
                state = 0;
                token = createToken();
                token->tok = PLUS;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 14: { // Accept State for MINUS
                state = 0;
                token = createToken();
                token->tok = MINUS;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 15: {
                if(curr == '*'){
                    state = 45;
                } else {
                    state = 16;
                    forward--;
                }
                break;
            }
            case 16: { // Accept State for MUL
                state = 0;
                token = createToken();
                token->tok = MUL;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 17: { // Accept State for DIV
                state = 0;
                token = createToken();
                token->tok = DIV;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 18: {
                if (curr == '=') {
                    state = 20;
                } else if (curr == '<') {
                    state = 21;
                } else { 
                    state = 19;
                    forward--;
                }
                break;
            }
            case 19: { // Accept State for LT
                state = 0;
                token = createToken();
                token->tok = LT;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 20: { // Accept State for LE
                state = 0;
                token = createToken();
                token->tok = LE;
                
                forward--;
                tokenCreated = true;;
                break;
            }
            case 21: {
                if (curr == '<') {
                    state = 23;
                } else { 
                    state = 22;
                    forward--;
                }
                break;
            }
            case 22: { // Accept State for DEF
                state = 0;
                token = createToken();
                token->tok = DEF;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 23: { // Accept State for DRIVERDEF
                state = 0;
                token = createToken();
                token->tok = DRIVERDEF;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 24: {
                if (curr == '=') {
                    state = 26;
                } else if (curr == '>') {
                    state = 27;
                } else {
                    state = 25;
                    forward--;
                }
                break;
            }
            case 25: { // Accept State for GT
                state = 0;
                token = createToken();
                token->tok = GT;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 26: { // Accept State for GE
                state = 0;
                token = createToken();
                token->tok = GE;
                forward--;
                tokenCreated = true;;
                break;                
            }
            case 27: {
                if (curr == '>') {
                    state = 29;
                } else {
                    state = 28;
                    forward--;
                }
                break;
            } 
            case 28: { // Accept State for ENDDEF
                state = 0;
                token = createToken();
                token->tok = ENDDEF;
                forward--;
                tokenCreated = true;;
                break;
            } 
            case 29: { // Accept State for DRIVERENDDEF
                state = 0;
                token = createToken();
                token->tok = DRIVERENDDEF;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 30: {
                if (curr == '=') {
                    state = 31;
                } else {
                    state = 100;
                    forward--;
                    errno = 4; // char other than = after one =
                }
                break;
            }
            case 31: { // Accept State for EQ
                state = 0;
                token = createToken();
                token->tok = EQ;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 32: {
                if (curr == '=') {
                    state = 33;
                } else {
                    state = 100;
                    forward--; 
                    errno = 5; // Not = after ! 
                }
                break;
            } 
            case 33: { // Accept State for NE
                state = 0;
                token = createToken();
                token->tok = NE;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 34: {
                if (curr == '=') {
                    state = 35;
                } else {
                    state = 36;
                    forward--;
                }
                break;
            } 
            case 35: { // Accept State for ASSIGNOP
                state = 0;
                token = createToken();
                token->tok = ASSIGNOP;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 36: { // Accept State for COLON
                state = 0;
                token = createToken();
                token->tok = COLON;
                forward--;
                tokenCreated = true;;
                break;
            } 
            case 37: { // Accept State for SEMICOL
                state = 0;
                token = createToken();
                token->tok = SEMICOL;
                forward--;
                tokenCreated = true;;
                break;
            } 
            case 38: { // Accept State for COMMA
                state = 0;
                token = createToken();
                token->tok = COMMA;
                forward--;
                tokenCreated = true;;
                break;
            } 
            case 39: { // Accept State for SQBO
                state = 0;
                token = createToken();
                token->tok = SQBO;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 40: { // Accept State for SQBC
                state = 0;
                token = createToken();
                token->tok = SQBC;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 41: { // Accept State for BO
                state = 0;
                token = createToken();
                token->tok = BO;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 42: { // Accept State for BC
                state = 0;
                token = createToken();
                token->tok = BC;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 43: {
                if (curr == '.') {
                    state = 44;
                } else {
                    state = 100;
                    forward--;
                    errno = 6; // Not . after .
                }
                break;
            }
            case 44: { // Accept State for RANGEOP
                state = 0;
                token = createToken();
                token->tok = RANGEOP;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 45: { // Comment starts here
                state = 46; 
                token = createToken();
                token->tok = COMMENTMARK;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 46: {
                if (curr == '*') {
                    state = 47;
                } else if (curr == '\n') {
                    LINE_NUM++;
                } 
                else if(curr == EOF){
                    state = 100;
                    forward--;
                    errno = 7;  
                }
                else {
                    state = 46;
                }
                break;
            }
            case 47: {
                if (curr == '*') {
                    state = 48;
                } else if (curr == '\n') {
                    LINE_NUM++;
                } 
                else if(curr == EOF){
                    state = 100;
                    forward--;
                    errno = 7;
                }
                else {
                    state = 46;
                }
                break;
            }
            case 48: { // Comment ends here
                state = 0;
                begin = forward - 2;
                token = createToken();
                token->tok = COMMENTMARK;
                forward--;
                tokenCreated = true;;
                break;
            }
            case 100:{
                char invalidLex[forward-begin];
                int i=0;
                while (begin < forward) {
                    invalidLex[i++] = buf[begin % (2 * BUF_SIZE)];
                    begin++;
                }
                // Prevent printing for unlosed comment
                if(errno!=7){
                    printf("Syntax error at line number %d: \"%s\"; ",LINE_NUM,invalidLex);
                }
                switch(errno){
                    case 1:
                        printf("Invalid character after '.'\n");
                        break;
                    case 2:
                        printf("Invalid character after start of exponent\n");
                        break;
                    case 3:
                        printf("Expected integer after exponent\n");
                        break;
                    case 4:
                        printf("Expected == for equality operator\n");
                        break;
                    case 5:
                        printf("Expected = after ! \n");
                        break;
                    case 6:
                        printf("Invalid .\n");
                        break;
                    case 7:
                        printf("Unclosed comment\n");
                        return NULL;   
                        break;
                    case 8:
                    default:
                        printf("Undetected Syntax Error\n");
                        break;
                }
                state = 101;
                forward--;
                break;
            }

            case 101:{
                if(curr=='\n'){
                    state = 0;
                    LINE_NUM++;
                } 
                else if(curr==';' || curr==' '||curr=='.'){
                    state = 0;
                }
                else{
                    state = 101;
                }
                begin++;
                break;
            }


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
        if(tokenCreated) return token;
    
    }


}
