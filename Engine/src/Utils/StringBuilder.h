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

void vstring_builder_init(StringBuilder* builder);
void vstring_builder_append_character(StringBuilder* builder, char character);
void vstring_builder_append_string(StringBuilder* builder, char* string);
void vstring_builder_append_format(StringBuilder* builder, const char* format, ...);
void vstring_builder_replace(StringBuilder* builder, char old_character, char new_character);
void vstring_builder_destroy(StringBuilder* builder);

void vstring_builder_test();

#endif
