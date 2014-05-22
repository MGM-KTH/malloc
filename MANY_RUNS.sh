#!/bin/bash
# comment out prints in .c before running this one

make clean
make

# Set the time format and save the old
OLD_FORMAT=$TIMEFORMAT
TIMEFORMAT="%U"

rm data/*.dat
echo "running small tests"
for program in *_small; do
    for i in {1,2,4,8,16}; do
        echo `( time ( echo $i | ./$program ) 2>&1 )` | awk '{ print $1, $2, substr($3,4) }' >> data/"$program.dat"
    done
done

echo "running big tests"
for program in *_big; do
    for i in {2,4,6,8,10,12,14,16}; do
        echo `( time ( echo $i | ./$program ) 2>&1 )` | awk '{ print $1, $2, substr($3,4) }' >> data/"$program.dat"
    done    
done

echo "screwing with first fit"
for program in screw_first screw_best screw_worst; do
    for i in {2,4,6,8,10}; do
        echo `( time ( echo $i | ./$program ) 2>&1 )` | awk '{ print $1, $2, substr($3,4) }' >> data/"$program.dat"
    done
done

echo "running random tests"
for program in *_rand; do
    for i in {2,4,6,8,10}; do
        echo `( time ( echo $i | ./$program ) 2>&1 )` | awk '{ print $1, $2, substr($3,4) }' >> data/"$program.dat"
    done    
done

make clean

./gnu_plot.sh
cd data
if [ "$(uname)" == "Darwin" ]; then
    open *.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    find -iname '*.png' -print0 | xargs -0 -n 1 xdg-open
fi
cd ..

TIMEFORMAT=$OLD_FORMAT
