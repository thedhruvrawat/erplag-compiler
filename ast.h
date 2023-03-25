/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include "astDef.h"

ASTNodeList* initASTNodeList(void);
AST* initAST(void);
ASTNode* createASTNode(const char* label, ParseTreeNode* node);
void insertAtEnd(ASTNodeList* list, ASTNode* node);
void insertAtStart(ASTNodeList* list, ASTNode* node);
void appendASTNodeAsChild(ASTNode* parent, ASTNode* node);
ASTStackNode* createASTStackNode(ParseTreeNode* parseTreeNode, ASTNode* par);
ASTStack* initASTStack(void);
ASTStackNode* peekASTStack(ASTStack* st);
void popASTStack(ASTStack* st);
void pushASTStack(ASTStack* st, ParseTreeNode* parseTreeNode, ASTNode* par);
void pushChildrenToASTStack(ASTStack* st, ASTNode* par, ParseTreeNode* curr);
bool isUsefulTerminal(ASTStackNode* node);
void createAST(void);
void printAST(ASTNode* node, bool firstChild);
void ASTCreator(ParseTree* parseTree);

