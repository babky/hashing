#!/bin/bash

./process.sh os64_bits$1.out > os64_bits$1.dat
./create_plot.sh $1 > os64_bits$1.plt
gnuplot < os64_bits$1.plt

rm -f os64_bits$1.dat os64_bits$1.plt

