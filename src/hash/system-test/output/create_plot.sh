#!/bin/bash

cat <<- END
set terminal png size 1024, 768
set output "os64_bits$1.png"
set key left top
set xlabel "log size [b]"
set ylabel "Time [ms]"
set xtic 1

plot \
	"os64_bits$1.dat" using 1:2 title 'ID' with linespoints, \
	"os64_bits$1.dat" using 1:3 title 'TR1' with linespoints, \
	"os64_bits$1.dat" using 1:4 title 'CWLF' with linespoints, \
	"os64_bits$1.dat" using 1:5 title 'BitString' with linespoints, \
	"os64_bits$1.dat" using 1:6 title 'Tabulation' with linespoints, \
	"os64_bits$1.dat" using 1:7 title 'LinearMap' with linespoints, \
	"os64_bits$1.dat" using 1:8 title 'Pol2' with linespoints, \
	"os64_bits$1.dat" using 1:9 title 'Pol4' with linespoints, \
	"os64_bits$1.dat" using 1:10 title 'DW4' with linespoints
END

