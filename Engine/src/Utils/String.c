#include "String.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Math/BaseMath.h"
#include "Array.h"
#include "Logger.h"
#include "Types.h"
#include "MemoryAllocator.h"

typedef struct InterningString
{
    u32 Length;
    const char* String;
} InterningString;

static InterningString* g_InterningStrings = NULL;

char*
istring(const char* src)
{
    vassert(src);

    i32 i;
    u32 length;
    u32 interningsCount;

    length = vstring_length(src);
    interningsCount = array_count(g_InterningStrings);

    for (i = 0; i < interningsCount; i++)
    {
	InterningString istr = g_InterningStrings[i];
	if (vstring_compare(istr.String, src))
	{
	    return istr.String;
	}
    }

    u32 size = (length + 1) * sizeof(char);
    char* str = (char*) internal_memory_allocate(size);
    memcpy(str, src, length);
    str[length] = '\0';

    InterningString istr = DEFAULT(InterningString);
    istr.String = str;
    istr.Length = length;

    array_push(g_InterningStrings, istr);

    return istr.String;
}

u32
istring_length(char* str)
{
    for (i32 i = 0; i < array_len(g_InterningStrings); i++)
    {
	InterningString istr = g_InterningStrings[i];
	if (istr.String == str)
	{
	    return istr.Length;
	}
    }

    return 0;
}

char*
vstring(const char* string)
{
    u32 length;
    char* newString;

    length = vstring_length(string);
    newString = (char*) memory_allocate((length + 1) * sizeof(char));
    newString[length] = '\0';
    memcpy(newString, string, length*sizeof(char));

    return newString;
}

void
vstring_set(char* string, char c, u32 length)
{
    i32 i;
    for (i = 0; i < length; i++)
    {
	string[i] = c;
    }
}

u32
vstring_length(const char* str)
{
    vassert(str);

    u32 index;
    for (index = 0; str[index] != '\0'; index++);
    return index;
}

char*
vstring_copy(const char* oth, u32 length)
{
    vassert(oth);

    char* result = (char*) memory_allocate(length * sizeof(char));
    memcpy(result, oth, length);
    return result;
}

char*
vstring_concat(char* left, char* right)
{
    u32 leftLength = vstring_length(left);
    u32 rightLength = vstring_length(right);

    char* newString = (char*) memory_allocate(leftLength + rightLength + 1);

    memcpy(newString, left, leftLength);
    memcpy(newString + leftLength, right, rightLength);
    newString[leftLength + rightLength] = '\0';

    return newString;
}

char*
vstring_concat3(char* left, char* middle, char* right)
{
    u32 leftLength = vstring_length(left);
    u32 middleLength = vstring_length(middle);
    u32 rightLength = vstring_length(right);

    char* newString = (char*) memory_allocate(leftLength + middleLength + rightLength + 1);

    memcpy(newString, left, leftLength);
    memcpy(newString + leftLength, middle, middleLength);
    memcpy(newString + leftLength + middleLength, right, rightLength);
    newString[leftLength + middleLength + rightLength] = '\0';

    return newString;
}

i32
vstring_compare(const char* left, const char* right)
{
    vassert(left);
    vassert(right);

    i32 leftLength  = vstring_length(left);
    i32 rightLength = vstring_length(right);

    if (leftLength != rightLength || leftLength == 0 || rightLength == 0)
    {
	return 0;
    }

    for (rightLength = 0; rightLength < leftLength; rightLength++)
    {
	if (left[rightLength] != right[rightLength])
	{
	    return 0;
	}
    }

    return 1;
}

char*
vstring_to_upper(const char* input)
{
    i32 i;
    i32 input_length = vstring_length(input);
    char* result = memory_allocate((input_length + 1) * sizeof(char));

    for (i = 0; i < input_length; i++)
    {
	char element = input[i];
	if (element >= 'a' && element <= 'z')
	{
	    result[i] = element - 'a' + 'A';
	}
	else
	{
	    result[i] = element;
	}
    }
    result[input_length] = '\0';
    return result;
}

char*
vstring_to_lower(const char* input)
{
    i32 i;
    i32 input_length;
    char  element;
    char* result;

    if (input == NULL)
    {
	return NULL;
    }

    input_length = vstring_length(input);
    result = memory_allocate((input_length + 1) * sizeof(char));
    for (i = 0; i < input_length; i++)
    {
	element = input[i];
	if (element >= 'A' && element <= 'Z')
	{
	    result[i] = element - 'A' + 'a';
	}
	else
	{
	    result[i] = element;
	}
    }
    result[input_length] = '\0';
    return result;
}

i32
vstring_index_of(const char* input, char character)
{
    i32 i;

    if (input == NULL)
    {
	return -1;
    }

    for (i = 0; input[i] != '\0'; i++)
    {
	if (input[i] == character)
	{
	    return i;
	}
    }

    return -1;
}

i32
vstring_index_of_string(const char* input, const char* string)
{
    i32 i;
    i32 j;
    i32 flag;
    i32 input_length;
    i32 string_length;

    vassert(input != NULL);
    vassert(string != NULL);

    input_length = vstring_length(input);
    string_length = vstring_length(string);

    if (input_length <= 0 || string_length <= 0)
    {
	return -1;
    }

    flag = -1;
    for (i = 0; i < input_length; i++)
    {
	for (j = 0; j < string_length; j++)
	{
	    if (input[i + j] == string[j])
	    {
		flag = 1;
	    }
	    else
	    {
		flag = -1;
		break;
	    }
	}

	if (flag == 1)
	{
	    return i;
	}
    }

    return -1;
}

