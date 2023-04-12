#!/bin/bash
make clean
make sanitized
./stage1exe $1 $2  <<< $(echo -e "4\n0")
nasm -f elf64 code.asm -o test.o && gcc -no-pie test.o -o testing
./testing
