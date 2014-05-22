#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TIMES 1
#define SIZE 512

int main(int argc, char * argv[]){
    unsigned memory_size;
    while(scanf("%u", &memory_size) == 1) {
        /*fprintf(stderr, "block size: %u, pid: %u\n", memory_size, getpid());*/ /* used with MANY_RUNS.sh */
        int i,j;

        unsigned pagesize = getpagesize();

        char *a[SIZE];
        void *memory_start, *memory_end;

        for(j = 0; j < SIZE/2; ++j) {
            a[j] = malloc(memory_size*pagesize*10);
            free(a[j]);
        }
        for(j = SIZE/2+1; j < SIZE; ++j) {
            a[j] = malloc(memory_size*pagesize*3);           
            free(a[j]);
        }

        memory_start = endHeap();

        for(i = 0; i < TIMES; ++i) {
            for(j = 0; j < SIZE; ++j) {
                a[j] = malloc(pagesize*3);
            }
        }
        
        memory_end = endHeap();


        for(j = 0; j < SIZE; j++) {
            free(a[j]);
        }

        /* block size, memory and time */
        fprintf(stdout,"%u %u\n", memory_size,(unsigned)(memory_end - memory_start)/1000);

    #ifndef SYSTEM
        reset_free_list();
    #endif

        /* block size and time */
        /*fprintf(stderr,"%u %d\n", memory_size,msec);*/

        /* block size and memory */
        /*fprintf(stderr,"%u %u\n", memory_size,(unsigned)(endHeap() - memory_start));*/
    }
    return 0;
}
