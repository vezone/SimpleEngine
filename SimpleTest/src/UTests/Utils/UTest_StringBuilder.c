#include "UTest_StringBuilder.h"

#include "UTests/Test.h"
#include "Utils/StringBuilder.h"

void
string_builder_appendc_test()
{
    char* sb = NULL;
    string_builder_appendc(sb, 'c');
    string_builder_appendc(sb, 'h');
    string_builder_appendc(sb, 'a');
    string_builder_appendc(sb, 'r');
    string_builder_appendc(sb, ' ');
    string_builder_appendc(sb, 'c');
    string_builder_appendc(sb, 'r');
    string_builder_appendc(sb, 'e');
    string_builder_appendc(sb, 'a');
    string_builder_appendc(sb, 't');
    string_builder_appendc(sb, 'i');
    string_builder_appendc(sb, 'o');
    string_builder_appendc(sb, 'n');

    Condition(string_builder_buffer(sb) != NULL);
    String_Value(string_builder_buffer(sb));
}

void
string_builder_appends_test()
{
    char* sb = NULL;
    // 5 * 10
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    string_builder_appends(sb, "01234567890123456789012345678901234567890123456789");

    String_Value(string_builder_buffer(sb));
    Int_Value(string_builder_count(sb));
    Int_Value(string_builder_capacity(sb));
}

void
string_builder_appends_big_string_test()
{
    char* sb = NULL;

    string_builder_appends(sb, "first\n");

    for (i32 i = 0; i < 100000; i++)
    {
	string_builder_appends(sb, "01234567890123456789012345678901234567890123456789\n");
    }

    string_builder_appends(sb, "last\n");

    Int_Value(string_builder_count(sb));
    Int_Value(string_builder_capacity(sb));
    //GERROR("%s\n", sb);
    String_Value(string_builder_buffer(sb));

}

void
string_builder_appendf_test()
{
    char* sb = NULL;
    string_builder_appendf(sb, "Format: %s - %d - %b", "string", 10, 123);

    String_Value(string_builder_buffer(sb));
    Int_Value(string_builder_count(sb));
    Int_Value(string_builder_capacity(sb));
}

void
string_builder_test()
{
    TEST(string_builder_appendc_test());
    TEST(string_builder_appends_test());
    TEST(string_builder_appendf_test());
    TEST(string_builder_appends_big_string_test());
}
