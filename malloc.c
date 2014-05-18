#define SYSTEM_MALLOC 0
#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3
#define QUICK_FIT 4

#ifndef STRATEGY
#define STRATEGY BEST_FIT /* default: best fit */
#endif

#define _GNU_SOURCE

#ifdef __APPLE__
#define MAP_ANONYMOUS MAP_ANON
#endif

#include <stdio.h> /* for debugging */
#include <assert.h> /* for debugging */
#include "brk.h"
#include "malloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <limits.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define MIN_ALLOC 1024 /* minimum number of bytes to request */

#ifdef MMAP
static void *__endHeap = 0;

void *endHeap(void)
{
    if(__endHeap == 0) __endHeap = sbrk(0);
    return __endHeap;
}
#endif

#ifdef LOCAL
int main() {
    void *p;
    /* detta skall fungera korrekt */ 
    p = malloc(0);
    free(p);
    free(NULL);
    /* detta skall ge tillbaka p == NULL */
    struct rlimit r;
    getrlimit(RLIMIT_DATA, &r);
    p = malloc(2 * r.rlim_max);
    assert(p == NULL);
    /* och dessa? */
    p = realloc(NULL, 17); 
    assert(p != NULL); /* as malloc 17 */
    p = realloc(p, 0);
    assert(p == NULL); /* as free, returns null */
    p = realloc(NULL, 0);
    assert(p == NULL); /* empty free */
}
#endif

#if STRATEGY != SYSTEM_MALLOC

typedef double alignment_variable; /* the largest possible alignment size */

/*
 * |                       |      |                     |
 * | pointer to next block | size |        block        |
 * |                       |      |                     |
 *                                 ^ address returned to user
 */

/* block header */ 
union header {
	struct {
		union header *next; /* pointer to next block (header) */
		unsigned size;      /* Includes header */
	} block;
	alignment_variable x;   /* align header size */
};

typedef union header header; /* skip the union keyword */

static header head; /* head of linked list of allocated memory */
static header *free_list = NULL;


