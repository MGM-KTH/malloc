gcc -c -w -O4 -DSTRATEGY=2 malloc.c
gcc -c -w -O4 -pg malloc_test_small.c
gcc -o malloc_test *.o
echo "RUNNING BEST FIT..."
./malloc_test < test_data.in 2> best_fit.dat
#/usr/bin/time ./malloc_test < test_data.in

gcc -c -w -O4 -DSTRATEGY=1 malloc.c
gcc -c -w -O4 -pg malloc_test_small.c
gcc -o malloc_test *.o
echo "RUNNING FIRST FIT..."
./malloc_test < test_data.in 2> first_fit.dat
#/usr/bin/time ./malloc_test < test_data.in

./gnu_plot.sh
if [ "$(uname)" == "Darwin" ]; then
    open *.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    find -iname '*.png' -print0 | xargs -0 -n 1 xdg-open
fi
