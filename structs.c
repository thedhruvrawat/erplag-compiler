#include "structs.h"

const char *token_types[] =  {
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
    "TRUE", "FALSE", "AND", "OR"

};