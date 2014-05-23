#ifndef _brk_h_
#define _brk_h_


#define MMAP      /* if defined use mmap() instead of sbrk/brk */

#ifdef MMAP
void * endHeap(void);
#endif

#endif /* end header guard */