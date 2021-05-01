#include "Array.h"

#include <stdlib.h>
#include <string.h>
#include "MemoryAllocator.h"

static i32 g_StartSize = 1;

void*
array_grow(const void* array, i32 elementSize)
{
    BufferHeader* newHeader = NULL;

    if (array != NULL)
    {
	u32 newCapacity = 2 * array_cap(array) + 1;
	u32 newSize = newCapacity * elementSize + sizeof(BufferHeader);
	BufferHeader* header = array_header(array);
	newHeader = (BufferHeader*) memory_reallocate(header, newSize);
	newHeader->Capacity = newCapacity;
    }
    else
    {
	i32 size = g_StartSize * elementSize + sizeof(BufferHeader);
	newHeader = (BufferHeader*) memory_allocate(size);
	newHeader->Count = 0;
	newHeader->Capacity = g_StartSize;
	newHeader->ElementSize = elementSize;
    }

    return newHeader->Buffer;
}

void*
internal_array_copy(const void* src)
{
    if ((src) != NULL)
    {
	i32 count = array_count((src));
	i32 i = 0;
	i32 elementSize = array_header((src))->ElementSize;
	i32 size = count * elementSize + sizeof(BufferHeader);
	BufferHeader* newHeader = (BufferHeader*) memory_allocate(size);
	newHeader->Count = count;
	newHeader->Capacity = count;
	newHeader->ElementSize = elementSize;
	memcpy(newHeader->Buffer, src, count * elementSize);

	return newHeader->Buffer;
    }

    return NULL;
}
