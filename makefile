
grammar: grammar.o trie.o
	gcc grammar.o trie.o -o grammar

grammar.o: grammar.c grammar.h
	gcc -c grammar.c

lexer: driver.o lexer.o trie.o structs.o
	gcc driver.o lexer.o trie.o structs.o -o lexer

driver.o: driver.c
	gcc -c driver.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

trie.o: trie.c trie.h
	gcc -c trie.c

structs.o: structs.c structs.h
	gcc -c structs.c

clean:
	rm -f *.o lexer grammar a.out
