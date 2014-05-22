#define _GNU_SOURCE

#ifndef MAP_ANONYMOUS
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

#define MIN_ALLOC 64 /* minimum number of units to request */
#define POSSIBLY_MALLOCED 0
#define ONLY_MAPPED 1

#ifdef MMAP
static void *__endHeap = NULL;

void *endHeap(void)
{
	if(__endHeap == NULL) __endHeap = sbrk(0);
	return __endHeap;
}
#endif

#ifndef STRATEGY
#define STRATEGY BEST_FIT /* default: best fit */
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

static header *malloced_headers  = NULL;
static header *free_list = NULL;
static header base; /* empty base header for free_list */


static header *request_memory(unsigned naligned)
{
	void *cp;
	header *up;

	unsigned pagesize = getpagesize();

	if(naligned < MIN_ALLOC) naligned = MIN_ALLOC;

#ifdef MMAP
	unsigned noPages;
	noPages = ((naligned*sizeof(header))-1)/pagesize + 1;
	if(noPages > 10) noPages *= 16; /* assume more large blocks follow */
	cp = mmap(__endHeap, noPages*pagesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	naligned = (noPages*pagesize)/sizeof(header);
	if (cp != MAP_FAILED)
		__endHeap += noPages*pagesize; /* increase total amount of allocations */
#else
	cp = sbrk(naligned*sizeof(header));
#endif
	if(cp == (void *) -1){ /* no space at all */
		perror("failed to get more memory");
		return NULL;
	}
	up = (header *) cp;
	up->block.size = naligned;
	free_memory((void *)(up+1), ONLY_MAPPED);
	return free_list;
}


/* malloc implementations: */


void register_malloced_header(header *h) {
	/*fprintf(stderr, "registering 0x%x\n", h);*/
	if (malloced_headers == NULL) {
		malloced_headers = h;
		h->block.next = h;
	}
	else {
		/*unregister_malloced_header(h+1);*/
		h->block.next = malloced_headers->block.next;
		malloced_headers->block.next = h;
		malloced_headers = h;
	}
}

int unregister_malloced_header(void *block) {
	if (malloced_headers == NULL)
		return 0;
	header *bh, *h, *prev_h;
	bh = (header *) block - 1; /* point to block header */
	/*fprintf(stderr, "trying to unregister 0x%x\n", bh);*/
	prev_h = malloced_headers;
	h = prev_h->block.next;
	while(1) {
/*		fprintf(stderr, "h = 0x%x\n", h);
		fprintf(stderr, "h next = 0x%x\n", h->block.next);
		fprintf(stderr, "prev_h = 0x%x\n", prev_h);*/
		if(h == bh) {
			/*fprintf(stderr, "SUCCESS \n");*/
			prev_h->block.next = h->block.next; /* unlink */
			malloced_headers = prev_h;
			if(prev_h == h) malloced_headers = NULL;
			return 1;
		}
		if(h == malloced_headers) {
			/*fprintf(stderr, "FAIL \n");*/
			return 0;
		}
		prev_h = h;
		h = h->block.next;
	}
}


#if STRATEGY == FIRST_FIT
void *malloc(size_t nbytes)
{
	if (nbytes == 0) return NULL;
	if(nbytes >= ULONG_MAX - sizeof(header)) return NULL; /* overflow */

	header *h, *prev_h;

	/* number of aligned units needed for nbytes bytes */
	unsigned naligned = (nbytes+sizeof(header)-1)/sizeof(header) + 1; 

	if (free_list == NULL) { /* initialize free_list */
		free_list = &base;
		base.block.next = free_list;
		base.block.size = 0;
	}

	/* loop free_list looking for memory */
	prev_h = free_list;
	h = prev_h->block.next;
	while(1) {
	/*for (h = prev_h->block.next; ; prev_h = h, h = h->block.next) {*/
		if(h->block.size >= naligned) {
			if (h->block.size == naligned) { /* found perfect block! */
				prev_h->block.next = h->block.next; /* unlink h */
			}else { /* bigger. allocate tail */
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
		prev_h = h;
		h = h->block.next;
	}
}
#endif

#if STRATEGY == BEST_FIT || STRATEGY == WORST_FIT
void *malloc(size_t nbytes)
{
/*	fprintf(stderr, "MALLOC CALLED\n");*/
	if(nbytes == 0) return NULL;
	if(nbytes >= ULONG_MAX - sizeof(header)) return NULL; /* overflow */

	header *h, *prev_h, *best = NULL, *prev_best = NULL;
	int new_memory = 0;
	unsigned threshold = sizeof(header);
	unsigned naligned = (nbytes+sizeof(header)-1)/sizeof(header) + 1; /* number of aligned units needed for nbytes bytes */

	if (free_list == NULL) { /* initialize free_list */
		free_list = &base;
		base.block.next = free_list;
		base.block.size = 0;
	}

	/* loop free_list looking for memory */
	prev_h = free_list;
	h = prev_h->block.next;
	while(1) {
/*		fprintf(stderr, "free_list = 0x%x\n", free_list);
		fprintf(stderr, "h = 0x%x\n", h);
		fprintf(stderr, "prev_h = 0x%x\n", prev_h);*/
		if(h->block.size >= naligned) {
#if STRATEGY == BEST_FIT
			if(best == NULL || h->block.size < best->block.size) {
				best = h;
				prev_best = prev_h;
			}
			if (best->block.size == naligned) /* perfect match */
				break;
#endif
#if STRATEGY == WORST_FIT
			if(best == NULL || h->block.size > best->block.size) {
				best = h;
				prev_best = prev_h;
			}
#endif
		}
		if(h == free_list) { /* wrapped around */
			/*fprintf(stderr, "WRAPPED AROUND. best = 0x%x\n", best);*/
			if (best != NULL) /* block found */
				break;
			h = request_memory(naligned); /* request more heap space */
			if (h == NULL) return NULL; /* no memory left */
			/*fprintf(stderr, "new memory is of size %u\n", h->block.size);*/
			new_memory = 1;
		}
		prev_h = h;
		h = h->block.next;
	}
	if(best->block.size <= naligned + threshold) { /* found perfect block! */
		/*fprintf(stderr, "unlinked best\n");*/
		prev_best->block.next = best->block.next; /* unlink best */
	}else {
		best->block.size -= naligned;
		best += best->block.size;
		best->block.size = naligned;
		/*fprintf(stderr, "tail allocated best\n");*/
	}
	register_malloced_header(best);
	free_list = prev_best;
	return (void *) (best+1); /* return start of block */        
}
#endif


void free(void *block) 
{
	free_memory(block, POSSIBLY_MALLOCED);
}

/*
 * Free the memory beginning at address block
 */
void free_memory(void *block, int flag)
{
	if(flag == POSSIBLY_MALLOCED) {
		if(!unregister_malloced_header(block))
			return;
	}
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
void *realloc(void *block, size_t nbytes)
{
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

void reset_free_list()
{
	header *h = free_list->block.next, *next_h;

	do {
		/* Save the pointer to the next free space */
		next_h = h->block.next;

		/* Unmap the space which the current header belongs to */
		munmap(h, h->block.size);

	}while(free_list != (h = next_h));

	/* Unmap free_list last since we have now looped through
 	 * the entire list and know there are no others
 	 */
	munmap(free_list, free_list->block.size);

	free_list = NULL;
}


#if LOCAL
int main()
{

	/* requesting one page of memory using mmap */
	void *m = mmap(0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	assert((unsigned)m != MAP_FAILED);
	void *s = sbrk(0);
	fprintf(stderr, "mmap: %u\n", (unsigned)(m));
	fprintf(stderr, "sbrk: %u\n", (unsigned)(s));
	fprintf(stderr, "%u\n", (unsigned)(s-m));
	assert((unsigned)(s-m) % getpagesize() == 0);



	void *p, *memory_start, *memory_end;

/*	memory_start = endHeap();

	p = malloc(1);
	p = malloc(1024);
	p = malloc(1024);
	p = malloc(2048);

	memory_end = endHeap();

	fprintf(stderr, "Memory used: %u\n", (unsigned)(memory_end - memory_start));
	fprintf(stderr, "(Should be 8192 with a page size of 4096 if MIN_ALLOC low enough)\n");*/

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

	return 0;
}
#endif


#endif
