#include "UTest_Path.h"

#include "UTests/Test.h"
#include "Utils/Path.h"

static void
path_get_current_directory_test()
{
    String_Value(path_get_current_directory());
}

static void
path_get_home_directory_test()
{
    String_IsEquals(path_get_home_directory(), "/home/bies");
    String_Value(path_get_home_directory());
}

static void
path_get_extension_test()
{
    String_IsEquals(path_get_extension("/path/to/file.json"), ".json");
    String_IsEquals(path_get_extension("file.lua"), ".lua");
    String_IsEquals(path_get_extension("/path/file."), ".");
}

static void
path_get_prev_directory_test()
{
    String_IsEquals(path_get_prev_directory( "/path/to/current/directory"), "/path/to/current");

    char* prev = path_get_prev_directory("/path/to/current/directory");
    String_Value(prev);

    prev = path_get_prev_directory(prev);
    String_Value(prev);

    prev = path_get_prev_directory(prev);
    String_Value(prev);
    prev = path_get_prev_directory(prev);
    String_Value(prev);

    prev = path_get_prev_directory(prev);
    Condition(prev == NULL);
}

static void
path_combine_test()
{
    String_IsEquals(path_combine("path/to", "file"), "path/to/file");
}

static void
path_absolute_test()
{
    char* currentDirectory = path_get_current_directory();
    String_IsEquals(path_get_absolute("file"), path_combine(currentDirectory, "file"));

    String_Value(path_get_absolute("file"));
}

static void
directory_get_files_test()
{
    char** files;

    //files = directory_get_files(path_get_current_directory());
    //Condition(array_len(files) > 0);
    //String_List_Value(files, '\n');
    //array_free(files);

    char* path = ".";
    files = directory_get_files(path);
    Condition(array_len(files) > 0);
    String_List_Value(files, '\n');
    array_free(files);
}

static void
directory_get_directories_test()
{
    char** dirs;
    //dirs = directory_get_directories(path_get_current_directory());
    //Condition(array_len(dirs) > 0);
    //String_List_Value(dirs, '\n');
    //array_free(dirs);

    // . - current directory .. - prev directory
    // "./CTPL/"
    // "../CTPL/"
    char* path = "/home/bies/Data/programming/C";
    dirs =  directory_get_directories(path);
    Condition(array_len(dirs) > 0);
    Int_Value(array_len(dirs));
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
    TEST(path_absolute_test());

    TEST(directory_get_files_test());
    TEST(directory_get_directories_test());
}
