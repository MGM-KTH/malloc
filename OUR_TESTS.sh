gcc -c -w -O4 -DSTRATEGY=1 malloc.c
gcc -c -w -O4 malloc_test.c
gcc -o malloc_test *.o
echo "RUNNING FIRST FIT..."
./malloc_test < test_data.in 2> first_fit.dat

gcc -c -w -O4 -DSTRATEGY=2 malloc.c
gcc -c -w -O4 malloc_test.c
gcc -o malloc_test *.o
echo "RUNNING BEST FIT..."
./malloc_test < test_data.in 2> best_fit.dat

./gnu_plot.sh
if [ "$(uname)" == "Darwin" ]; then
    open plot.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    xdg-open plot.png
fi
