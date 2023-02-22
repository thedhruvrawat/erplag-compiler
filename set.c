#include "set.h"
#include "grammar.h"
#include <stdio.h>

Set* initSet(int sz) {
    Set* s = malloc(sizeof(Set));
    s->sz = sz;
    s->contains = malloc(sz * sizeof(bool));
    memset(s->contains, false, sz * sizeof(bool));

    return s;
}

bool unionSet(Set* a, Set* b) {
    int sz = a->sz;
    // Flag for checking LL(1)
    bool flag = false;
    for (int i = 0; i < sz; ++i) {
        if (a->contains[i] && b->contains[i]) {
            flag = true;
        }
        a->contains[i] = a->contains[i] || b->contains[i];
    }

    return flag;
}

void intersectionSet(Set* a, Set* b) {
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        a->contains[i] = a->contains[i] && b->contains[i];
    }

    return;
}

void differenceSet(Set* a, Set* b) {
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        if (b->contains[i]) {
            a->contains[i] = false;
        }
    }

    return;
}
