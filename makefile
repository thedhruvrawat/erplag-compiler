grammarNew: grammar.o trie.o stackADT.o lexer.o
	gcc grammar.o trie.o stackADT.o lexer.o -g -o grammarNew

grammar: grammar.o lexer.o trie.o
	gcc grammar.o trie.o lexer.o -g -o grammar

# set.o: set.c
# 	gcc -c -g set.c

# parseTable.o: parseTable.c
# 	gcc -c -g parseTable.c

stackADT.o: stackADT.c
	gcc -c -g stackADT.c

grammar.o: grammar.c grammar.h
	gcc -c -g grammar.c

lexer: driver.o lexer.o trie.o structs.o
	gcc -g driver.o lexer.o trie.o structs.o -o lexer 

driver.o: driver.c
	gcc -c -g driver.c

lexer.o: lexer.c lexer.h
	gcc -c -g lexer.c

trie.o: trie.c trie.h
	gcc -c -g trie.c

structs.o: structs.c structs.h
	gcc -c -g structs.c

clean:
	rm -f *.o lexer grammar grammarNew a.out
