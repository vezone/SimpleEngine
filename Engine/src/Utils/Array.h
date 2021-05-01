#ifndef ARRAY_H
#define ARRAY_H

#include "Types.h"

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

void* array_grow(const void* array, i32 elementSize);
void* internal_array_copy(const void* src);

#define array_header(b) ((BufferHeader*) (((u8*)b) - sizeof(BufferHeader)))
#define array_len(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_count(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_cap(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_capacity(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_is_init(b) ((b != NULL) ? array_header(b)->IsInitialized : 0)

#define array_push(b, v)					\
    {								\
	if ((b) == NULL || (array_len(b) + 1) >= array_cap(b))	\
	{							\
	    (b) = array_grow(b, sizeof(*b));			\
	}							\
	b[array_len(b)] = (v);					\
	array_header(b)->Count++;				\
    }
#define array_copy(src)	internal_array_copy(src)
#define array_free(b) ((b) ? memory_free(array_header(b)) : 0)

#endif
