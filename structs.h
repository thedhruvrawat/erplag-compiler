/*
Group Number : 2
1 	Dhruv Rawat 	2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	2019B3A70593P 	prabhuvaibhav
*/
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// ANSI Colors used 
#define RED   		"\033[0;31m"
#define GREEN 		"\033[0;32m"
#define YELLOW 		"\033[0;33m"
#define BLUE 		"\033[0;34m"
#define PURPLE 		"\033[0;35m"
#define CYAN 		"\033[0;36m"
#define INVERT		"\033[0;7m"
#define RESET  		"\e[0m" 
#define BOLD		"\e[1m"
#define ITALICS		"\e[3m"
#define UNDERLINE	"\e[4m"

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

typedef struct Trie {
    int count;
    bool end;
    TOK_TYPE tok;
    struct Trie* next[128];
} Trie;

typedef struct Tuple {
    int enumID;
    char *token;
} Tuple;

typedef struct listElement {
    int productionID;
    struct listElement* next;
} listElement;

typedef struct Set {
    int sz;
    bool* contains;
} Set;


typedef struct grammarElement {
    bool isTerminal;
    int tokenID;
    char lexeme[100];
    struct grammarElement *next, *prev;
} grammarElement;

typedef struct ProductionRule {
    int productionID;
    int RHScount;
    grammarElement *LHS; // pointer to LHS of a rule
    grammarElement *RHSHead, *RHSTail; // pointer to first element in RHS
    Set *firstSet, *followSet;
} ProductionRule;

typedef struct ProductionTable {
    int maxRules, ruleCount;
    ProductionRule **grammarrules; 
} ProductionTable;

typedef struct TreeNode {
    TOKEN* tok;
    int depth;
    int tokenID;
    int productionID;
    int tokenDerivedFrom;
    bool isLeaf;
    struct TreeNode* next;
    struct TreeNode* child;
} TreeNode;

typedef struct ParseTree {
    TreeNode* root;
    int sz;
    int treeDepth;
} ParseTree;