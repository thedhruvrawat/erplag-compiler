#ifndef LEXER_DEF
#define LEXER_DEF
typedef enum { 
    // Keywords
    INTEGER, REAL, BOOLEAN, OF, ARRAY, START,
    END, DECLARE, MODULE, DRIVER, PROGRAM, GET_VALUE,
    PRINT, USE, WITH, PARAMETERS, TAKES, INPUT,
    RETURNS, FOR, IN, SWITCH, CASE, BREAK,
    DEFAULT, WHILE,
    // Non-Keywords
    ID, NUM, RNUM, 
    PLUS, MINUS, MUL, DIV, 
    LT, LE, GE, GT, EQ, NE, 
    DEF, ENDDEF, DRIVERDEF, DRIVERENDDEF, 
    COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, 
    SQBO, SQBC, BO, BC, COMMENTMARK, 
    // Boolean Operators
    TRUE, FALSE, AND, OR,
    EPS, EOF_SYMBOL
} TOK_TYPE;

extern const char* token_types[];

typedef struct TOKEN {
    TOK_TYPE tok;
    union{
        int num;
        double rnum;
    };
    //Lexeme outside union as requirement to print lexeme for evaluation
    char lexeme[21]; 
    int linenum;
} TOKEN;

#endif