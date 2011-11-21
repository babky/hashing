#!/bin/bash

IN="$1"

awk '
BEGIN {
    print "# Length System_1 ... System_n"
}

/^TEST: length/ {
	bits = int(log(substr($4, 0, length($4) - 1)) / log(2))
	systems = ""
}

/ ms$/ {
	systems = systems " " $(NF - 3)
}

/^$/ {
	if (bits > 0) {
		print bits systems
	}
	bits = 0
	systems = ""
}

/---PRINTING WHOLE DATA SET---/ {
	nextfile
}

' < "$IN"


