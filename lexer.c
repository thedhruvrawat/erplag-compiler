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
#include "lexer.h"
#include "colorCodes.h"
#include "trie.h"

// Buffers
long long BUF_SIZE = 512;

const char* token_types[] = {
    "INTEGER", "REAL", "BOOLEAN", "OF", "ARRAY", "START",
    "END", "DECLARE", "MODULE", "DRIVER", "PROGRAM", "GET_VALUE",
    "PRINT", "USE", "WITH", "PARAMETERS", "TAKES", "INPUT",
    "RETURNS", "FOR", "IN", "SWITCH", "CASE", "BREAK",
    "DEFAULT", "WHILE",
    // Non-Keywords
    "ID", "NUM", "RNUM",
    "PLUS", "MINUS", "MUL", "DIV",
    "LT", "LE", "GE", "GT", "EQ", "NE",
    "DEF", "ENDDEF", "DRIVERDEF", "DRIVERENDDEF",
    "COLON", "RANGEOP", "SEMICOL", "COMMA", "ASSIGNOP",
    "SQBO", "SQBC", "BO", "BC", "COMMENTMARK",
    // Boolean Operators
    "TRUE", "FALSE", "AND", "OR",
    "EPS", "EOF_SYMBOL"
};

/**
 * @brief Sets the size of the buffer
 *
 * @param newBufSize
 */
void setBufferSize(long long newBufSize)
{
    BUF_SIZE = newBufSize;
}

#define MAX_LEXEME_SIZE 20

// made it on heap
char* buf;

/**
 * @brief Allocates memory for the buffer from heap
 *
 */
void initbuf(void)
{
    buf = malloc(sizeof(char) * (BUF_SIZE * 2));
    memset(buf, 0, BUF_SIZE * 2);
    return;
}

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

// Program file pointer
static FILE* fp;

static int count = 0;
static int lastBufLoad = 0;
static bool twice = false;
static bool LexCorrPrint = true; // Printed Lexically Correct

/**
 * @brief Checks whether the given characters is in the Synchronization Set A
 *
 * @param c
 * @return true
 * @return false
 */
bool inSyncSetA(char c)
{
    switch (c) {
    case ' ':
        return true;
        break;

    case ';':
        return true;
        break;

    case '+':
        return true;
        break;

    case '-':
        return true;
        break;

    default:
        return false;
        break;
    }
}

/**
 * @brief Checks whether the given character is in Synchronization Set B
 *
 * @param c
 * @return true
 * @return false
 */
bool inSyncSetB(char c)
{
    switch (c) {
    case '.':
        return true;

    default:
        return false;
    }
}

/**
 * @brief Loads one half of the twin buffer with user code from the file
 *
 * @param fp
 * @param firstPart
 */
