#include "MemoryAllocator.h"

i32 g_Size;

void*
memory_helper_allocate(i32 size, i32 line, const char* file)
{
    g_Size += size;

    void* data = malloc(size);
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
