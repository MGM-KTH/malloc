#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SIZE 128
#define LOOPS 20

int main(int argc, char * argv[]){
    unsigned pages;
    while(scanf("%u", &pages) == 1) {
        /*fprintf(stderr, "block size: %u, pid: %u\n", memory_size, getpid());*/ /* used with MANY_RUNS.sh */
        int i,j;
        char *a[SIZE];
        void *memory_start, *memory_end;

        memory_start = endHeap();

        clock_t start = clock(), diff;

        for (i = 0; i < LOOPS; ++i) {
            for (j = 0; j < SIZE; ++j) {
                a[j] = malloc(pages*getpagesize());
            }
        }
        
        memory_end = endHeap();

        diff = clock() - start;
        int msec = diff * 1000 / CLOCKS_PER_SEC;

        for(j = 0; j < SIZE; j++){
            free(a[j]);
        }

        /* block size, memory and time */
        fprintf(stdout,"%u %u %d\n", pages*getpagesize(),(unsigned)(memory_end - memory_start)/1000, msec);

#if STRATEGY != SYSTEM_MALLOC
        reset_free_list();
#endif        

        /* block size and time */
        /*fprintf(stderr,"%u %d\n", memory_size,msec);*/

        /* block size and memory */
        /*fprintf(stderr,"%u %u\n", memory_size,(unsigned)(endHeap() - memory_start));*/
    }
    return 0;
}
