typedef enum { 
    ID, NUM, RNUM, 
    PLUS, MINUS, MUL, DIV, 
    LT, LE, GE, GT, EQ, NE, 
    DEF, ENDDEF, DRIVERDEF, DRIVERENDDEF, 
    COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, 
    SQBO, SQBC, BO, BC, 
    COMMENTMARK 
} TOK_TYPE ;

const char *token_types[] =  {
    "ID", "NUM", "RNUM", 
    "PLUS", "MINUS", "MUL", "DIV", 
    "LT", "LE", "GE", "GT", "EQ", "NE", 
    "DEF", "ENDDEF", "DRIVERDEF", "DRIVERENDDEF", 
    "COLON", "RANGEOP", "SEMICOL", "COMMA", "ASSIGNOP", 
    "SQBO", "SQBC", "BO", "BC", 
    "COMMENTMARK"
};

typedef struct TOKEN {
    TOK_TYPE tok;
    char lexeme[20];
    int linenum;
} TOKEN;