void bufferLoader(FILE* fp, bool firstPart)
{
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

/**
 * @brief Calculates and stores the numerical value of the lexeme in the token structure if of type NUM or RNUM
 *
 */
void setLexeme()
{
    if (token->tok == NUM) {
        token->num = atoi(token->lexeme);
    } else if (token->tok == RNUM) {
        token->rnum = atof(token->lexeme);
    }
}

/**
 * @brief Creates a new token with its lexeme and token type properly assigned if keyword, otherwise set to ID
 *
 * @return TOKEN*
 */
TOKEN* createToken()
{
    if (token == NULL) {
        token = malloc(sizeof(TOKEN));
    }
    int pos = 0;
    char lexeme[MAX_LEXEME_SIZE + 1]; // +1 To incorporate \0 at the end of char lexeme[]
    while (begin < forward) {
        lexeme[pos++] = buf[begin % (2 * BUF_SIZE)];
        begin++;
    }
    if (pos < (sizeof(lexeme) / sizeof(char)))
        lexeme[pos] = '\0';
    strcpy(token->lexeme, lexeme);

    token->linenum = LINE_NUM;
    token->tok = searchWord(terminalTrie, token->lexeme);
    return token;
}

/**
 * @brief Sets up the lexer by setting up the keywords trie and the twin buffer
 *
 * @param fpointer
 * @return int
 */
int setupLexer(FILE* fpointer)
{
    fp = fpointer;
    terminalTrie = setupTrie();
    populateTerminalTrie(terminalTrie);
    if (terminalTrie == NULL) {
        printf(RED BOLD "Trie not set up" RESET);
    }
    initbuf(); // Malloc memory to Buffer based on BUF_SIZE value
    bufferLoader(fp, true);
    return 0;
}

/**
 * @brief Used to free all memory allocated and resets the variables for the next run
 *
 */
void cleanLexer()
{
    free(buf);
    buf = NULL;

    // Line Number
    LINE_NUM = 1;

    // Pointers for the buffer (DFA)
    begin = 0;
    forward = 0;
    state = 0;
    errno = 0;
    LexCorrPrint = true;

    freeTrie(terminalTrie);
    terminalTrie = NULL;

    count = 0;
    lastBufLoad = 0;
    twice = false;
    return;
}

/**
 * @brief Interface for the parser to interact with the lexer; Returns the next token.
 *
 * @return TOKEN*
 */
TOKEN* getNextToken()
{
    char curr;
    bool tokenCreated = false;
    while (true) {
        count++;
        curr = buf[forward % (2 * BUF_SIZE)];

        switch (state) {
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
            } else if (curr == EOF) {
                if (errno == 0 && (LexCorrPrint)) {
                    printf(GREEN BOLD "Input source code is lexically correct\n" RESET);
                    LexCorrPrint = false;
                }
                token = createToken();
                strcpy(token->lexeme, "EOF");
                token->linenum = LINE_NUM;
                token->tok = EOF_SYMBOL;
                return token;
            } else {
                state = 100;
                forward--;
                errno = 8;
            }
            break;
        }
        case 1: {
            if (curr == '_' || (curr >= 'a' && curr <= 'z') || (curr >= 'A' && curr <= 'Z') || (curr >= '0' && curr <= '9')) {
                if (forward - begin >= MAX_LEXEME_SIZE) {
                    state = 100;
                    errno = 9;
                    forward--;
                } else {
                    state = 1;
                }
            } else {
                state = 2;
                forward--;
            }
            break;
        }
        case 2: { // Accept State for ID
            state = 0;
            token = createToken();
            tokenCreated = true;
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
            tokenCreated = true;
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
                forward--;
            }
            break;
        }
        case 6: { // Accept State for NUM
            state = 0;
            token = createToken();
            token->tok = NUM;
            setLexeme();
            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 14: { // Accept State for MINUS
            state = 0;
            token = createToken();
            token->tok = MINUS;
            forward--;
            tokenCreated = true;
            break;
        }
        case 15: {
            if (curr == '*') {
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
            tokenCreated = true;
            break;
        }
        case 17: { // Accept State for DIV
            state = 0;
            token = createToken();
            token->tok = DIV;
            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 20: { // Accept State for LE
            state = 0;
            token = createToken();
            token->tok = LE;

            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 23: { // Accept State for DRIVERDEF
            state = 0;
            token = createToken();
            token->tok = DRIVERDEF;
            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 26: { // Accept State for GE
            state = 0;
            token = createToken();
            token->tok = GE;
            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 29: { // Accept State for DRIVERENDDEF
            state = 0;
            token = createToken();
            token->tok = DRIVERENDDEF;
            forward--;
            tokenCreated = true;
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
            tokenCreated = true;
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
            tokenCreated = true;
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
            tokenCreated = true;
            break;
        }
        case 36: { // Accept State for COLON
            state = 0;
            token = createToken();
            token->tok = COLON;
            forward--;
            tokenCreated = true;
            break;
        }
        case 37: { // Accept State for SEMICOL
            state = 0;
            token = createToken();
            token->tok = SEMICOL;
            forward--;
            tokenCreated = true;
            break;
        }
        case 38: { // Accept State for COMMA
            state = 0;
            token = createToken();
            token->tok = COMMA;
            forward--;
            tokenCreated = true;
            break;
        }
        case 39: { // Accept State for SQBO
            state = 0;
            token = createToken();
            token->tok = SQBO;
            forward--;
            tokenCreated = true;
            break;
        }
        case 40: { // Accept State for SQBC
            state = 0;
            token = createToken();
            token->tok = SQBC;
            forward--;
            tokenCreated = true;
            break;
        }
        case 41: { // Accept State for BO
            state = 0;
            token = createToken();
            token->tok = BO;
            forward--;
            tokenCreated = true;
            break;
        }
        case 42: { // Accept State for BC
            state = 0;
            token = createToken();
            token->tok = BC;
            forward--;
            tokenCreated = true;
            break;
        }
        case 43: {
            if (curr == '.') {
                state = 44;
            } else {
                state = 100;
                forward -= 2;
                errno = 6; // Not . after .
            }
            break;
        }
        case 44: { // Accept State for RANGEOP
            state = 0;
            token = createToken();
            token->tok = RANGEOP;
            forward--;
            tokenCreated = true;
            break;
        }
        case 45: { // Comment starts here
            state = 46;
            // Token for COMMENT shouldn't be generated
            // token = createToken();
            // token->tok = COMMENTMARK;
            forward--;
            // tokenCreated = true;
            break;
        }
        case 46: {
            if (curr == '*') {
                state = 47;
            } else if (curr == '\n') {
                LINE_NUM++;
            } else if (curr == EOF) {
                state = 100;
                forward--;
                errno = 7;
            } else {
                state = 46;
            }
            break;
        }
        case 47: {
            if (curr == '*') {
                state = 48;
            } else if (curr == '\n') {
                LINE_NUM++;
            } else if (curr == EOF) {
                state = 100;
                forward--;
                errno = 7;
            } else {
                state = 46;
            }
            break;
        }
        case 48: { // Comment ends here
            state = 0;
            begin = forward - 2;
            // Token for COMMENT shouldn't be generated
            // token = createToken();
            // token->tok = COMMENTMARK;
            begin = forward;
            forward--;
            // tokenCreated = true;
            break;
        }
        case 100: {
            char invalidLex[forward - begin];
            int i = 0;
            // forward was decremented once more than required to exclude the char after error
            while (begin < (forward + 1)) {
                invalidLex[i++] = buf[begin % (2 * BUF_SIZE)];
                begin++;
            }
            begin--; // Begin should begin just after the error char
            invalidLex[i] = 0;

            // Prevent printing for unlosed comment
            if (errno != 7) {
                printf(RED BOLD "[Lexer] Line: %d Error in the input as \"%s\"; " RESET, LINE_NUM, invalidLex);
            }

            state = 101;
            forward--;

            switch (errno) {
            case 1:
                printf(RED BOLD "Invalid character after '.'\n" RESET);
                state = 0;
                break;
            case 2:
                printf(RED BOLD "Invalid character after start of exponent\n" RESET);
                break;
            case 3:
                printf(RED BOLD "Expected integer after exponent\n" RESET);
                state = 0;
                break;
            case 4:
                printf(RED BOLD "Invalid character after \"=\" \n" RESET);
                state = 0;
                break;
            case 5:
                printf(RED BOLD "Expected = after ! \n" RESET);
                break;
            case 6:
                printf(RED BOLD "Invalid '.'\n" RESET);
                break;
            case 7:
                printf(RED BOLD "[Lexer] Unclosed Comment\n" RESET);
                token = createToken();
                strcpy(token->lexeme, "EOF");
                token->linenum = LINE_NUM;
                token->tok = EOF_SYMBOL;
                state = 0;
                return token;
                break;
            case 8:
                printf(RED BOLD "Invalid character found\n" RESET);
                break;
            case 9:
                printf(RED BOLD "ID size exeeds limit\n" RESET);
                break;
            default:
                printf(RED BOLD "Undetected Syntax Error\n" RESET);
                break;
            }

            break;
        }

        case 101: {
            if (curr == '\n') {
                state = 0;
                LINE_NUM++;
            } else if (inSyncSetA(curr)) {
                state = 0;
                forward--;
            } else if (inSyncSetB(curr)) {
                state = 0;
            } else {
                state = 101;
            }
            begin++;
            break;
        }
        }
        forward++;

        // Loading the buffer when required
        if ((forward % (2 * BUF_SIZE)) == (BUF_SIZE - 1) && !lastBufLoad) {
            bufferLoader(fp, false);
            lastBufLoad = 1;
        } else if ((forward % (2 * BUF_SIZE)) == (2 * BUF_SIZE - 1) && lastBufLoad) {
            bufferLoader(fp, true);
            lastBufLoad = 0;
        }
        if (tokenCreated)
            return token;
    }
}
