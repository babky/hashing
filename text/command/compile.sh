#!/bin/sh

cd /home/babky/Documents/DP

if [ $# -ge 1 ]
then
	rm -rf output
	mkdir output
	cd output
	TEXINPUTS=".:..:$TEXINPUTS" pdflatex -halt-on-error ../text.tex > /dev/null
	TEXINPUTS=".:..:$TEXINPUTS" pdflatex -halt-on-error ../text.tex > /dev/null
else
	mkdir output 2> /dev/null
	cd output
fi

TEXINPUTS=".:..:$TEXINPUTS" pdflatex -halt-on-error ../text.tex
echo

echo --------------------------------------------------------------------
echo Overfull hboxes:
if ! grep Overfull text.log 
then
  echo none
else
  cat ../command/beep
fi

echo
echo Underfull hboxes:
if ! grep Underfull text.log 
then
  echo none
else
  cat ../command/beep
fi

echo 
echo Warnings:
if ! grep Warning text.log | grep -vi "shell escape feature"
then
  echo none
else
  cat ../command/beep
fi

cd ..
