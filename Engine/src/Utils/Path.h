#ifndef PATH_H
#define PATH_H

#include "Types.h"
#include "String.h"
#include "MemoryAllocator.h"

enum Path
{
    PATH_IS_SOMETHING = 0,
    PATH_IS_FILE,
    PATH_IS_DIRECTORY
};

#ifdef LINUX_PLATFORM

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "String.h"
#include "Logger.h"
#include "Array.h"

#elif WINDOWS_PLATFORM

#error "Not implimented!"

#else

#error "Platform unsupported"

#endif

i32 path_is_file_exist(const char* path);
i32 path_is_directory_exist(const char* path);
//should we intern all the stuff here?
char* path_get_filename(const char* path);
const char* path_get_filename_interning(const char* path);

#ifdef LINUX_PLATFORM

char* path_get_current_directory();
char* path_get_prev_directory(const char* currentDirectory);
const char* path_get_prev_directory_interning(const char* currentDirectory);

static struct passwd* g_UserInfo = NULL;

force_inline u8
path(const char* path)
{
    struct stat fileInfo;

    if (stat(path, &fileInfo) != 0)
    {
	return PATH_IS_SOMETHING;
    }

    if (S_ISDIR(fileInfo.st_mode))
    {
	return PATH_IS_DIRECTORY;
    }
    else if (S_ISREG(fileInfo.st_mode))
    {
	return PATH_IS_FILE;
    }

    return PATH_IS_SOMETHING;
}

force_inline char*
path_get_home_directory()
{
    if (!g_UserInfo)
    {
	g_UserInfo = getpwuid(geteuid());
    }

    return g_UserInfo->pw_dir;
}

force_inline const char*
path_get_extension(const char* path)
{
    i32 extensionIndex = vstring_last_index_of(path, '.');
    return (const char*) (path + extensionIndex * sizeof(char));
}

force_inline const char*
path_get_name(const char* path)
{
    i32 extensionIndex = vstring_last_index_of(path, '/');
    return (const char*) (path + (extensionIndex + 1) * sizeof(char));
}

force_inline char*
path_combine(const char* left, const char* right)
{
    char* str = vstring_concat3(left, "/", right);
    return str;
}

force_inline const char*
path_combine_interning(const char* left, const char* right)
{
    char* path = path_combine(left, right);
    const char* iPath = istring(path);
    memory_free(path);
    return iPath;
}

force_inline char*
path_get_absolute(char* path)
{
    char* currentDirectory = path_get_current_directory();
    char* absolutePath = vstring_concat3(currentDirectory, "/", path);
    return absolutePath;
}

const char** directory_get_files(const char* directory);
const char** directory_get_files_absolute(const char* directory);
const char** directory_get_directories(const char* directory);

#elif WINDOWS_PLATFORM
#error "Not implimented!"
#endif

#endif
