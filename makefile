driver: driver.o grammar.o lexer.o trie.o stackADT.o structs.o
	gcc driver.o grammar.o trie.o lexer.o structs.o stackADT.o -g -o stage1exe

sanitized: driver.o grammar.o lexer.o trie.o stackADT.o structs.o
	gcc -fsanitize=address driver.o grammar.o trie.o lexer.o structs.o stackADT.o -g -o stage1exe

stackADT.o: stackADT.c
	gcc -c -g stackADT.c

grammar.o: grammar.c grammar.h
	gcc -c -g grammar.c

driver.o: driver.c
	gcc -c -g driver.c

lexer.o: lexer.c lexer.h
	gcc -c -g lexer.c

trie.o: trie.c trie.h
	gcc -c -g trie.c

structs.o: structs.c structs.h
	gcc -c -g structs.c

clean:
	rm -f *.o lexer grammar a.out stage1exe vgcore*
