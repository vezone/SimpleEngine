#ifndef STRING_H
#define STRING_H

#include "Types.h"

char* istring(const char* src);
u32 istring_length(char* str);

char* vstring(const char* string);
void vstring_set(char* string, char c, u32 length);
u32 vstring_length(const char* str);
char* vstring_assign(char* str, const char* oth, u32 length);
char* vstring_copy(const char* oth, u32 length);
char* vstring_concat(char* left, char* right);
char* vstring_concat3(char* left, char* middle, char* right);
/*
compare logic:
*  0 - means left != rigth
*  1 - means left == rigth
*/
i32 vstring_compare(const char* left, const char* right);
char* vstring_to_upper(const char* input);
char* vstring_to_lower(const char* input);
i32 vstring_index_of(const char* input, char character);
i32 vstring_index_of_string(const char* input, const char* string);
i32 vstring_last_index_of(const char* input, char character);
char* vstring_substring(const char* input, i32 beginIndex);
char* vstring_substring_range(const char* input, i32 beginIndex, i32 endIndex);
char* vstring_cut(const char* input, u32 begin, u32 end);

char** vstring_split(char* input, char separator);

char* vstring_join(char** list, char joinCharacter);

void vstring_parse_i32(char* input, i32 number);
void vstring_parse_i64(char* input, i64 number);
void vstring_parse_f64(char* input, f64 number);

force_inline char
vchar_to_upper(char character)
{
    if (character >= 'a' && character <= 'z')
	return (character - 'a' + 'A');
    else
	return character;
}

force_inline char
vchar_to_lower(char character)
{
    if (character >= 'A' && character <= 'Z')
	return (character - 'A' + 'a');
    else
	return character;
}

#endif
