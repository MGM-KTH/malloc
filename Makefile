CC = GCC
CFLAGS	= -c -g -Wall -ansi -DSTRATEGY=2 -O4

all: malloc

malloc: malloc.o 
	$(CC) malloc.o -o malloc

malloc.o: malloc.c
	$(CC) $(CFLAGS) malloc.c

clean: 
	rm -f *.o malloc