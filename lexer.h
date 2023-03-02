/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#ifndef LEXER
#define LEXER

#include "lexerDef.h"

int setupLexer(FILE* fp);
TOKEN* getNextToken();
void cleanLexer();
void initbuf(void);
void setBufferSize(long long newBufSize);

#endif