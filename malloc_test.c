#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 512
#define SMALL_LOOPS 200
#define BIG_LOOPS 10

int main(int argc, char * argv[]){
    unsigned memory_size;
    int loops;
    while(scanf("%u", &memory_size) == 1) {

        int i,j;
        char *a[SIZE];
        void *memory_start, *memory_end;

        if (memory_size > 128)
            loops = BIG_LOOPS;
        else
            loops = SMALL_LOOPS;

        memory_start = endHeap();

        clock_t start = clock(), diff;

        for (i = 0; i < loops; ++i) {
            for (j = 0; j < SIZE; ++j) {
                a[j] = malloc(memory_size);
            }
        }
        memory_end = endHeap();

        for(j = 0; j < SIZE; j++){
            free(a[j]);
        }

        diff = clock() - start;
        int msec = diff * 1000 / CLOCKS_PER_SEC;
        double seconds = msec/1000 + (double)(msec%1000)/1000;

        /*fprintf(stderr,"%u %u %.3lfs\n", memory_size,(unsigned)(endHeap() - memory_start), seconds);*/
        fprintf(stderr,"%u %u\n", memory_size,(unsigned)(endHeap() - memory_start));
    }
    return 0;
}