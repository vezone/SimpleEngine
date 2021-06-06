#ifndef TEST_H
#define TEST_H

#include "Utils/String.h"
#include "Utils/StringBuilder.h"
#include "Utils/Path.h"
#include "Utils/Types.h"
#include "Utils/stb_ds.h"

typedef struct FunctionResult
{
    //NOTE(bies): Массив результирующих кодов
    i8 IsSuccess;
    i8* Codes;
    char* Builder;
} FunctionResult;

typedef struct FileInfo
{
    const char** Functions;
    FunctionResult** Results;
} FileInfo;

typedef struct TestTable
{
    const char** Filenames;
    FileInfo** Infos;
} TestTable;

FunctionResult* file_info_get_function_result(FileInfo* fileInfo, const char* functionName);
FileInfo* test_table_get_file_info(TestTable* testTable, const char* filename);
void test_set_function(const char* function);
void test(i8 testResult, const char* filename, const char* message);
FileInfo* file_get_info(const char* filename);
const char** test_get_filenames();

#define Condition(condition) { i8 temp = (condition); test(temp, __FILE__, #condition); }

// USAGE: String_IsEqual(vstring_copy("It's a string"), "It's a string")
#define String_IsEquals(a, b) { i8 temp = vstring_compare((a), (b)); test(temp, __FILE__, #a" == "#b); }
#define String_IsNotEquals(a, b) { i8 temp = vstring_compare((a), (b)); test(!temp, __FILE__, #a" != "#b); }
#define String_Value(str) test((str ? 1 : 0), __FILE__, (str ? str : "NULL"))
#define String_List_Value(list, joinCharacter) test((array_count(list) ? 1 : 0), __FILE__, (list ? vstring_join(list, joinCharacter) : "EMPTY"))

#define Int_Value(val)					\
    {							\
	char str[50];					\
	vstring_set(str, '\0', 50);			\
	vstring_parse_i32(str, (val));			\
	char* res = vstring_concat(#val": ", str);	\
	test(1, __FILE__, res);				\
    }

#define TEST(function)					\
    {							\
	test_set_function(#function);			\
	function;					\
    }

typedef struct TestInfo
{
    u32 Count;
    u32 ErrorsCount;
    u32 SuccessCount;
} TestInfo;

TestInfo test_get_info();

#endif
