#include <stdio.h> /* for debugging */
#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <sys/mman.h>

#define MIN_ALLOC 1024 /* minimum number of bytes to request */

/*
 * Some reading on unions:
 * http://www.tutorialspoint.com/cprogramming/c_unions.htm
 * 
 * This is currently gibberish-code inspired by the reference impl.
 */
union header {
    struct {
        union header *nxt_hdr;
        unsigned block_size;
    } block;
    long x;
};

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}

#endif


void *malloc(size_t nbytes) {
    return NULL;
}


void free(void * block) {

}


void *realloc(void * block, size_t nbytes) {
	if( NULL == block) { /* If block ptr is NULL, behave as malloc */
		return malloc(nbytes);
	}else if( 0 == nbytes ) { /* If size is 0 and ptr!=NULL, behave as free */
		free(block);
		return NULL;
	}

    return NULL;
}
