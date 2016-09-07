set yrange[0:10]
set datafile separator ','
plot \
	'ilt.csv' using 1:2 with line title 'linear tab',\
	'ilm.csv' using 1:2 with line title 'linear mult',\
	'itt.csv' using 1:2 with line title 'two tab',\
	'itm.csv' using 1:2 with line title 'two mult',\
	'icm.csv' using 1:2 with line title 'cuckoo mult',\
	'ict.csv' using 1:2 with line title 'cuckoo tab'
