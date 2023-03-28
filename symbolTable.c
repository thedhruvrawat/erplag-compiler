/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
#include "symbolTableDef.h"

// Fowler–Noll–Vo hash function
unsigned int hash(const char *str) {
    unsigned int res = 0;
    const unsigned int prime = 16777619;

    for (int i = 0; i < 20 && str[i] != '\0'; i++) {
        res = (res * prime) ^ str[i];
    }

    return (res % HASH_TABLE_SIZE);
}

