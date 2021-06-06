#ifndef ARRAY_H
#define ARRAY_H

#include "Types.h"
#include "MemoryAllocator.h"
#include <stdarg.h>

typedef struct BufferHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    u8 Buffer[0];
} BufferHeader;

/*
  The macro offsetof() returns the offset of the field
  member from the start of the structure type
  NOTE it's offset because we take 0 address memory
  convert it to size_t type
  #define offset(type, mem) (size_t) &(((type*)0)->mem)
*/

void* internal_array_reserve(void* array, i32 minCapacity, i32 elementSize);
void* internal_array_grow(const void* array, i32 elementSize);
void* internal_array_copy(const void* src);

#define array_header(b) ((BufferHeader*) (((u8*)b) - sizeof(BufferHeader)))
#define array_len(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_count(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_cap(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_capacity(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_element_size(b) ((b != NULL) ? array_header(b)->ElementSize : 0)

#define array_reserve(b, elementsCount) internal_array_reserve(b, elementsCount, sizeof(*b))

#define array_set(b, v)						\
    {								\
	vassert(b);						\
	i32 count = array_count(b);				\
	for (i32 i = 0; i < count; i++)				\
	{							\
	    b[i] = v;						\
	};							\
    }

#define array_push(b, ...)					\
    {								\
	if ((b) == NULL || array_len(b) >= array_cap(b))	\
	{							\
	    (b) = internal_array_grow(b, sizeof(*b));		\
	}							\
	b[array_len(b)] = (__VA_ARGS__);			\
	array_header(b)->Count++;				\
    }

#define array_push_at(b, i, ...)				\
    {								\
	if ((b) == NULL || array_len(b) >= array_cap(b))	\
	{							\
	    (b) = internal_array_grow(b, sizeof(*b));		\
	}							\
	b[i] = (__VA_ARGS__);					\
	array_header(b)->Count++;				\
    }

#define array_end(b) (((void*)b) + array_count(b) * sizeof(*b))
#define array_foreach(b)			\
    for (__typeof__(b[0])* item = b; item != array_end(b); item++)

#define array_copy(src)	internal_array_copy(src)
#define array_free(b) ((b) ? memory_free(array_header(b)) : 0)

#endif
