#!/bin/sh

get_value() {
	while [ $# -gt 0 ]
	do
		header=$1

		val=`grep -i "$header.*:" $i/mail | cut -d: -f2-`
		if [ -n "$val" ]
		then
			echo "$val" | sed 's/^ //'
			return
		fi
		shift
	done

	echo ---
}

get_description() {
	sed -n "/Popis: /,/.*: /p" "$i/mail" | head -n-1 | sed 's/^Popis: //' | paste -s -d ' '
	sed -n "/Description[^:]*: /,/.*: /p" "$i/mail" | head -n-1 | sed 's/^Description[^:]*: //' | paste -s -d ' '
}

for i in [0-9]*
do 
	id=$i
	pseudo=`get_value pseudonym | sed 's/\.$//'`
	name=`get_value jmeno name jméno | sed 's/\.$//'`
	practical=`get_value cviceni tutorial cvičení | sed 's/\.$//'`
	desc=`get_description description`
	time=`get_value time cas čas Čas | sed 's/.*real[^0-9]*\([0-9ms.]*\).*/\1/'`

	echo "$id","$pseudo","$name","$practical","$time","\"$desc\""
done > db.txt
