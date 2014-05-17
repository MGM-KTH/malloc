#include <stdio.h> /* for debugging */
#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <sys/mman.h>

#define MIN_ALLOC 1024 /* minimum nalignedmber of bytes to request */

#ifdef __APPLE__
#define MAP_ANONYMOUS MAP_ANON
#endif

typedef double alignment_variable; /* the largest possible alignment size */

/*
 * |                       |      |                     |
 * | pointer to next block | size | block_size          |
 * |                       |      |                     |
 *                                 ^ address returned to user
 */
union header { /* block header */ 
    struct {
        union header *next; /* pointer to next block (header) */
        unsigned size;
    } block;
    alignment_variable x; /* align header size */
};

typedef union header header; /* skip the union keyword */

static header head; /* head of linked list of allocated memory */
static header *free_list = NULL;


/*int main() {
    header something;
    double x;
    union header *next;
    unsigned block_size;

    printf("union header pointer size: %d\n", sizeof(next));
    printf("unsigned size: %d\n", sizeof(block_size));
    printf("double size (alignment_variable): %d bytes\n", sizeof(alignment_variable));
    printf("union header size: %d bytes\n", sizeof(something));
    return 0;
}*/

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}

#endif

static header *request_memory(unsigned naligned) {
  void *cp;
  header *up;

#ifdef MMAP
    unsigned noPages;
    if(__endHeap == 0) __endHeap = sbrk(0);
#endif

    if(naligned < MIN_ALLOC) naligned = MIN_ALLOC;

#ifdef MMAP
    noPages = ((naligned*sizeof(header))-1)/getpagesize() + 1;
    cp = mmap(__endHeap, noPages*getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    naligned = (noPages*getpagesize())/sizeof(header);
    __endHeap += noPages*getpagesize();
#else
    cp = sbrk(naligned*sizeof(header));
#endif
    if(cp == (void *) -1){ /* no space at all */
        perror("failed to get more memory");
        return NULL;
    }
    up = (header *) cp;
    up->block.size = naligned;
    free((void *)(up+1));
    return free_list;    
}

/*
 * Inspiration source: The C programming language, chapter 8.7
 *
 */
void *malloc(size_t nbytes) {
    if (nbytes == 0) return NULL;

    header *h;
    header *prev_h;
    /*nalignedmber of aligned units needed (rounded up of course, hence the -1 and +1) */
    unsigned naligned = (nbytes+sizeof(header)-1)/sizeof(header) + 1; 

    if (free_list == NULL) { /* initialize free_list */
        free_list = &head;
        head.block.next = free_list;
        head.block.size = 0;
    }
    /* loop free_list looking for memory */
    prev_h = free_list;
    h = prev_h->block.next;
    while (1) {
        fprintf(stderr, "naligned = %u\n", naligned);
        fprintf(stderr, "block size = %u\n", h->block.size);
        if (h->block.size >= naligned) {
            if (h->block.size == naligned) { /* found perfect block! */
                prev_h->block.next = h->block.next; /* unlink h */
            }
            else if (h->block.size > naligned) { /* big enough */
                h += (h->block.size - naligned); /* offset h */
                h->block.size = naligned; /* update size */
            }
            free_list = prev_h;
            return (void *) (h + 1); /* return start of block */
        }

        if (h == free_list) { /* wrapped around */
            h = request_memory(naligned);
            if (h == NULL) return NULL; /* no memory left */
            fprintf(stderr, "Got memory from OS: %u\n", stderr, h->block.size);
        }
        /* move to next entry */
        prev_h = h;
        h = h->block.next;
    }
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
