#include "Array.h"

#include <stdlib.h>
#include <string.h>
#include "Logger.h"

static i32 g_StartSize = 5;

void*
internal_array_reserve(void* array, i32 elementsCount, i32 elementSize)
{
    vassert(array == NULL);

    i32 capacity = elementsCount * elementSize;
    i32 size = capacity + sizeof(ArrayHeader);
    ArrayHeader* newHeader = (ArrayHeader*) memory_allocate(size);
    newHeader->Count = 0;
    newHeader->Capacity = elementsCount;
    newHeader->ElementSize = elementSize;

    return newHeader->Buffer;
}
#include <stdarg.h>

void*
internal_array_reserve_debug(void* array, i32 minCapacity, i32 elementSize, i32 line, const char* file)
{
    if (array != NULL)
    {
	GERROR("Wrong usage of array_reserve (line: %d file %s)\n", line, file);
    }

    vassert(array == NULL);

    return internal_array_reserve(array, minCapacity, elementSize);
}

void*
internal_array_grow(const void* array, i32 elementSize)
{
    ArrayHeader* newHeader = NULL;

    if (array != NULL)
    {
	u32 newCapacity = 2 * array_cap(array) + 1;
	u32 newSize = newCapacity * elementSize + sizeof(ArrayHeader);
	ArrayHeader* header = array_header(array);
	newHeader = (ArrayHeader*) memory_reallocate(header, newSize);
	newHeader->Capacity = newCapacity;
    }
    else
    {
	i32 size = g_StartSize * elementSize + sizeof(ArrayHeader);
	newHeader = (ArrayHeader*) memory_allocate(size);
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
	ArrayHeader* header = array_header(src);
	i32 count = header->Count;
	i32 elementSize = header->ElementSize;
	i32 size = count * elementSize + sizeof(ArrayHeader);

	ArrayHeader* newHeader = (ArrayHeader*) memory_allocate(size);
	newHeader->Count = count;
	newHeader->Capacity = count;
	newHeader->ElementSize = elementSize;
	vmemcpy(newHeader->Buffer, src, count * elementSize);

	return newHeader->Buffer;
    }

    return NULL;
}
