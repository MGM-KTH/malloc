make clean > /dev/null 2>&1
make > /dev/null 2>&1


echo "RUNNING TESTS ON SMALL DATA"

echo "RUNNING WORST FIT..."
./worstfit_small < data/test_data_small.in > data/worst_fit_small.dat

echo "RUNNING BEST FIT..."
./bestfit_small < data/test_data_small.in > data/best_fit_small.dat

echo "RUNNING FIRST FIT..."
./firstfit_small < data/test_data_small.in > data/first_fit_small.dat

echo "RUNNING System malloc..."
./system_malloc_small < data/test_data_small.in > data/system_malloc_small.dat


echo "RUNNING TESTS ON BIGGER DATA"

echo "RUNNING WORST FIT..."
./worstfit_big < data/test_data_big.in > data/worst_fit_big.dat

echo "RUNNING BEST FIT..."
./bestfit_big < data/test_data_big.in > data/best_fit_big.dat

echo "RUNNING FIRST FIT..."
./firstfit_big < data/test_data_big.in > data/first_fit_big.dat

echo "RUNNING System malloc..."
./system_malloc_big < data/test_data_big.in > data/system_malloc_big.dat

make clean > /dev/null 2>&1

./gnu_plot.sh
cd data
if [ "$(uname)" == "Darwin" ]; then
    open *.png
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    find -iname '*.png' -print0 | xargs -0 -n 1 xdg-open
fi
cd ..
