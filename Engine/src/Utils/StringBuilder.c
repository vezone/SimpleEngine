#include <stdlib.h>
#include <stdio.h>

#include "StringBuilder.h"
#include "String.h"
#include "Logger.h"
#include "MemoryAllocator.h"

#define REALLOC_SIZE 511

void
vstring_builder_init(StringBuilder* builder)
{
    builder->Count = 0;
    builder->Capacity = REALLOC_SIZE;
    builder->Buffer = (char*) internal_memory_allocate(REALLOC_SIZE);
    vstring_set(builder->Buffer, '\0', REALLOC_SIZE);
}

void
vstring_builder_append_character(StringBuilder* builder, char character)
{
    if ((builder->Count + 1) <= builder->Capacity)
    {
	builder->Buffer[builder->Count++] = character;
    }
    else
    {
	i32 i;
	i32 new_length = builder->Capacity + REALLOC_SIZE;
	char* data = internal_memory_allocate(new_length);
	vstring_set(data, '\0', new_length);

	for (i = 0; i < builder->Count; i++)
	{
	    data[i] = builder->Buffer[i];
	}
	data[builder->Count++] = character;

	if (builder->Buffer)
	{
	    memory_free(builder->Buffer);
	}

	builder->Buffer = data;
    }
}

void
vstring_builder_append_string(StringBuilder* builder, char* string)
{
    i32 i;
    i32 string_length = vstring_length(string);

    if ((string_length + builder->Count) <= builder->Capacity)
    {
	for (i = 0; i < string_length; i++)
	{
	    builder->Buffer[i + builder->Count] = string[i];
	}
	builder->Count += string_length;
	builder->Buffer[builder->Count] = '\0';
    }
    else
    {
	i32 new_length = builder->Capacity + REALLOC_SIZE;
	char* data = internal_memory_allocate(new_length);
	vstring_set(data, '\0', new_length);

	for (i = 0; i < builder->Count; i++)
	{
	    data[i] = builder->Buffer[i];
	}

	for (i = 0; i < string_length; i++)
	{
	    data[i + builder->Count] = string[i];
	}
	builder->Count += string_length;

	if (builder->Buffer)
	{
	    memory_free(builder->Buffer);
	}

	builder->Buffer = data;
    }
}

void vstring_builder_append_format(StringBuilder* builder, const char* format, ...)
{
    i32 state = 0;
    i32 arguments_count = 0;
    va_list valist;

    for (char* ptr = (char*)format; *ptr != '\0'; ptr++)
    {
	if (*ptr == '%')
	{
	    state = 1;
	}
	else if (*ptr == 'c' || *ptr == 's' || *ptr == 'l' || *ptr == 'd' || *ptr == 'b' || *ptr == 'f')
	{
	    state = 0;
	    ++arguments_count;
	}
	else
	{
	    state = 0;
	}
    }

    if (arguments_count <= 0)
    {
	GERROR("Arguments count == 0\n");
	return;
    }

    state = 0;
    va_start(valist, format);
    while (*format != '\0')
    {
	switch (*format)
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
		state = 0;
		char elementc = (char) va_arg(valist, i32);
		vstring_builder_append_character(builder, elementc);
	    }
	    else
	    {
		vstring_builder_append_character(builder, *format);
	    }

	    break;
	}
	case 's':
	{
	    if (state == 1)
	    {
		state = 0;
		const char* elements = va_arg(valist, const char *);
		vstring_builder_append_string(builder, (char*) elements);
	    }
	    else
	    {
		vstring_builder_append_character(builder, *format);
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
		vstring_builder_append_character(builder, *format);
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
		vstring_builder_append_string(builder, str);

		state = 0;
	    }
	    else if (state == 2)
	    {
		i64 number = va_arg(valist, i64);
		char str[64];
		vstring_set(str, '\0', 64);
		vstring_parse_i64(str, number);
		vstring_builder_append_string(builder, str);

		state = 0;
	    }
	    else
	    {
		vstring_builder_append_character(builder, *format);
	    }

	    break;
	}
	case 'b':
	{
	    if (state == 1)
	    {
		i8 number = (i8) va_arg(valist, i32);
		char str[3];
		vstring_set(str, '\0', 3);
		vstring_parse_i32(str, number);
		vstring_builder_append_string(builder, str);

		state = 0;
	    }
	    else
	    {
		vstring_builder_append_character(builder, *format);
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
		vstring_builder_append_string(builder, str);

		state = 0;
	    }
	    else
	    {
		vstring_builder_append_character(builder, *format);
	    }

	    break;
	}
	default:
	{
	    vstring_builder_append_character(builder, *format);
	    state = 0;

	    break;
	}

	}
	format++;
    }
}

i32
vstring_builder_contains(StringBuilder* builder, const char* part)
{
    i32 index, equals = 0, c;
    char* temp = (char*) part;
    i32 temp_length = vstring_length(temp);

    for (index = 0; index < builder->Count; index++)
    {
	equals = 1;
	for (c = 0; c < temp_length; c++)
	{
	    if (builder->Buffer[c + index] != temp[c])
	    {
		equals = 0;
	    }
	}

	if (equals)
	{
	    return 1;
	}
    }

    return equals;
}

void vstring_builder_destroy(StringBuilder* builder)
{
    if (builder->Buffer)
    {
	free(builder->Buffer);
    }
}

void
vstring_builder_test()
{
    StringBuilder builder;
    vstring_builder_init(&builder);

#if 1
    vstring_builder_append_string(&builder, "Hello");
    vstring_builder_append_string(&builder, " ");
    vstring_builder_append_string(&builder, "my");
    vstring_builder_append_string(&builder, " ");
    vstring_builder_append_string(&builder, "name");
    vstring_builder_append_string(&builder, " ");
    vstring_builder_append_string(&builder, "is");
    vstring_builder_append_string(&builder, " ");
    //vstring_builder_append_string(&builder, "Vova");
    vstring_builder_append_character(&builder, 'V');
    vstring_builder_append_character(&builder, 'o');
    vstring_builder_append_character(&builder, 'v');
    vstring_builder_append_character(&builder, 'a');
    vstring_builder_append_string(&builder, "!");
    printf("%s\n", builder.Buffer);
#endif
    vstring_builder_append_string(&builder, " Hello");
    printf("%s\n", builder.Buffer);

    i32 cont = vstring_builder_contains(&builder, "dou");
    if (cont)
	printf("contains\n");
    else
	printf("does not\n");
}
