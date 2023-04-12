#!/bin/bash
make clean
make sanitized
./compiler $1 $2

