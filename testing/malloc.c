#include <stdio.h> /* for debugging */
#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <sys/mman.h>

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}
#endif


void *malloc(size_t nbytes) {
    printf("INSIDE MALLOC\n");
    return NULL;
}


void free(void * block) {

}


void *realloc(void * block, size_t nbytes) {
    return NULL;
}