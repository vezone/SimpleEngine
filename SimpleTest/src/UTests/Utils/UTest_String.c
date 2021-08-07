#include "UTest_String.h"

#include <stdlib.h>

#include "UTests/Test.h"
#include "Utils/String.h"

static void
failure_test()
{
    Condition(1 == 0);
}

static void
success_test()
{
    Condition(1 == 1);
}

static void
length_test()
{
    Condition(vstring_length("string") == 6);
    Condition(vstring_length("      ") == 6);
    Condition(vstring_length("\n      \n") == 8);
}

static void
string_set_test()
{
    char str[] = "Hello string!";
    vstring_set(str, 'C', 5);
    Condition(vstring_compare(str, "CCCCC string!"));

    vstring_set(str, '\n', 5);
    Condition(vstring_compare(str, "\n\n\n\n\n string!"));

    vstring_set(str, '\0', 1);
    Condition(vstring_length(str) == 0);

}

static void
vstring_copy_test()
{
    //String_IsEquals(vstring_copy("It's me, Mario!"), "It's me, Mario!");
}

static void
vstring_concat_test()
{
    String_Equal(vstring_concat("It's me, ", "Mario!"), "It's me, Mario!");
    String_Equal(vstring_concat("\n\n\n", "\n\n"), "\n\n\n\n\n");
    String_Equal(vstring_concat("123", " 456 "), "123 456 ");
}

static void
vstring_concat3_test()
{
    String_Equal(vstring_concat3("It's me", ", ", "Mario!"), "It's me, Mario!");
    String_Equal(vstring_concat3("left", " middle ", "right"), "left middle right");
}

static void
vstring_compare_test()
{
    String_Equal("Constant string", "Constant string");
    String_NotEqual("Constant string", " onstan string");
}

static void
vstring_test()
{
    String_Equal(vstring("const char* string"), "const char* string");
}

static void
vstring_substring_test()
{
    String_Equal(vstring_substring("const char* input", 6), "char* input");
    String_Equal(vstring_substring("0123456789", 3), "3456789");
}

static void
vstring_substring_range_test()
{
    String_Equal(vstring_substring_range("const char* input", 6, 10), "char*");
    String_Equal(vstring_substring_range("0123456789", 3, 7), "34567");
}

static void
vstring_split_test()
{
    char** splitted = vstring_split("/home/bies/Data/C/programming/Engine", '/');

    for (i32 i = 0; i < array_len(splitted); i++)
    {
	String_Value(splitted[i]);
    }
}

static void
vstring_join_test()
{
    const char** list = NULL;
    array_push(list, "Hello");
    array_push(list, "it");
    array_push(list, "is");
    array_push(list, "me");

    char* joinedValue = vstring_join(list, ' ');
    String_Equal(joinedValue, "Hello it is me");
    String_Value(joinedValue);
}

static void
istring_test()
{
    Condition(istring("This is string") == istring("This is string"));

    const char* istr = istring("Interning string");
    u32 length = istring_length(istr);
    Condition(istring_length(istr) == 16);
    Condition(length);
}

void
vmemset_test()
{
    char* dest1 = vstring("00000");
    vmemset(dest1, '1', 5);

    String_Value(dest1);
    String_Equal(dest1, "11111");
}

void
vmemcpy_test()
{
    const char* src1 = "This is string i want to copy!!!";
    i32 length = vstring_length(src1);
    char* dest1 = vstring_allocate(length);
    vmemcpy(dest1, src1, length);

    String_Value(dest1);
    String_Equal(dest1, src1);
}

static char* g_MemcpyTestString = "This is string i want to copy!!!";

void
vmemcpy_profiling_test()
{
    const char* src1 = g_MemcpyTestString;
    i32 length = vstring_length(src1);
    char* dest1 = vstring_allocate(length);
    vmemcpy(dest1, src1, length);
}

void
vmemcpy_wo_restrict_profiling_test()
{
    const char* src1 = g_MemcpyTestString;
    i32 length = vstring_length(src1);
    char* dest1 = vstring_allocate(length);
    vmemcpy_wo_restrict(dest1, src1, length);
}

void
vmemcpy_standart_profiling_test()
{
    const char* src2 = g_MemcpyTestString;
    i32 length = vstring_length(src2);
    char* dest2 = vstring_allocate(length);
    memcpy(dest2, src2, length);
}

void string_test()
{
    TEST(failure_test());
    TEST(success_test());
    TEST(length_test());
    TEST(string_set_test());

    TEST(vstring_concat_test());
    TEST(vstring_concat3_test());
    TEST(vstring_compare_test());

    TEST(vstring_test());
    TEST(vstring_substring_test());
    TEST(vstring_substring_range_test());
    TEST(vstring_split_test());
    TEST(vstring_join_test());

    TEST(istring_test());

    TEST(vmemset_test());
    TEST(vmemcpy_test());
}
