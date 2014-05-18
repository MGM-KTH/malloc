#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1024
#define LOOPS 500 /* many loops for small data */

int main(int argc, char * argv[]){
    unsigned memory_size;
    while(scanf("%u", &memory_size) == 1) {
        int i,j;
        char *a[SIZE];
        void *memory_start, *memory_end;

        memory_start = endHeap();

        clock_t start = clock(), diff;

        for (i = 0; i < LOOPS; ++i) {
            for (j = 0; j < SIZE; ++j) {
                a[j] = malloc(memory_size);
            }
        }
        
        memory_end = endHeap();

        diff = clock() - start;
        int msec = diff * 1000 / CLOCKS_PER_SEC;

        for(j = 0; j < SIZE; j++){
            free(a[j]);
        }

        /* block size, memory and time */
        fprintf(stderr,"%u %u %d\n", memory_size,(unsigned)(memory_end - memory_start)/1000, msec);

        reset_free_list();

        /* block size and time */
        /*fprintf(stderr,"%u %d\n", memory_size,msec);*/

        /* block size and memory */
        /*fprintf(stderr,"%u %u\n", memory_size,(unsigned)(endHeap() - memory_start));*/
    }
    return 0;
}