/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
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
    "TRUE", "FALSE", "AND", "OR",
    "EPS", "EOF_SYMBOL"
};