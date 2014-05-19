make clean > /dev/null 2>&1
echo "RUNNING TESTS ON SMALL DATA"
gcc -c -w -DSTRATEGY=2 malloc.c
gcc -c -w malloc_test_small.c
gcc -o malloc_test *.o
echo "RUNNING BEST FIT..."
./malloc_test < data/test_data_small.in 2> data/best_fit_small.dat

gcc -c -w -DSTRATEGY=1 malloc.c
gcc -c -w malloc_test_small.c
gcc -o malloc_test *.o
echo "RUNNING FIRST FIT..."
./malloc_test < data/test_data_small.in 2> data/first_fit_small.dat

gcc -c -w -DSTRATEGY=0 malloc.c
gcc -c -w malloc_test_small.c
gcc -o malloc_test *.o
echo "RUNNING System malloc..."
./malloc_test < data/test_data_small.in 2> data/system_malloc_small.dat

echo "RUNNING TESTS ON BIGGER DATA"
make clean > /dev/null 2>&1
gcc -c -w -DSTRATEGY=2 malloc.c
gcc -c -w malloc_test_big.c
gcc -o malloc_test *.o
echo "RUNNING BEST FIT..."
./malloc_test < data/test_data_big.in 2> data/best_fit_big.dat

gcc -c -w -DSTRATEGY=1 malloc.c
gcc -c -w malloc_test_big.c
gcc -o malloc_test *.o
echo "RUNNING FIRST FIT..."
./malloc_test < data/test_data_big.in 2> data/first_fit_big.dat

gcc -c -w -DSTRATEGY=0 malloc.c
gcc -c -w malloc_test_big.c
gcc -o malloc_test *.o
echo "RUNNING System malloc..."
./malloc_test < data/test_data_big.in 2> data/system_malloc_big.dat


./gnu_plot.sh
cd data
if [ "$(uname)" == "Darwin" ]; then
    open *.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    find -iname '*.png' -print0 | xargs -0 -n 1 xdg-open
fi
cd ..