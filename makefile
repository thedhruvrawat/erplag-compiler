default: lexer.o trie.o
	gcc lexer.o trie.o -o lexer

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

trie.o: trie.c trie.h
	gcc -c trie.c

clean:
	rm -f *.o lexer
