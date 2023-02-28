# grammarNew: grammar.o trie.o stackADT.o lexer.o
# 	gcc grammar.o trie.o stackADT.o lexer.o -g -o grammarNew


driver: driver.o grammar.o lexer.o trie.o stackADT.o structs.o
	gcc driver.o grammar.o trie.o lexer.o structs.o stackADT.o -g -o stage1exe

grammar: grammar.o lexer.o trie.o stackADT.o
	gcc grammar.o trie.o lexer.o stackADT.o -g -o grammar

sanitized: grammar.o lexer.o trie.o stackADT.o
	gcc -fsanitize=address grammar.o trie.o lexer.o stackADT.o -g -o grammar

# set.o: set.c
# 	gcc -c -g set.c

# parseTable.o: parseTable.c
# 	gcc -c -g parseTable.c

stackADT.o: stackADT.c
	gcc -c -g stackADT.c

grammar.o: grammar.c grammar.h
	gcc -c -g grammar.c

lexer: lexer.o trie.o structs.o
	gcc -g lexer.o trie.o structs.o -o lexer 

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