static header *request_memory(unsigned naligned) {
	void *cp;
	header *up;

#ifdef MMAP
	unsigned noPages;
	if(__endHeap == 0) __endHeap = sbrk(0);
#endif

    /*
    if(naligned < MIN_ALLOC) {
        if(naligned > MIN_ALLOC/4) {
            naligned = MIN_ALLOC;
        }
    }
    */
	/*if(naligned < MIN_ALLOC) naligned = MIN_ALLOC;*/

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


#if STRATEGY == FIRST_FIT
void *malloc(size_t nbytes) {
	if (nbytes == 0) return NULL;

	header *h, *prev_h;

	/* number of aligned units needed for nbytes bytes */
	unsigned naligned = (nbytes+sizeof(header)-1)/sizeof(header) + 1; 

	if (free_list == NULL) { /* initialize free_list */
		free_list = &head;
		head.block.next = free_list;
		head.block.size = 0;
	}

	/* loop free_list looking for memory */
	prev_h = free_list;
	h = prev_h->block.next;
	while(1) {
		if(h->block.size >= naligned) {
			if (h->block.size == naligned) { /* found perfect block! */
                prev_h->block.next = h->block.next; /* unlink h */
			}else if(h->block.size > naligned) { /* big enough */
				/*h += (h->block.size - naligned);*/ /* offset h */
				/*h->block.size = naligned;*/ /* update size */
                h->block.size -= naligned;
                h += h->block.size;
                h->block.size = naligned;
			}
			free_list = prev_h;
			return (void *) (h + 1); /* return start of block */
		}

		if(h == free_list) { /* wrapped around */
			h = request_memory(naligned); /* request more heap space */
			if (h == NULL) return NULL; /* no memory left */
		}
		/* move to next entry */
		prev_h = h;
		h = h->block.next;
	}
}
#endif

#if STRATEGY == BEST_FIT
void *malloc(size_t nbytes) {
    if(nbytes == 0) return NULL;
    if(nbytes >= ULONG_MAX - sizeof(header)) return NULL; /* overflow */

    header *h, *prev_h, *best = NULL, *prev_best = NULL;
    unsigned threshold = sizeof(header);
    unsigned naligned = (nbytes+sizeof(header)-1)/sizeof(header) + 1; /* number of aligned units needed for nbytes bytes */

    if (free_list == NULL) { /* initialize free_list */
        free_list = &head;
        head.block.next = free_list;
        head.block.size = 0;
    }

    /* loop free_list looking for memory */
    prev_h = free_list;
    h = prev_h->block.next;
    while(1) {
        if(h->block.size >= naligned) {
            if(best == NULL || h->block.size < best->block.size) {
                best = h;
                prev_best = prev_h;
            }
            if (best->block.size == naligned) /* perfect match */
                break;
        }
        if(h == free_list) { /* wrapped around */
            if (best != NULL) /* block found */
                break;
            h = request_memory(naligned); /* request more heap space */
            if (h == NULL) return NULL; /* no memory left */

        }
        /* move to next entry */
        prev_h = h;
        h = h->block.next;
    }
    if(best->block.size <= naligned + threshold) { /* found perfect block! */
        prev_best->block.next = best->block.next; /* unlink best */
    }else {
        best->block.size -= naligned;
        best += best->block.size;
        best->block.size = naligned;
    }
    free_list = prev_best;
    return (void *) (best+1); /* return start of block */        
}
#endif


/*
 * Free the memory beginning at address block
 */
void free(void *block) {
	header *bh, *h; /* block header and loop variable */

	if(block == NULL) return; /* Nothing to do */

	bh = (header *) block - 1; /* point to block header */
	h = free_list;

    /*for(h = free_list; !(bh > h && bh < h->block.next); h = h->block.next) {*/
    while(!(bh > h && bh < h->block.next)) {
        if(h >= h->block.next && (bh > h || bh < h->block.next))
            break; /* freed block at start or end of arena */
        h = h->block.next;
    }

    if(bh + bh->block.size == h->block.next) { /* join to upper nb */
        bh->block.size += h->block.next->block.size;
        bh->block.next = h->block.next->block.next;
    }else
        bh->block.next = h->block.next;
    if(h + h->block.size == bh) { /* join to lower nbr */
        h->block.size += bh->block.size;
        h->block.next = bh->block.next;
    }else
        h->block.next = bh;
    free_list = h;
}

/*
 * Shrink or grow the memory area beginning at adress block,
 * move memory area if necessary
 */
void *realloc(void *block, size_t nbytes) {
	if(block == NULL) { /* If block ptr is NULL, behave as malloc */
		return malloc(nbytes);
	}else if( 0 == nbytes ) { /* If size is 0 and ptr!=NULL, behave as free */
		free(block);
		return NULL;
	}

	header *bh, *h;

	unsigned nunits = (nbytes + sizeof(header) - 1) / sizeof(header) + 1;

	/* Get the header for existing block */
	bh = (header *) block - 1;

	/* No change in size, return to sender */
	if(nunits == bh->block.size) {
		return block;
	}

	/* Increase size */
	if(nunits > bh->block.size) {
		/* Lazy implementation */
		/* Allocate new memory */
		void *new_area = malloc(nbytes);
        if (new_area == NULL) /* malloc failed */
            return NULL;

		/* Copy data from old block to new area */
		memcpy(new_area, block, (bh->block.size-1)*sizeof(header));

		/* Free old block */
		free(block);

		return new_area;

	}else{ /* Decrease size */
		/* Get start position of new block to be freed */
		h = bh + nunits;
		/* Set blocksize of new free block */
		h->block.size = bh->block.size - nunits;
		/* Update blocksize of block to be shrinked */
		bh->block.size = nunits;

		/* Free the tail */
		free((void *)(h+1));

		return block;
	}

	return NULL;
}

#endif