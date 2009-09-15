#!/bin/sh

echo first $1
echo second $2

# enter directory path to this shell script
HERE="/home/mima/Desktop/IMAGINARY/surfer_printing"
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
dvips -t a5 $LATEX_FILE.dvi
lpr   $LATEX_FILE.ps
ps2pdf $LATEX_FILE.ps

# create archive copy
mkdir -p archive
timestamp=`date +%Y-%m-%d_%H-%M-%S`
cp $LATEX_FILE.pdf archive/$timestamp.pdf
cp $1 archive/$timestamp.png
cp /tmp/usersurfb_f_p.pic archive/$timestamp.pic

