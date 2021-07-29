#include "Path.h"

#if WINDOWS_PLATFORM
#include <shlwapi.h>
#endif

#include "String.h"

static char g_CurrentDirectory[4096] = "\0";

i32
path_is_file_exist(const char* path)
{
#if LINUX_PLATFORM
    struct stat buf;
    i32 result = stat(path, &buf);
    if (result == -1)
	return 0;
    else
	return 1;
#elif WINDOW_PLATFORM
    BOOL isExist = PathFileExistsA(path);
    return (i32) isExist;
#endif
}

i32
path_is_directory_exist(const char* path)
{
    return 0;
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
#ifdef LINUX_PLATFORM
    if (g_CurrentDirectory[0] == '\0')
    {
	getcwd(g_CurrentDirectory, 4096);
    }
    return g_CurrentDirectory;
#elif WINDOWS_PLATFORM
    GetCurrentDirectoryA(4096, g_CurrentDirectory);
    return g_CurrentDirectory;
#endif
}

char*
path_get_prev_directory(const char* currentDirectory)
{
    if (!vstring_compare(currentDirectory, ROOT_DIRECTORY))
    {
#ifdef LINUX_PLATFORM
	i32 index = vstring_last_index_of(currentDirectory, '/');
#elif WINDOWS_PLATFORM
    i32 index = vstring_last_index_of_string(currentDirectory, PATH_SEPARATOR);
#endif
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

#if LINUX_PLATFORM
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
#elif WINDOWS_PLATFORM
const char**
directory_get_files(const char* directory)
{
    WIN32_FIND_DATA foundFileData;
    HANDLE hFind;
    const char** files = NULL;

    hFind = FindFirstFileA(directory, &foundFileData);
    do
    {
	if (foundFileData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
	{
	    array_push(files, foundFileData.cFileName);
	}
    } while (FindNextFile(hFind, &foundFileData) != 0);

    return files;
}

const char**
directory_get_directories(const char* directory)
{
    WIN32_FIND_DATA foundFileData;
    HANDLE hFind;
    const char** dirs = NULL;

    hFind = FindFirstFileA(directory, &foundFileData);
    do
    {
	if (foundFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
	    array_push(dirs, foundFileData.cFileName);
	}
    } while (FindNextFile(hFind, &foundFileData) != 0);

    return dirs;
}

const char**
directory_get_files_absolute(const char* directory)
{
    const char** files = directory_get_files(directory);
    return files;
}

const char**
directory_get_directories_absolute(const char* directory)
{
    const char** dirs = directory_get_directories(directory);
    return dirs;
}
#endif
