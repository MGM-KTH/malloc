SRC = malloc.h malloc.c malloc_test_small.c malloc_test_big.c

OBJ = malloc.o malloc_test_small.o malloc_test_big.o

BIN = bestfit_small bestfit_big firstfit_small firstfit_big \
 	  system_malloc_small system_malloc_big \
 	  worstfit_small worstfit_big

CC = gcc
CFLAGS	=  -g -w -ansi

all: $(BIN)

malloc: malloc.o
	$(CC) $(CFLAGS) -o $@ malloc.o

malloc.o:
	$(CC) -g -w -ansi -DLOCAL=1 -DSTRATEGY=2 -c malloc.c

worstfit_small: malloc_test_small.o
	$(CC) $(CFLAGS) -DSTRATEGY=3 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o

worstfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=3 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

bestfit_small: malloc_test_small.o
	$(CC) $(CFLAGS) -DSTRATEGY=2 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o

bestfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=2 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o	

firstfit_small: malloc_test_small.o
	$(CC) $(CFLAGS) -DSTRATEGY=1 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o

firstfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=1 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

system_malloc_small: malloc_test_small.o
	$(CC) $(CFLAGS) -DSTRATEGY=0 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o

system_malloc_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=0 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

malloc_test_small.o:
	$(CC) $(CFLAGS) -c malloc_test_small.c 

malloc_test_big.o:
	$(CC) $(CFLAGS) -c malloc_test_big.c 

clean: 
	rm -f *.o $(BIN)
