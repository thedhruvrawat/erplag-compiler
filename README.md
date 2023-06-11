# ERPLAG Compiler, Group 02

![Language](https://img.shields.io/static/v1?label=Language&message=C&color=informational&style=for-the-badge)

This repository contains the toy-compiler created as part of requirements for the course **CS F363: Compiler Construction** at BITS Pilani, Pilani Campus in Spring 2023 under [Dr. Vandana Agarwal](https://universe.bits-pilani.ac.in/pilani/vandana/profile). Our team (**Group 2**) scored the highest among all groups which were part of the course.

> See marks distribution for our team [here](./docs/Marks.pdf)

## Features supported by the language

> [Click here](./docs/language%20specifications.pdf) for language specifications.

- The language `ERPLAG` is a strongly typed language with primitive data types as **integer** and **floating point**. 
- It also supports two other data types: **boolean** and **arrays**. 
- The language supports arithmetic and boolean expressions. 
- The language supports assignment statements, input/output statements, declarative, conditional, iterative, and function call statements. 
- The language supports the modular implementation of the functionalities. 
- Functions can return multiple values. The function may or may not return a value as well. 
- The scope of the variables is static and the variable is visible only in the block where it is declared.

## Dependencies

1. Ensure you have `gcc`, version 5.0 or greater, installed.
> `gcc` version can be checked via `gcc --version` command on terminal.

2. Ensure you have `nasm`, version 2.14.02 or greater, installed
> `nasm` version can be checked via `nasm --version` command on terminal. You can install `nasm` via `sudo apt install nasm`

## Instructions to run

`makefile` has been provided with the code along with some sample test cases.
Run the following commands.
```bash
$ make
// The output to this should be
gcc -c -g driver.c
gcc -c -g parser.c
gcc -c -g lexer.c
gcc -c -g trie.c
gcc -c -g stackADT.c
gcc -c -g Set.c
gcc -c -g ast.c
gcc -c -g symbolTable.c
gcc -c -g intermedCodeGen.c
gcc -c -g codegen.c
gcc -Wall driver.o parser.o trie.o lexer.o Set.o stackADT.o ast.o symbolTable.o intermedCodeGen.o codegen.o -g -o compiler
```
Once all the files have been compiled correctly, run
```bash
$ ./compiler <testcase.txt> <outputFile.asm>
```

To run the `.asm` file, use this command
```bash
$ nasm -f elf64 <outputFile.asm> -o code.o && gcc -no-pie code.o -o code
$ ./code
```

## Sample `ERPLAG` Code

```
<<<driver program>>>
start
	declare x, y, z:integer;
	declare a, b, c:integer;
	a:= 5;
	b:= 9;
	get_value(x);
	get_value(y);
	z:= x + y*b +(a-b)*y+ a*2 - b*x;	
        print(z);
end
```

## Sample Output
```
Input: Enter an integer value 
2
Input: Enter an integer value 
3
Output: 9
```

## Team Members

| S. No. | Name                                     | ID        |  GitHub Profile |
| ------ | -------------------- | ----------- | -- |
| 1      | Dhruv Rawat          | 2019B3A70537P | [thedhruvrawat](https://github.com/thedhruvrawat) |
| 2      | Chirag Gupta         | 2019B3A70555P | [Chirag5128](https://github.com/Chirag5128) |
| 3      | Swastik Mantry       | 2019B1A71019P | [Swastik-Mantry](https://github.com/Swastik-Mantry) |
| 4      | Shreyas Sheeranali   | 2019B3A70387P | [ShreyasSR](https://github.com/ShreyasSR) |
| 5      | Vaibhav Prabhu       | 2019B3A70593P | [prabhuvaibhav](https://github.com/prabhuvaibhav) |

![Group](./docs/team.png)

> ## Disclaimer
> All the code and files provided on this repository are strictly for educational purposes with no intention of promoting unfair means in any evaluative component.

## License 
MIT