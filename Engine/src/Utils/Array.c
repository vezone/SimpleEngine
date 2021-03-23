#include "Array.h"

#include <stdlib.h>
#include "MemoryAllocator.h"

void*
array_grow(const void* array, i32 elemSize)
{
    BufferHeader* newHeader = NULL;

    if (array != NULL)
    {
	u32 newCapacity = 2 * array_cap(array) + 1;
	u32 newSize = newCapacity * elemSize + sizeof(BufferHeader);
	BufferHeader* header = array_hdr(array);
	newHeader = (BufferHeader*) memory_reallocate(header, newSize);
	newHeader->Capacity = newCapacity;
    }
    else
    {
	newHeader = (BufferHeader*) internal_memory_allocate(3 * elemSize + sizeof(BufferHeader));
	newHeader->Count = 0;
	newHeader->Capacity = 3;
    }

    return newHeader->Buffer;
}
