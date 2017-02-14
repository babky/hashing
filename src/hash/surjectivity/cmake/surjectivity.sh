#!/bin/sh

u=$1
x=$2

./Surjectivity -u "$u" -s "$x" -t "$x" -i 0 > surjectivity-u:"$u"-t:"$x"-s:"$x".out

