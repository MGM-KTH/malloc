#include "malloc.h"
#include "brk.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#ifndef SIZE /* if SIZE is defined, so is all the others */
#define SIZE 128
#define TIMES 2000
#define PAGE 0
#endif

int main(int argc, char * argv[]){
    unsigned memory_size;
    while(scanf("%u", &memory_size) == 1) {
        /*fprintf(stderr, "block size: %u, pid: %u\n", memory_size, getpid());*/ /* used with MANY_RUNS.sh */
        int i,j;

#ifdef RANDOM
        srand(time(NULL));
#endif

#if PAGE || RANDOM
        unsigned pagesize = getpagesize();
#endif
        char *a[SIZE];
        void *memory_start, *memory_end;

        memory_start = endHeap();

        for (i = 0; i < TIMES; ++i) {
            for (j = 0; j < SIZE; ++j) {
#if PAGE
                a[j] = malloc(memory_size*pagesize);           
#elif RANDOM
                a[j] = malloc(memory_size*(rand()%pagesize));
#else
                a[j] = malloc(memory_size);                
#endif


            }
        }
        
        memory_end = endHeap();

        for(j = 0; j < SIZE; j++){
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
