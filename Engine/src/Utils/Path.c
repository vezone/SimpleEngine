#include "Path.h"

#include "String.h"

static char g_CurrentDirectory[4096] = "\0";

i32
path_is_file_exist(const char* path)
{
    struct stat buf;
    i32 result = stat(path, &buf);
    if (result == -1)
	return 0;
    else
	return 1;
}

i32
path_is_directory_exist(const char* path)
{

}

char*
path_get_filename(const char* path)
{
    i32 index = vstring_last_index_of(path, '/');
    if (index)
    {
	char* result = vstring_substring(path, (index + 1));
	return result;
    }

    return NULL;
}

const char*
path_get_filename_interning(const char* path)
{
    //index can be -1
    i32 index = vstring_last_index_of(path, '/');
    const char* iPath = istring(path + index + 1);
    return iPath;
}

char*
path_get_current_directory()
{
    if (g_CurrentDirectory[0] == '\0')
    {
	getcwd(g_CurrentDirectory, 4096);
    }

    return g_CurrentDirectory;
}

char*
path_get_prev_directory(const char* currentDirectory)
{
    if (!vstring_compare(currentDirectory, "/"))
    {
	i32 index = vstring_last_index_of(currentDirectory, '/');
	if (index != 0)
	{
	    --index;
	}

	char* prevDirectoryPath = vstring_substring_range(currentDirectory, 0, index);
	return prevDirectoryPath;
    }

    return NULL;
}

const char*
path_get_prev_directory_interning(const char* currentDirectory)
{
    char* prevDirectory = path_get_prev_directory(currentDirectory);
    const char* iPrevDirectory = istring(prevDirectory);
    memory_free(prevDirectory);
    return iPrevDirectory;
}

i32
string_comparer(const struct dirent **a, const struct dirent **b)
{
    char* left  = (char*) (*a)->d_name;
    char* right = (char*) (*b)->d_name;
    u32 leftLength = vstring_length(left);
    u32 rightLength = vstring_length(right);

    for (u32 i = 0; i < leftLength; i++)
    {
	char l = vchar_to_lower(left[i]);
	char r = vchar_to_lower(right[i]);

	if (l < r)
	{
	    return 1;
	}
	else if (l > r)
	{
	    return -1;
	}
    }

    return (rightLength - leftLength);
}

force_inline const char**
_directory_get(const char* directory, i32 elemCode)
{
    const char** elements = NULL;
    struct dirent** namelist = NULL;
    i32 n = scandir(directory, &namelist, 0, string_comparer);

    while (n > 0)
    {
	const char* dName = namelist[n - 1]->d_name;
	vassert(dName);

	const char* iAbsolutePath = path_combine_interning(directory, dName);
	const char* iPath = istring(namelist[n - 1]->d_name);

	if (path(iAbsolutePath) == elemCode)
	{
	    array_push(elements, iPath);
	}

	free(namelist[n - 1]);

	--n;
    }

    free(namelist);

    return elements;
}

const char**
_directory_get_absolute(const char* directory, i32 elemCode)
{
    const char** elements = NULL;
    struct dirent** namelist = NULL;
    i32 n = scandir(directory, &namelist, 0, string_comparer);

    while (n > 0)
    {
	const char* dName = namelist[n - 1]->d_name;
	vassert(dName);

	const char* iAbsolutePath = path_combine_interning(directory, dName);

	if (path(iAbsolutePath) == elemCode)
	{
	    array_push(elements, iAbsolutePath);
	}

	free(namelist[n - 1]);

	--n;
    }

    free(namelist);

    return elements;
}


const char**
directory_get_files(const char* directory)
{
    const char** files = _directory_get(directory, PATH_IS_FILE);
    return files;
}

const char**
directory_get_directories(const char* directory)
{
    const char** dirs = _directory_get(directory, PATH_IS_DIRECTORY);
    return dirs;
}

const char**
directory_get_files_absolute(const char* directory)
{
    const char** files = _directory_get_absolute(directory, PATH_IS_FILE);
    return files;
}

const char**
directory_get_directories_absolute(const char* directory)
{
    const char** dirs = _directory_get_absolute(directory, PATH_IS_DIRECTORY);
    return dirs;
}
