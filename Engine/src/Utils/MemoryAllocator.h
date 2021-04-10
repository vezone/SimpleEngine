#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

/*

Replace this peace of potato with light-weight debug wrapper on malloc && free

*/

#include <stdlib.h>

#define memory_allocate(size) malloc(size)
#define memory_free(data) free(data)
#define internal_memory_allocate(size) malloc(size)
#define memory_reallocate(data, size) realloc(data, size)


#endif
