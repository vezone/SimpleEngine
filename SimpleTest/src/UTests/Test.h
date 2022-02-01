#ifndef TEST_H
#define TEST_H

#include <Utils/SimpleStandardLibrary.h>
#include <Math/Math.h>

typedef struct FunctionResultData
{
    /* flag for ui */
    i32 NestingLevel;
    i32 IsSeparated;
    i32 ReturnCode;
    const char* Message;
} FunctionResultData;

typedef struct FunctionResult
{
    //NOTE(bies): Массив результирующих кодов
    i8 IsSuccess;
    i8* Codes;
    FunctionResultData* Results;
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

typedef struct ProfilingFunctionResult
{
    // function naem
    const char* Key;
    // time
    const char* Value;
} ProfilingFunctionResult;

typedef struct ProfilingFileInfo
{
    // file name
    const char* Key;
    // profiling time
    ProfilingFunctionResult* Value;
} ProfilingFileInfo;

typedef struct ProfilingTest
{
    ProfilingFileInfo* FileInfoTable;
} ProfilingTest;

// TODO(bies): change _is_equal to equal

FunctionResult* file_info_get_function_result(FileInfo* fileInfo, const char* functionName);
FileInfo* test_table_get_file_info(TestTable* testTable, const char* filename);
void test_set_function(const char* function);
void test(i8 testResult, const char* filename, const char* message);

void profiling_test_init(ProfilingTest* profilingTest);
ProfilingTest* profiling_test_get();
void profiling_test(ProfilingTest* profilingTest, const char* file, const char* profilingResult);

void _test_next_as_child();
void _test_next_separated();
i32 get_next_nesting_level();

FileInfo* file_get_info(const char* filename);
const char** test_get_filenames();

#define Condition(condition) { i8 temp = (condition); test(temp, __FILE__, #condition); }

// USAGE: String_IsEqual(string_copy("It's a string"), "It's a string")
#define String_Equal(a, b) { i8 temp = string_compare((a), (b)); test(temp, __FILE__, #a" == "#b); }
#define String_NotEqual(a, b) { i8 temp = string_compare((a), (b)); test(!temp, __FILE__, #a" != "#b); }
#define String_Value(str)						\
    ({									\
	test((str ? 1 : 0), __FILE__, (str ? string_concat(#str": ", str) : "NULL")); \
    })
#define String_List_Value(list, joinCharacter) test((array_count(list) ? 1 : 0), __FILE__, (list ? string_join(list, joinCharacter) : "EMPTY"))

#define Int_Value(val)					\
    {							\
	char str[50];					\
	string_set(str, '\0', 50);			\
	memset(str, '\0', 50);				\
	string_i32(str, (val));				\
	char* res = string_concat(#val": ", str);	\
	test(1, __FILE__, res);				\
    }

#define I32_As_String(val)			\
    ({						\
	char str[32];				\
	memset(str, '\0', 32);			\
	string_i32(str, (val));			\
	string(str);				\
    })

#define I32_Value(val)					\
    {							\
	const char* str = I32_As_String(val);		\
	char* res = string_concat(#val": ", str);	\
	test(1, __FILE__, res);				\
    }

#define F32_Value(val)					\
    {							\
	char str[32];					\
	sprintf(str, "%f", val);			\
	char* res = string_concat(#val": ", str);	\
	test(1, __FILE__, res);				\
    }
#define F32_Is_Equal(a, b)				\
    {							\
	i32 result = f32_equal(a, b);			\
	test(result, __FILE__, #a" == "#b);		\
    }
#define F32_Equal(a, b)					\
    {							\
	i32 result = f32_equal(a, b);			\
	test(result, __FILE__, #a" == "#b);		\
    }
#define F32_Equal_Epsilon(a, b, eps)			\
    {							\
	i32 result = f32_equal_epsilon(a, b, eps);	\
	test(result, __FILE__, #a" == "#b);		\
    }

#define V2_Value(a)							\
    {									\
	char str[99];							\
	sprintf(str, "V2 ( %s ): %.2f, %.2f", #a, a[0], a[1]);		\
	test(1, __FILE__, string(str));				\
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
	test(1, __FILE__, string(str));				\
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
	test(1, __FILE__, string(str));				\
    }
#define V4_Is_Equal(a, b)						\
    {									\
	i32 result = f32_equal(a[0], (b)[0]) * f32_equal(a[1], (b)[1]) * f32_equal(a[2], (b)[2]) * f32_equal(a[3], (b)[3]); \
	test(result, __FILE__, #a" == "#b);				\
    }
#define V4_Equal(a, b)				\
    V4_Is_Equal(a, b)

static char* IND[4] = {
    "",
    "    ",
    "        ",
    "            "
};

#define M4_Value(m)							\
    {									\
	char* ind = IND[get_next_nesting_level()];			\
	char sb[511];							\
	sprintf(sb, "M4 ( %s ):\n%s%.2f %.2f %.2f %.2f\n%s%.2f %.2f %.2f %.2f\n%s%.2f %.2f %.2f %.2f\n%s%.2f %.2f %.2f %.2f", #m, \
		ind, m[0][0], m[0][1], m[0][2], m[0][3],		\
		ind, m[1][0], m[1][1], m[1][2], m[1][3],		\
		ind, m[2][0], m[2][1], m[2][2], m[2][3],		\
		ind, m[3][0], m[3][1], m[3][2], m[3][3]);		\
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

#define JsonToken_Value(token)						\
    {									\
	char* msg;							\
	const char* tokenToString = json_token_to_string(token);	\
									\
	if (token.Type != JSON_TOKEN_TYPE_FLOAT_NUMBER			\
	    && token.Type != JSON_TOKEN_TYPE_INT_NUMBER)		\
	{								\
	    msg = string_concat_with_space_between(tokenToString, token.Data, 50, \
						    ' ');		\
	}								\
	else								\
	{								\
	    const char* data = I32_As_String(void_ptr_to_i32(token.Data)); \
	    msg = string_concat_with_space_between(tokenToString, data, 50, ' '); \
	}								\
	GERROR("%s\n", msg);						\
	test(-2, __FILE__, msg);					\
    }

#define Texture_PtrValue(texture)		\
    {						\
	Condition(texture != NULL);		\
	I32_Value(texture->ID);			\
	I32_Value(texture->Width);		\
	I32_Value(texture->Height);		\
	I32_Value(texture->Channels);		\
	I32_Value(texture->Slot);		\
	I32_Value(texture->DataFormat);		\
	String_Value(texture->Path);		\
    }

#define TEST(function)					\
    {							\
	test_set_function(#function);			\
	function;					\
	test(1, __FILE__, "");				\
    }

#define TEST_NEXT_AS_CHILD()			\
    _test_next_as_child(1)
#define TEST_NEXT_AS_CHILD_N(n)			\
    _test_next_as_child(n)
#define TEST_NEXT_SEPARATED()\
    _test_next_separated()

#define PROFILING_TEST(function)					\
    {									\
	test_set_function(#function);					\
	profiler_start();						\
	function;							\
	profiler_end();							\
	profiling_test(profiling_test_get(), __FILE__, profiler_get_string_as_float()); \
    }

typedef struct TestInfo
{
    u32 Count;
    u32 ErrorsCount;
    u32 SuccessCount;
} TestInfo;

TestInfo test_get_info();

#endif
