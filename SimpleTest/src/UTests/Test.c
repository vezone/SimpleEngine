#include "Test.h"
#include "Utils/stb_ds.h"
#include "Utils/Logger.h"
#include "Utils/Array.h"
#include <assert.h>

static i8 g_IsInitialized = 0;
static TestTable g_TestTable;
static char* g_CurrentFunction;
static TestInfo g_TestInfo;

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
	if (vstring_compare(fileInfo->Functions[i], functionName))
	{
	    return results[i];
	}
    }

    return NULL;
}

void
file_info_add_function_result(FileInfo* fileInfo, char* functionName, i32 code, const char* message)
{
    FunctionResult* functionResult = file_info_get_function_result(fileInfo, functionName);
    if (functionResult)
    {
	if (!code)
	{
	    functionResult->IsSuccess = code;
	}

	array_push(functionResult->Codes, code);
	vstring_builder_append_format(&functionResult->Builder, "%s [Result: %d]\n", message, code);
    }
    else
    {
	functionResult = malloc(sizeof(FunctionResult));
	functionResult->Codes = NULL;
	vstring_builder_init(&functionResult->Builder);
	vstring_builder_append_format(&functionResult->Builder, "%s [Result: %d]\n", message, code);
	array_push(functionResult->Codes, code);
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
}

FileInfo*
test_table_get_file_info(TestTable* testTable, char* filename)
{
    i32 i;
    i32 filesCount = array_len(testTable->Filenames);

    for (i = 0; i < filesCount; i++)
    {
	if (vstring_compare(testTable->Filenames[i], filename))
	{
	    return testTable->Infos[i];
	}
    }

    return NULL;
}

void
test_table_add_file_info(TestTable* testTable, char* filename, i8 code, const char* message)
{
    FileInfo* fileInfo = test_table_get_file_info(testTable, filename);
    if (fileInfo != NULL)
    {
	file_info_add_function_result(fileInfo, g_CurrentFunction, code, message);
    }
    else
    {
	fileInfo = malloc(sizeof(FileInfo));
	fileInfo->Functions = NULL;
	fileInfo->Results = NULL;
	file_info_add_function_result(fileInfo, g_CurrentFunction, code, message);
	array_push(testTable->Filenames, filename);
	array_push(testTable->Infos, fileInfo);
    }
}

void test_set_function(const char* function)
{
    g_CurrentFunction = istring(function);
}

void
test(i8 code, const char* filename, const char* message)
{
    vassert(g_CurrentFunction);
    vassert(filename);
    vassert(message);

    // GWARNING("Result: %d, Filename: %s, Message: %s, FunctionName: %s\n", code, filename, message, g_CurrentFunction);

    char* ifilename = path_get_filename_interning(filename);
    vassert(ifilename);

    if (!g_IsInitialized)
    {
	g_IsInitialized = 1;
	test_table_initialize(&g_TestTable);

	g_TestInfo.Count = 0;
	g_TestInfo.SuccessCount = 0;
	g_TestInfo.ErrorsCount = 0;
    }

    test_table_add_file_info(&g_TestTable, ifilename, code, message);
}

FileInfo*
file_get_info(char* filename)
{
    vassert(filename);

    FileInfo* value = test_table_get_file_info(&g_TestTable, filename);
    return value;
}

char**
test_get_filenames()
{
    return g_TestTable.Filenames;
}

TestInfo
test_get_info()
{
    return g_TestInfo;
}
