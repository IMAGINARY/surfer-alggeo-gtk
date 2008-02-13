#!/bin/sh

echo first $1
echo second $2

HERE="/home/hmeyer/printing"
IMAGE_FILE=$HERE/image.png
FORMULA_FILE=$HERE/formula.tex
LATEX_FILE=$HERE/surfer_print

FORMULA_EPS_TEX=$HERE/surfer_formula


cp $1 $IMAGE_FILE
cat $2 | $HERE/texify.pl > $FORMULA_FILE
#cat $PREMATH $FORMULA_FILE.pre $POSTMATH > $FORMULA_FILE

cd $HERE
convert image.png image.eps

latex -interactive=nonstopmode $FORMULA_EPS_TEX.tex
dvips -E $FORMULA_EPS_TEX.dvi
eps2eps $FORMULA_EPS_TEX.ps eps_formula.eps

latex -interaction=nonstopmode $LATEX_FILE.tex
dvips $LATEX_FILE.dvi
lpr   $LATEX_FILE.ps

