#include <stdbool.h>

void computeFirstSet(int nonTerminalLen, int terminalLen);
bool findFirst(int tokenID);
void computeFollowSet(int nonTerminalLen, int terminalLen);
bool findFollow(int tokenID);

