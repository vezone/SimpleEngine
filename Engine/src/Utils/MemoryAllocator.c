#include "MemoryAllocator.h"

i32 g_Size;
MemoryList g_AllocList;

void*
memory_helper_allocate(i32 size, i32 line, const char* file)
{
    g_Size += size;

    if (g_AllocList.IsInitialized != 1)
    {
	g_AllocList.IsInitialized = 1;
	list_init(&g_AllocList);
    }

    void* data = malloc(size);
    MemoryBlock* block;
    block = (MemoryBlock*) internal_memory_allocate(sizeof(*block));
    block_create(block, data, size, file, line);
    list_add(&g_AllocList, block);

    return data;
}

void
memory_helper_free(void* data, i32 line, const char* file)
{
    //g_Size -= size;
}

i32
memory_helper_get_allocated_size()
{
    return g_Size;
}

MemoryList*
memory_get_list()
{
    return &g_AllocList;
}
