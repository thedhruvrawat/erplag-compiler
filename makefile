#Group Number : 2
#1 	Dhruv Rawat 		2019B3A70537P 	thedhruvrawat
#2 	Chirag Gupta 		2019B3A70555P 	Chirag5128
#3 	Swastik Mantry 		2019B1A71019P 	Swastik-Mantry
#4 	Shreyas Sheeranali 	2019B3A70387P 	ShreyasSR
#5 	Vaibhav Prabhu 		2019B3A70593P 	prabhuvaibhav
driver: driver.o parser.o lexer.o trie.o stackADT.o Set.o ast.o symbolTable.o intermedCodeGen.o codegen.o
	gcc -Wall driver.o parser.o trie.o lexer.o Set.o stackADT.o ast.o symbolTable.o intermedCodeGen.o codegen.o -g -o compiler

sanitized: driver.o parser.o lexer.o trie.o stackADT.o Set.o ast.o symbolTable.o intermedCodeGen.o codegen.o
	gcc -fsanitize=address -Wall driver.o parser.o trie.o lexer.o Set.o stackADT.o symbolTable.o intermedCodeGen.o codegen.o ast.o -g -o compiler

intermedCodeGen.o: intermedCodeGen.c intermedCodeGen.h intermedCodeGenDef.h
	gcc -c -g intermedCodeGen.c

symbolTable.o: symbolTable.c symbolTable.h symbolTableDef.h
	gcc -c -g symbolTable.c

stackADT.o: stackADT.c
	gcc -c -g stackADT.c

codegen.o: codegen.c 
	gcc -c -g codegen.c

parser.o: parser.c parser.h parserDef.h
	gcc -c -g parser.c

ast.o: ast.c ast.h astDef.h
	gcc -c -g ast.c

driver.o: driver.c
	gcc -c -g driver.c

lexer.o: lexer.c lexer.h lexerDef.h
	gcc -c -g lexer.c

trie.o: trie.c trie.h
	gcc -c -g trie.c

Set.o: Set.c Set.h
	gcc -c -g Set.c

clean:
	rm -f *.o lexer parser a.out stage1exe vgcore*
