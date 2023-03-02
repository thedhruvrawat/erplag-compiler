#ifndef SET
#define SET

#include <stdbool.h>

typedef struct Set {
    int sz;
    bool* contains;
} Set;

Set* initSet(int sz);

bool unionSet(Set* a, Set* b);

void intersectionSet(Set* a, Set* b);

void differenceSet(Set* a, Set* b);

void destroySet(Set* s);

#endif