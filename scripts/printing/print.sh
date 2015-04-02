#!/bin/sh

script_dir=$(dirname $0)

echo
echo Printing ...
echo script_dir $script_dir
echo png file $1
echo formula $2
echo surfpic file $3

# get generated surf image
cp $1 $script_dir/image.png
cat $2 | "$script_dir/texify.pl" > "$script_dir/formula.tex"

cd $script_dir

pdflatex surfer_print.tex
lpr surfer_print.pdf

# move surf input and script output to archive
mkdir -p archive
timestamp=`date +%Y-%m-%d_%H-%M-%S`
#next two lines must be fixed: Surfer should send path to .pic-file as script argument
#cp /tmp/usersurfb_f_p.pic $script_dir/archive/$timestamp.pic
#cp $3 $script_dir/archive/$timestamp.pic

mv surfer_print.pdf archive/$timestamp.pdf
mv image.png archive/$timestamp.png
echo done ...
