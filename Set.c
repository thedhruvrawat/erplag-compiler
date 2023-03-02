/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include <stdlib.h>
#include <string.h>
#include "Set.h"

/**
 * @brief Allocates memory for a new set from the heap and initializes it
 *
 * @param sz
 * @return Set*
 */
Set* initSet(int sz)
{
    Set* s = malloc(sizeof(Set));
    s->sz = sz;
    s->contains = malloc(sz * sizeof(bool));
    memset(s->contains, false, sz * sizeof(bool));

    return s;
}

/**
 * @brief Computes the union of the two input sets and stores it in the first set
 *
 * @param a
 * @param b
 * @return true
 * @return false
 */
bool unionSet(Set* a, Set* b)
{
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

/**
 * @brief Computes the intersection of the two input sets and stores it in the first set
 *
 * @param a
 * @param b
 */
void intersectionSet(Set* a, Set* b)
{
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        a->contains[i] = a->contains[i] && b->contains[i];
    }

    return;
}

/**
 * @brief Computes the difference of the two input sets and stores it in the first set
 *
 * @param a
 * @param b
 */
void differenceSet(Set* a, Set* b)
{
    int sz = a->sz;
    for (int i = 0; i < sz; ++i) {
        if (b->contains[i]) {
            a->contains[i] = false;
        }
    }

    return;
}

/**
 * @brief Frees the memory allocated for the set
 *
 * @param s
 */
void destroySet(Set* s)
{
    free(s->contains);
    free(s);
    return;
}