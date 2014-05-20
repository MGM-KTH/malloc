#ifndef _MALLOC_MGM_H_
#define _MALLOC_MGM_H_

#include <stdlib.h> /* size_t */

#define SYSTEM_MALLOC 0
#define FIRST_FIT 1
#define BEST_FIT 2
#define WORST_FIT 3
#define QUICK_FIT 4

void *malloc(size_t);
void *malloc_ff(size_t);
void *malloc_bf(size_t);
void free(void *);
void *realloc(void *, size_t);
void reset_free_list(); /* for testing */

#ifndef _MACHTYPES_H_ /* sys/types.h */
/*typedef void * caddr_t;*/
#endif

#endif /* end header guard */
