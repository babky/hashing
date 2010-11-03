#!/bin/sh

cd /home/babky/Documents/DP

i=0
for file in *.tex
do
  let i+=1
  echo $i")" $file
done

read line

if [[ "$line" == 0 ]]
then
  exit 0
fi

i=0
for file in *.tex
do
  let i+=1
  if [[ "$line" == $i ]] 
  then
	aspell -l sk check "$file"
  fi
done
