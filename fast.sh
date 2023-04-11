#!/bin/bash
make clean
make sanitized
./stage1exe $1 parseTable.txt 21

