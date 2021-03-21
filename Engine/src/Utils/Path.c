#include "Path.h"

#include "String.h"

char g_CurrentDirectory[4096] = "\0";

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
path_get_prev_directory(char* currentDirectory)
{
    if (!vstring_compare(currentDirectory, "/"))
    {
	i32 index = vstring_last_index_of(currentDirectory, '/');
	if (index != 0)
	{
	    index--;
	}
	char* prevDirectoryPath = vstring_substring_range(currentDirectory, 0, index);
	char* iprevDirectoryPath = istring(prevDirectoryPath);
	memory_free(prevDirectoryPath);
	return iprevDirectoryPath;
    }

    return NULL;
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

    return istring(path);
}

char*
path_get_filename_interning(const char* path)
{
    i32 index = vstring_last_index_of(path, '/');
    assert(index != -1);
    char* iPath = istring(path + index + 1);
    return iPath;
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

force_inline char**
internal_directory_get(char* directory, u8 elemCode)
{
    char** elements = NULL;

    struct dirent **namelist;
    i32 n;
    n = scandir(directory, &namelist, 0, string_comparer);
    while (n--)
    {
	char* absolutePath = path_combine(directory, namelist[n]->d_name);
	char* iAbsolutePath = istring(absolutePath);
	char* iPath = istring(namelist[n]->d_name);
	//NOTE(bies): path doesn't work
	if (path(iAbsolutePath) == elemCode)
	{
	    array_push(elements, iPath);
	}

	memory_free(absolutePath);
	free(namelist[n]);
    }

    free(namelist);

    return elements;
}

char**
directory_get_files(char* directory)
{
    char** files = internal_directory_get(directory, PATH_IS_FILE);
    return files;
}

char**
directory_get_directories(char* directory)
{
    char** dirs = internal_directory_get(directory, PATH_IS_DIRECTORY);
    return dirs;
}
