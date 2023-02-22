#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Set {
    int sz;
    bool* contains;
} Set;

Set* initSet(int sz);
void unionSet(Set* a, Set* b);
