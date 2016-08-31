sort -k1,1n -k2,2n /tmp/data.txt -o /tmp/data.sorted

oldkey=
while read key value
do
	if [[ $key = $oldkey ]]
	then
		continue
	fi

	echo $key $value
	oldkey=$key
done < /tmp/data.sorted > data.reduced


