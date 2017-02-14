#!/bin/sh

file=$1

name=`echo "$file" | sed 's/[.].*//'`

gnuplot <<-END
	set terminal png size 2048,512
	set datafile separator ','
	set output '$name.png'
	set logscale y 32
	plot	\
		'$file' using 1:5 with lines title 'mult-shift',\
		'$file' using 1:7 with lines title 'lin-trans'
END

#		'$file' using 1:3 with lines title 'modular',\
