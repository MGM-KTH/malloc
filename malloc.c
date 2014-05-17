#define _GNU_SOURCE
#include <stdio.h> /* for debugging */
#include "brk.h"
#include "malloc.h"
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



#ifdef MMAP
static void *__endHeap = 0;

void *endHeap(void)
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
			h = request_memory(naligned);
			if (h == NULL) return NULL; /* no memory left */
		}
		/* move to next entry */
		prev_h = h;
		h = h->block.next;
	}
}

/*
 * Free the memory beginning at adress block
 */
void free(void *block) {
	header *bh, *h;

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
	if( NULL == block) { /* If block ptr is NULL, behave as malloc */
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
		void* new_area = malloc(nbytes);
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

		return (void *)(bh+1);
	}

	return NULL;
}
