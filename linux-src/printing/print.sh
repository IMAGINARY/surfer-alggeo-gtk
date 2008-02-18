#!/bin/sh

# $1 ist die Datei mit dem Bild
# $2 ist die Datei mit der Formel
# $3 ist die Datei mit dem Namen

HERE="/home/hmeyer/IMAGINARY/surfer/trunk/linux-src/printing"
IMAGE_FILE=$HERE/image.png
FORMULA_FILE=$HERE/formula.tex
LATEX_FILE_N_NAME=$HERE/surfer_print
LATEX_FILE_W_NAME=$HERE/surfer_with_name
NAME_FILE=$HERE/name.tex

FORMULA_EPS_TEX=$HERE/surfer_formula

echo $1
echo $2
echo $3

echo printing

cp $1 $IMAGE_FILE
cat $2 | $HERE/texify.pl > $FORMULA_FILE
if test -e $3; then cp $3 $NAME_FILE; fi

cd $HERE
convert image.png image.eps

latex -interaction=nonstopmode $FORMULA_EPS_TEX.tex
dvips -E $FORMULA_EPS_TEX.dvi
eps2eps $FORMULA_EPS_TEX.ps eps_formula.eps

if test -e $3 ;  then
LATEX_FILE=$LATEX_FILE_W_NAME
else
LATEX_FILE=$LATEX_FILE_N_NAME
fi

latex -interaction=nonstopmode $LATEX_FILE.tex
dvips $LATEX_FILE.dvi
lpr   $LATEX_FILE.ps;

