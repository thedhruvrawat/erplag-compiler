#!/bin/bash
make clean
make sanitized
./stage1exe $1 $2

