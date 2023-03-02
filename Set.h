/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
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