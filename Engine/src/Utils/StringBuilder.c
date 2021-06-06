#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "StringBuilder.h"
#include "String.h"
#include "Logger.h"
#include "MemoryAllocator.h"

char*
internal_string_builder_grow(char* builder, i32 newCount)
{
    i32 headerSize = sizeof(StringBuilderHeader);
    i32 bufferSize = newCount * sizeof(*builder);
    i32 newSize = bufferSize * sizeof(*builder) + headerSize;
    char* buffer =  NULL;

    if (builder == NULL)
    {
	StringBuilderHeader* header = (StringBuilderHeader*) internal_memory_allocate(newSize);
	header->Count = 0;
	header->Capacity = newCount;
	header->Buffer = (char*) (((char*)header) + headerSize);
	memset(header->Buffer, '\0', bufferSize);

	buffer = header->Buffer;
    }
    else
    {
	StringBuilderHeader* header = string_builder_header(builder);
	StringBuilderHeader* newHeader = (StringBuilderHeader*) internal_memory_allocate(newSize);
	newHeader->Count = header->Count;
	newHeader->Capacity = newCount;
	newHeader->Buffer = (char*) (((char*)newHeader) + headerSize);
	memset(newHeader->Buffer, '\0', bufferSize);
	memcpy(newHeader->Buffer, header->Buffer, bufferSize);

	memory_free(header);

	buffer = newHeader->Buffer;
    }

    return buffer;
}

char*
internal_string_builder_appendf(char* builder, const char* format, ...)
{
    i32 state = 0;
    i32 argumentsCount = 0;
    va_list valist;

    for (char* ptr = (char*)format; *ptr != '\0'; ptr++)
    {
	char c = *ptr;
	if (c == '%')
	{
	    state = 1;
	}
	else if (c == 'c' || c == 's' || c == 'd' || c == 'b' || c == 'f')
	{
	    state = 0;
	    ++argumentsCount;
	}
	else
	{
	    state = 0;
	}
    }

    vassert(argumentsCount > 0);

    state = 0;
    va_start(valist, format);
    while (*format != '\0')
    {
	char f = *format;

	switch (f)
	{
	case '%':
	{
	    state = 1;
	    break;
	}
	case 'c':
	{
	    if (state == 1)
	    {
		char elementc = (char) va_arg(valist, i32);
		string_builder_appendc(builder, elementc);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 's':
	{
	    if (state == 1)
	    {
		const char* elements = va_arg(valist, const char *);
		string_builder_appends(builder, (char*) elements);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'l':
	{
	    if (state == 1)
	    {
		state = 2;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'd':
	{
	    if (state == 1)
	    {
		i32 number = va_arg(valist, i32);
		char str[11];
		vstring_set(str, '\0', 11);
		vstring_parse_i32(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else if (state == 2)
	    {
		i64 number = va_arg(valist, i64);
		char str[64];
		vstring_set(str, '\0', 64);
		vstring_parse_i64(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'b':
	{
	    if (state == 1)
	    {
		u8 number = (u8) va_arg(valist, i32);
		char str[4];
		vstring_set(str, '\0', 4);
		vstring_parse_i32(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'f':
	{
	    if (state == 1)
	    {
		f64 f64Number = va_arg(valist, f64);
		char str[64];
		vstring_set(str, '\0', 64);
		vstring_parse_f64(str, f64Number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	default:
	{
	    string_builder_appendc(builder, f);
	    state = 0;

	    break;
	}

	}

	format++;
    }

    va_end(valist);

    return builder;
}
