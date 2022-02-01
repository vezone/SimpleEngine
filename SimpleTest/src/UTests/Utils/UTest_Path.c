#include "UTest_Path.h"

#include "UTests/Test.h"
#include <Utils/SimpleStandardLibrary.h>

static void
path_get_current_directory_test()
{
    String_Value(path_get_current_directory());
}

static void
path_get_home_directory_test()
{
    String_Equal(path_get_home_directory(), "/home/bies");
    String_Value(path_get_home_directory());
}

static void
path_get_extension_test()
{
    String_Equal(path_get_extension("/path/to/file.json"), ".json");
    String_Equal(path_get_extension("file.lua"), ".lua");
    String_Equal(path_get_extension("/path/file."), ".");
}

static void
path_get_prev_directory_test()
{
    const char* prev = (const char*) path_get_prev_directory("/path/to/current/directory");
    String_Equal(prev, "/path/to/current");

    prev = (const char*) path_get_prev_directory("/path/to/current/directory");
    String_Value(prev);
    prev = (const char*) path_get_prev_directory(prev);
    String_Value(prev);
    prev = (const char*) path_get_prev_directory(prev);
    String_Value(prev);
    prev = (const char*) path_get_prev_directory(prev);
    String_Value(prev);
    prev = (const char*) path_get_prev_directory(prev);
    Condition(prev == NULL);
}

static void
path_combine_test()
{
    String_Equal((const char*) path_combine_interning("path/to", "file"), "path/to/file");
}

#define PROFILE(codeToProfile)				\
    {							\
	TimeState state;				\
	profiler_start(&state);				\
	codeToProfile					\
	profiler_end(&state);				\
	i64 mcs = profiler_get_microseconds(&state);	\
	String_Value(#codeToProfile);			\
	I32_Value(mcs);					\
    }

void
directory_get_files_test()
{
    char* path = path_get_current_directory();
    const char** files = NULL;

    PROFILE(
	files = directory_get_files(path);
	);

    Condition(array_count(files) > 0);
    Int_Value(array_count(files));
    String_List_Value(files, '\n');

    array_free(files);
}

void
directory_get_directories_test()
{
    char* path = path_get_current_directory();
    const char** dirs = NULL;

    PROFILE(
	dirs = directory_get_directories(path);
	);

    Condition(array_count(dirs) > 0);
    Int_Value(array_count(dirs));
    String_List_Value(dirs, '\n');

    array_free(dirs);
}

void
path_test()
{
    TEST(path_get_current_directory_test());
    TEST(path_get_home_directory_test());
    TEST(path_get_extension_test());
    TEST(path_get_prev_directory_test());
    TEST(path_combine_test());

    TEST(directory_get_files_test());
    TEST(directory_get_directories_test());
}
