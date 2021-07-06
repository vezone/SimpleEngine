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

// TODO(bies): change _is_equal to equal

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

#define F32_Value(val)					\
    {							\
	char str[32];					\
	sprintf(str, "%s: %f", #val, val);		\
	test(1, __FILE__, str);				\
    }
#define F32_Is_Equal(a, b)				\
    {							\
	i32 result = f32_equal(a, b);			\
	test(result, __FILE__, #a" == "#b);		\
    }

#define V2_Value(a)							\
    {									\
	char str[99];							\
	sprintf(str, "V2 ( %s ): %.2f, %.2f", #a, a[0], a[1]);		\
	test(1, __FILE__, str);						\
    }
#define V2_Is_Equal(a, b)						\
    {									\
	i32 result = f32_equal(a[0], (b)[0]) * f32_equal(a[1], (b)[1]);	\
	test(result, __FILE__, #a" == "#b);				\
    }

#define V3_Value(a)							\
    {									\
	char str[99];							\
	sprintf(str, "V3 ( %s ): %.2f, %.2f, %.2f", #a, a[0], a[1], a[2]); \
	test(1, __FILE__, str);						\
    }
#define V3_Is_Equal(a, b)						\
    {									\
	i32 result = f32_equal(a[0], (b)[0]) * f32_equal(a[1], (b)[1]) * f32_equal(a[2], (b)[2]); \
	test(result, __FILE__, #a" == "#b);				\
    }

#define V4_Value(a)							\
    {									\
	char str[99];							\
	sprintf(str, "V4 ( %s ): %.2f, %.2f, %.2f, %.2f", #a, a[0], a[1], a[2], a[3]); \
	test(1, __FILE__, str);						\
    }
#define V4_Is_Equal(a, b)						\
    {									\
	i32 result = f32_equal(a[0], (b)[0]) * f32_equal(a[1], (b)[1]) * f32_equal(a[2], (b)[2]) * f32_equal(a[3], (b)[3]); \
	test(result, __FILE__, #a" == "#b);				\
    }

#define M4_Value(m)							\
    {									\
	char sb[511];							\
	sprintf(sb, "M4 ( %s ):\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f", #m, m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]); \
	String_Value(sb);						\
    }

#define M4_Equal(m1, m2)						\
    {									\
	i32 isEqual = 1;						\
	const char** list = NULL;					\
	for (i32 r = 0; r < 4; r++)					\
	    for (i32 c = 0; c < 4; c++)					\
	    {								\
		isEqual *= f32_equal(m1[r][c], m2[r][c]);		\
	    }								\
	test(isEqual, __FILE__, #m1" == "#m2);				\
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
