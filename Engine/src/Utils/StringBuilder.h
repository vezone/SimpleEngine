#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdarg.h>
#include "Types.h"

typedef struct StringBuilder
{
    u32 Count;
    u32 Capacity;
    char* Buffer;
} StringBuilder;

typedef struct StringBuilderHeader
{
    i32 Count;
    i32 Capacity;
    char* Buffer;
} StringBuilderHeader;

char* internal_string_builder_grow(char* builder, i32 newCount);
char* internal_string_builder_appendf(char* builder, const char* format, ...);

#define string_builder_header(s) ((StringBuilderHeader*) (((u8*)s) - sizeof(StringBuilderHeader)))
#define string_builder_count(s) ((s) != NULL ? string_builder_header((s))->Count : 0)
#define string_builder_capacity(s) ((s) != NULL ? string_builder_header((s))->Capacity : 0)
#define string_builder_buffer(s) ((s) != NULL ? string_builder_header((s))->Buffer : NULL)

#define START_ALLOCATION_SIZE 253

#define string_builder_append_base(s, count)				\
    {									\
	i32 prevCapacity = string_builder_capacity((s));		\
	i32 doubledCount = (2 * (count) + 1);				\
									\
	if ((s) == NULL)						\
	{								\
	    i32 startAllocationSize = START_ALLOCATION_SIZE;		\
	    i32 newSize = (((doubledCount) > startAllocationSize) ? (doubledCount) : startAllocationSize); \
	    (s) = internal_string_builder_grow((s), newSize);		\
	}								\
	else if	((string_builder_count((s)) + doubledCount) >= prevCapacity) \
	{								\
	    i32 newCapacity = 2 * prevCapacity + 1;			\
	    i32 newSize = (((doubledCount) > newCapacity) ? doubledCount : newCapacity); \
	    (s) = internal_string_builder_grow((s), newSize);		\
	}								\
    }

#define string_builder_appendc(s, c)					\
    ({									\
	string_builder_append_base((s), 1);				\
	StringBuilderHeader* header = string_builder_header((s));	\
	header->Buffer[header->Count] = (c);				\
	++header->Count;						\
    })

#define string_builder_appends(s, str)					\
    ({									\
	i32 strLength = vstring_length((str));				\
	string_builder_append_base((s), strLength);			\
	StringBuilderHeader* header = string_builder_header((s));	\
	memcpy((header->Buffer + header->Count), (str), strLength*sizeof(*(s))); \
	header->Count += strLength;					\
    })

#define string_builder_appendf(s, f, ...)				\
    ({									\
	(s) = internal_string_builder_appendf((s), (f), ##__VA_ARGS__);	\
    })

#define string_builder_free(s) free(string_builder_header((s)))
#define string_builder_replace()

#if 0
void vstring_builder_replace(StringBuilder* builder, char old_character, char new_character);
void vstring_builder_destroy(StringBuilder* builder);
void vstring_builder_test();
#endif

#endif
