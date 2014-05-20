# comment out prints in .c before running this one

make
rm data/*.dat
echo "running small tests"
for program in *_small; do
    for i in {1,2,4,8,16}; do
        echo $i | ./$program >> data/"$program.dat"
    done
done

echo "running big tests"
for program in *_big; do
    for i in {1,2,4,8,16}; do
        echo $i | ./$program >> data/"$program.dat"
    done    
done

./gnu_plot.sh
cd data
if [ "$(uname)" == "Darwin" ]; then
    open *.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    find -iname '*.png' -print0 | xargs -0 -n 1 xdg-open
fi
cd ..