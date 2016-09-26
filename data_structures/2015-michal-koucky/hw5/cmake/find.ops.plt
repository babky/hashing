set yrange[0:15]
set datafile separator ','
plot \
	'flt.csv' using 1:2 with line title 'linear tab',\
	'flm.csv' using 1:2 with line title 'linear mult',\
	'ftt.csv' using 1:2 with line title 'two tab',\
	'ftm.csv' using 1:2 with line title 'two mult',\
	'fcm.csv' using 1:2 with line title 'cuckoo mult',\
	'fct.csv' using 1:2 with line title 'cuckoo tab'
