#!/bin/sh

latexdiff lab070217-siam-submitted.tex lab-fix.tex > diff.tex
pdflatex diff.tex
pdflatex lab-fix.tex
