#!/bin/sh

u=$1
x=$2
t=${3:-x}

./Surjectivity -u "$u" -s "$x" -t "$t" -i 0 > surjectivity-u:"$u"-t:"$t"-s:"$s".out

