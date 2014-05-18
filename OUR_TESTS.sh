gcc -c malloc.c
gcc -c malloc_test.c
gcc -o malloc_test *.o
./malloc_test < test_data.in