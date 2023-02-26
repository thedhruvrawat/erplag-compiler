#include<stdbool.h>

typedef enum { 
    // Non-Keywords
    ID, NUM, RNUM, 
    PLUS, MINUS, MUL, DIV, 
    LT, LE, GE, GT, EQ, NE, 
    DEF, ENDDEF, DRIVERDEF, DRIVERENDDEF, 
    COLON, RANGEOP, SEMICOL, COMMA, ASSIGNOP, 
    SQBO, SQBC, BO, BC, COMMENTMARK, 
    // Boolean Operators
    TRUE, FALSE, AND, OR,
    // Keywords
    INTEGER, REAL, BOOLEAN, OF, ARRAY, START,
    END, DECLARE, MODULE, DRIVER, PROGRAM, GET_VALUE,
    PRINT, USE, WITH, PARAMETERS, TAKES, INPUT,
    RETURNS, FOR, IN, SWITCH, CASE, BREAK,
    DEFAULT, WHILE
} TOK_TYPE;

extern const char* token_types[];

typedef struct TOKEN {
    TOK_TYPE tok;
    char lexeme[20];
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
    bool isEpsilon;
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