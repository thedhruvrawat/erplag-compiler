/*
Group Number : 2
1 	Dhruv Rawat 	    2019B3A70537P 	thedhruvrawat
2 	Chirag Gupta 	    2019B3A70555P 	Chirag5128
3 	Swastik Mantry 	    2019B1A71019P 	Swastik-Mantry
4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
5 	Vaibhav Prabhu 	    2019B3A70593P 	prabhuvaibhav
*/
typedef struct loopStNode{
    char* label;
    struct loopStNode * next;
} loopStNode;

typedef struct loopSt{
    loopStNode * top;
    int size;
    char type; // 'S' for switch, 'F' for for loop, 'W' for while, 'N' for None
} loopSt;