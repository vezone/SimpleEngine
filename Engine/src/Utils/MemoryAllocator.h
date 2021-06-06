#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

/*

Replace this peace of potato with light-weight debug wrapper on malloc && free

*/

#include <stdlib.h>
#include "Types.h"
#include "MemoryList.h"

void* memory_helper_allocate(i32 size, i32 line, const char* file);
void memory_helper_free(void* data, i32 line, const char* file);
i32 memory_helper_get_allocated_size();

MemoryList* memory_get_list();

#define memory_allocate(size) memory_helper_allocate(size, __LINE__, __FILE__)
#define memory_free(data) memory_helper_free(data, __LINE__, __FILE__)
#define internal_memory_allocate(size) malloc(size)
#define memory_reallocate(data, size) realloc(data, size)


#endif
