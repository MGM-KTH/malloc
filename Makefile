SRC = malloc.h malloc.c malloc_test_small.c malloc_test_big.c

OBJ = malloc.o malloc_test_small.o malloc_test_big.o

LOCALBIN = bestfit_small bestfit_big firstfit_small firstfit_big \
 	  worstfit_small worstfit_big

SYSBIN = system_malloc_small system_malloc_big

CC = gcc
CFLAGS	=  -g -w -ansi
CFLAGS_SYS = ${CFLAGS} -DSYSTEM

all: local system

# Compile tests using local malloc implementation

local: $(LOCALBIN)

malloc: malloc.o
	$(CC) $(CFLAGS) -o $@ malloc.o

malloc.o:
	$(CC) -g -w -ansi -DLOCAL=1 -DSTRATEGY=2 -c malloc.c

worstfit_small: malloc_test_small.o screw_with_first_fit.o
	$(CC) $(CFLAGS) -DSTRATEGY=3 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o
	$(CC) $(CFLAGS) -o screw_worst screw_with_first_fit.o malloc.o

worstfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=3 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

bestfit_small: malloc_test_small.o screw_with_first_fit.o
	$(CC) $(CFLAGS) -DSTRATEGY=2 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o
	$(CC) $(CFLAGS) -o screw_best screw_with_first_fit.o malloc.o

bestfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=2 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o	

firstfit_small: malloc_test_small.o screw_with_first_fit.o
	$(CC) $(CFLAGS) -DSTRATEGY=1 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o
	$(CC) $(CFLAGS) -o screw_first screw_with_first_fit.o malloc.o

firstfit_big: malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=1 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

malloc_test_small.o:
	$(CC) $(CFLAGS) -c malloc_test_small.c 

malloc_test_big.o:
	$(CC) $(CFLAGS) -c malloc_test_big.c 

screw_with_first_fit.o:
	$(CC) $(CFLAGS) -c screw_with_first_fit.c

# Compile tests using system malloc implementation

system: $(SYSBIN)

system_malloc_small: sys_malloc_test_small.o
	$(CC) $(CFLAGS) -DSTRATEGY=0 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_small.o malloc.o

system_malloc_big: sys_malloc_test_big.o
	$(CC) $(CFLAGS) -DSTRATEGY=0 -c malloc.c
	$(CC) $(CFLAGS) -o $@ malloc_test_big.o malloc.o

sys_malloc_test_small.o:
	$(CC) $(CFLAGS_SYS) -c malloc_test_small.c 

sys_malloc_test_big.o:
	$(CC) $(CFLAGS_SYS) -c malloc_test_big.c 

clean: 
	rm -f *.o $(LOCALBIN) $(SYSBIN)
