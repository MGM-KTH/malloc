#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 512
#define LOOPS 10

int main(int argc, char * argv[]){
    unsigned memory_size;
    while(scanf("%u", &memory_size) == 1) {
        int i,j;
        char *a[SIZE];
        void *memory_start, *memory_end;

        memory_start = endHeap();

        for (i = 0; i < LOOPS; ++i) {
            for (j = 0; j < SIZE; ++j) {
                a[j] = malloc(memory_size);
            }
        }
        memory_end = endHeap();

        for(j = 0; j < SIZE; j++){
            free(a[j]);
        }

        fprintf(stderr,"Memory used: %u\n", (unsigned)(endHeap() - memory_start));
    }
    return 0;
}