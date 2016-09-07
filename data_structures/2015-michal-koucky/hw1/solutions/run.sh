#!/bin/sh

while read i
do 
	cd $i
	echo $i
	sh compile.sh
	ln ../data.txt
	time ./sorter-$i >output 2>time
	sha1sum data.out > sha1
	if grep a168e9b62d17b7b6a3d2ac06802f53f81a1aa4df sha1
	then
		rm -f data.out
		echo $i:`cat time | grep elapsed | cut -d' ' -f3`
	else
		echo $i:BAD_CHECKSUM
	fi
	rm -f *.txt
	cd ..
done < in.txt
