# comment out prints in .c before running this one
gcc -c malloc_test_small.c
gcc -o malloc_test *.o
for i in {1,2,4,8,16};
do
    echo $i | ./malloc_test
done    