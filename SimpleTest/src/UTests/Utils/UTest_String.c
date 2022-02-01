#include "UTest_String.h"

#include <stdlib.h>
#include "UTests/Test.h"
#include <Utils/SimpleStandardLibrary.h>

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
    Condition(string_length("string") == 6);
    Condition(string_length("      ") == 6);
    Condition(string_length("\n      \n") == 8);
}

static void
string_set_test()
{
    char str[] = "Hello string!";
    string_set(str, 'C', 5);
    Condition(string_compare(str, "CCCCC string!"));

    string_set(str, '\n', 5);
    Condition(string_compare(str, "\n\n\n\n\n string!"));

    string_set(str, '\0', 1);
    Condition(string_length(str) == 0);

}

static void
string_copy_test()
{
    //String_IsEquals(string_copy("It's me, Mario!"), "It's me, Mario!");
}

static void
string_concat_test()
{
    String_Equal(string_concat("It's me, ", "Mario!"), "It's me, Mario!");
    String_Equal(string_concat("\n\n\n", "\n\n"), "\n\n\n\n\n");
    String_Equal(string_concat("123", " 456 "), "123 456 ");
}

static void
string_concat3_test()
{
    String_Equal(string_concat3("It's me", ", ", "Mario!"), "It's me, Mario!");
    String_Equal(string_concat3("left", " middle ", "right"), "left middle right");
}

static void
string_compare_test()
{
    String_Equal("Constant string", "Constant string");
    String_NotEqual("Constant string", " onstan string");
}

static void
string_ctr_test()
{
    String_Equal(string("const char* string"), "const char* string");
}

static void
string_substring_test()
{
    String_Equal(string_substring("const char* input", 6), "char* input");
    String_Equal(string_substring("0123456789", 3), "3456789");
}

static void
string_substring_range_test()
{
    String_Equal(string_substring_range("const char* input", 6, 10), "char*");
    String_Equal(string_substring_range("0123456789", 3, 7), "34567");
}

static void
string_split_test()
{
    char** splitted = string_split("/home/bies/Data/C/programming/Engine", '/');

    for (i32 i = 0; i < array_len(splitted); i++)
    {
	String_Value(splitted[i]);
    }
}

static void
string_join_test()
{
    const char** list = NULL;
    array_push(list, "Hello");
    array_push(list, "it");
    array_push(list, "is");
    array_push(list, "me");

    char* joinedValue = string_join(list, ' ');
    String_Equal(joinedValue, "Hello it is me");
    String_Value(joinedValue);
}

void
istring_test()
{
    Condition(istring("This is string") == istring("This is string"));

    const char* istr = istring("Interning string");
    u32 length = istring_length(istr);
    Condition(istring_length(istr) == 16);
    Condition(istring_length(istr));
}

void
istring_crash_test()
{
    istring_free_headers();

    const char* istrCached = NULL;
    const char* istrCached2 = NULL;
    TimeState state;
    profiler_start(&state);
    //STUPID TEST FOR CHECKING IF THERE ANY BUG IN CREATION
    i32 i, count = 5000;
    for (i = 0; i < count; ++i)
    {
	char buf[256];
	sprintf(buf, "Hello Hello Hello %i", i);
	const char* istr = istring(buf);
	if (i == 7)
	{
	    istrCached2 = istr;
	}
	else if (i == 2401)
	{
	    istrCached = istr;
	}
    }
    profiler_end(&state);
    i64 ms = profiler_get_milliseconds(&state);
    I32_Value(ms);

    String_Value("ISTRING_LENGTH");
    profiler_start(&state);
    i32 length = istring_length(istrCached);
    profiler_end(&state);
    i32 ns = profiler_get_nanoseconds(&state);
    I32_Value(ns);

    String_Value("STRING_LENGTH");
    profiler_start(&state);
    length = string_length(istrCached);
    profiler_end(&state);
    ns = profiler_get_nanoseconds(&state);
    I32_Value(ns);

    String_Value("ISTRING_COMPARE");
    profiler_start(&state);
    i32 isEqual = istrCached == istrCached2;
    profiler_end(&state);
    ns = profiler_get_nanoseconds(&state);
    I32_Value(isEqual);
    I32_Value(ns);

    String_Value("STRING_COMPARE");
    profiler_start(&state);
    isEqual = string_compare(istrCached, istrCached2);
    profiler_end(&state);
    ns = profiler_get_nanoseconds(&state);
    I32_Value(isEqual);
    I32_Value(ns);

    //IFile IElement
    IStringHeader** strs = istring_get_headers();
    I32_Value(array_count(strs));
}

void
string_string_to_i32_test()
{
    Condition(string_to_i32("1234") == 1234);
    Int_Value(string_to_i32("1234"));

    Condition(string_to_i32("-1000234") == -1000234);
    Condition(string_to_i32("-1000234"));
}

void
string_string_to_f32_test()
{
    {
	f32 result = string_to_f32("0.1234");
	F32_Equal_Epsilon(result, 0.1234f, 0.00001f);
	F32_Value(result);
    }

    {
	f32 result = string_to_f32("-.1234");
	F32_Equal_Epsilon(result, -.1234, 0.00001);
	F32_Value(result);
    }
}

void
string_parse_i32_test()
{
    char res[100];
    string_i32(res, 100245);

    String_Value(res);
    String_Equal(res, "100245");
}

void string_test()
{
    TEST(failure_test());
    TEST(success_test());
    TEST(length_test());
    TEST(string_set_test());

    TEST(string_concat_test());
    TEST(string_concat3_test());
    TEST(string_compare_test());

    TEST(string_ctr_test());
    TEST(string_substring_test());
    TEST(string_substring_range_test());
    TEST(string_split_test());
    TEST(string_join_test());

    TEST(istring_test());
    TEST(istring_crash_test());

    TEST(string_string_to_i32_test());
    TEST(string_string_to_f32_test());

    TEST(string_parse_i32_test());

}