i32
vstring_last_index_of(const char* input, char character)
{
    vassert(input != NULL);

    i32 i;
    i32 start_index;
    start_index = vstring_length(input) - 1;

    for (i = start_index; i >= 0; i--)
    {
	if (input[i] == character)
	{
	    return i;
	}
    }

    return -1;
}

char*
vstring_substring(const char* input, i32 start_index)
{
    i32 i;
    i32 new_length;
    i32 input_length;
    char* result;

    input_length = vstring_length(input);
    vassert(start_index < input_length);
    vassert(start_index > 0);

    new_length = input_length - start_index;
    result = memory_allocate((new_length + 1) * sizeof(char));
    memcpy(result, input + start_index, new_length);
    result[new_length] = '\0';
#if 0
    for (i = start_index; i < input_length; i++)
    {
	result[i - start_index] = input[i];
    }
#endif

    return result;
}

char*
vstring_substring_range(const char* input, i32 start_index, i32 end_index)
{
    i32 i;
    i32 input_length;
    i32 new_length;
    char* result;

    input_length = vstring_length(input);
    new_length = end_index - start_index + 1;
    result = memory_allocate((new_length + 1) * sizeof(char));

    vassert(start_index < input_length);
    vassert(start_index >= 0);
    vassert(input_length > end_index);
    vassert(start_index <= end_index);

    for (i = start_index; i <= end_index; i++)
    {
	result[i - start_index] = input[i];
    }

    result[new_length] = '\0';

    return result;
}

/*
3, 5
"01234567" - len: 8
"01267" - len: 5
*/
char*
vstring_cut(const char* input, u32 begin, u32 end)
{
    i32 i;
    u32 inputLength = vstring_length(input);
    u32 resultLength = inputLength - (end - begin + 1);
    //u32 resultLength = inputLength - end + begin - 1;

    assert(input);
    assert(begin >= 0);
    assert(end < inputLength);
    assert(begin < end);
    assert(inputLength);
    assert(resultLength);

    char* result = memory_allocate(resultLength);
    for (i = 0; i < begin; i++)
    {
	result[i] = input[i];
    }

    for (i = (end + 1); i < inputLength; i++)
    {
	result[begin + i - end - 1] = input[i];
    }

    return result;
}

char**
vstring_split(char* input, char splitCharacter)
{
    i32 i;
    i32 inputLength = vstring_length(input);
    i32 wordBeginIndex = 0;
    i8  isWordIndexSet = 0;
    char** result = NULL;

    for (i = 0; i < inputLength; i++)
    {
	char character = input[i];

	if (character != splitCharacter && !isWordIndexSet)
	{
	    isWordIndexSet = 1;
	    wordBeginIndex = i;
	}

	if (character == splitCharacter && isWordIndexSet)
	{
	    isWordIndexSet = 0;

	    char* word = vstring_substring_range(input, wordBeginIndex, i - 1);
	    array_push(result, word);
	}
	else if (i == (inputLength - 1))
	{
	    char* word = vstring_substring(input, wordBeginIndex);
	    array_push(result, word);
	}
    }

    return result;
}

char*
vstring_join(char** list, char joinCharacter)
{
    u32 i;
    u32 length;
    u32 finalLength;
    u32 curLength = 0;
    char* finalString;
    char* str;
    u32 strLength;

    vassert(list);
    length = array_len(list);
    vassert(length);

    finalLength = length;
    for (i = 0; i < length; i++)
    {
	finalLength += vstring_length(list[i]);
    }

    finalString = (char*) memory_allocate(finalLength);
    for (i = 0; i < (length - 1); i++)
    {
	str = list[i];
	strLength = vstring_length(str);
	memcpy(finalString + curLength, str, strLength);
	finalString[curLength + strLength] = joinCharacter;
	curLength += strLength + 1;
    }

    str = list[length - 1];
    strLength = vstring_length(str);
    memcpy(finalString + curLength, str, strLength);

    finalString[finalLength - 1] = '\0';

    return finalString;
}

void vstring_parse_i32(char* input, i32 number)
{
    i8 isNumberNegative = ((number < 0) ? 1 : 0);
    i32 i, rank = number_rank(number), numberLength = rank + isNumberNegative + 1;

    if (isNumberNegative)
    {
	input[0] = '-';
    }

    for (i = isNumberNegative; i < numberLength; i++)
    {
	input[i] = number_of_digit(number, rank) + 48;
	--rank;
    }
}

void vstring_parse_i64(char* input, i64 number)
{
    i8 isNumberNegative = ((number < 0) ? 1 : 0);
    i32 i, rank = number_rank(number), numberLength = rank + isNumberNegative + 1;

    if (isNumberNegative)
    {
	input[0] = '-';
    }

    for (i = isNumberNegative; i < numberLength; i++)
    {
	input[i] = number_of_digit(number, rank) + 48;
	--rank;
    }
}

void vstring_parse_f64(char* input, f64 number)
{
    if (vstring_length(input) < 64)
    {
	return;
    }

    sprintf(input, "%f", number);
}
