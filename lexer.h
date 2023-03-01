#include <stdio.h>
#include "trie.h"

int setupLexer(FILE* fp);
TOKEN* getNextToken();
void cleanLexer();
void initbuf(void);
void setBufferSize(long long newBufSize);