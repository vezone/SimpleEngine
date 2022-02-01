#include "Test.h"
#include <assert.h>

static i8 g_IsInitialized = 0;
static i8 g_IsProfilingInitialized = 0;

static i32 g_NextNestingLevel = 0;
static i32 g_IsNextSeparated = 0;

static TestTable g_TestTable;
static ProfilingTest* g_ProfilingTest;

const char* g_CurrentFunction;
static TestInfo g_TestInfo;

force_inline void
reset_global_flags()
{
    g_NextNestingLevel = 0;
    g_IsNextSeparated = 0;
}

void
test_table_initialize(TestTable* testTable)
{
    testTable->Filenames = NULL;
    testTable->Infos = NULL;
}

FunctionResult*
file_info_get_function_result(FileInfo* fileInfo, const char* functionName)
{
    vassert(fileInfo != NULL);

    FunctionResult** results = fileInfo->Results;
    i32 functionsCount = array_len(fileInfo->Functions);

    for (i32 i = 0; i < functionsCount; i++)
    {
	if (string_compare(fileInfo->Functions[i], functionName))
	{
	    return results[i];
	}
    }

    return NULL;
}

void
file_info_add_function_result(FileInfo* fileInfo, FunctionResult* functionResult, i32 code, const char* message)
{
    if (functionResult != NULL)
    {
	if (!code)
	{
	    functionResult->IsSuccess = code;
	}
    }
    else
    {
	functionResult = malloc(sizeof(*functionResult));
	functionResult->Codes = NULL;
	functionResult->Results = NULL;
	functionResult->IsSuccess = code;
	if (code)
	{
	    g_TestInfo.SuccessCount++;
	}
	else
	{
	    g_TestInfo.ErrorsCount++;
	}

	array_push(fileInfo->Functions, g_CurrentFunction);
	array_push(fileInfo->Results, functionResult);

	g_TestInfo.Count++;
    }

    FunctionResultData functionResultData;
    functionResultData.NestingLevel = g_NextNestingLevel;
    functionResultData.IsSeparated = g_IsNextSeparated;
    functionResultData.ReturnCode = code;
    functionResultData.Message = message;

    array_push(functionResult->Results, functionResultData);

    // TODO(bies): remove this in the future
    if (code != -2)
    {
	array_push(functionResult->Codes, code);
    }

    reset_global_flags();
}

FileInfo*
test_table_get_file_info(TestTable* testTable, const char* filename)
{
    i32 i;
    i32 filesCount = array_count(testTable->Filenames);

    for (i = 0; i < filesCount; i++)
    {
	if (string_compare(testTable->Filenames[i], filename))
	{
	    return testTable->Infos[i];
	}
    }

    return NULL;
}

void
test_table_add_file_info(TestTable* testTable, const char* filename, i8 code, const char* message)
{
    FileInfo* fileInfo = test_table_get_file_info(testTable, filename);
    if (fileInfo == NULL)
    {
	fileInfo = malloc(sizeof(*fileInfo));
	fileInfo->Functions = NULL;
	fileInfo->Results = NULL;
	array_push(testTable->Filenames, filename);
	array_push(testTable->Infos, fileInfo);
    }

    FunctionResult* functionResult = file_info_get_function_result(fileInfo, g_CurrentFunction);
    file_info_add_function_result(fileInfo, functionResult, code, message);
}

void test_set_function(const char* function)
{
    g_CurrentFunction = function;
}

void
test(i8 code, const char* filename, const char* message)
{
    vassert(message);

    if (!g_IsInitialized)
    {
	g_IsInitialized = 1;
	test_table_initialize(&g_TestTable);

	g_TestInfo.Count = 0;
	g_TestInfo.SuccessCount = 0;
	g_TestInfo.ErrorsCount = 0;
    }

    vassert(g_CurrentFunction);
    vassert(filename);

    char* ifilename = path_get_filename(filename);
    vassert(ifilename);
    vassert(ifilename != NULL);

    test_table_add_file_info(&g_TestTable, ifilename, code, message);
}

void
_test_next_as_child(i32 nestingLevel)
{
    g_NextNestingLevel = nestingLevel;
}

void
_test_next_separated()
{
    g_IsNextSeparated = 1;
}

i32
get_next_nesting_level()
{
    return g_NextNestingLevel;
}

FileInfo*
file_get_info(const char* filename)
{
    vassert(filename);

    FileInfo* value = test_table_get_file_info(&g_TestTable, filename);
    return value;
}

const char**
test_get_filenames()
{
    return g_TestTable.Filenames;
}

TestInfo
test_get_info()
{
    return g_TestInfo;
}
