CC = GCC
CFLAGS	= -c -g -Wall -ansi -DSTRATEGY=2 -O4

all: malloc

malloc: malloc.o 
	$(CC) $(CFLAGS) malloc.o -o malloc

clean: 
	rm -f *.o malloc