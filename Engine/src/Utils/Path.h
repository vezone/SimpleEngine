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

//should we intern all the stuff here?
const char* path_get_filename(const char* path);
const char* path_get_filename_interning(const char* path);

#ifdef LINUX_PLATFORM

char* path_get_current_directory();
char* path_get_prev_directory(char* currentDirectory);

static struct passwd* g_UserInfo = NULL;

force_inline u8
path(char* path)
{
    struct stat fileInfo;

    if (stat(path, &fileInfo) != 0)
    {
	GERROR("Path incorrect: %s!\n", path);
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

force_inline char*
path_get_extension(char* path)
{
    i32 extensionIndex = vstring_last_index_of(path, '.');
    u32 length = vstring_length(path) - 1;
    char* extension = vstring_substring_range(path, extensionIndex, length);
    return extension;
}

force_inline const char*
path_combine(char* left, char* right)
{
    char* str = vstring_concat3(left, "/", right);
    const char* istr = istring(str);
    memory_free(str);
    return istr;
}

force_inline char*
path_get_absolute(char* path)
{
    char* currentDirectory = path_get_current_directory();
    char* absolutePath = vstring_concat3(currentDirectory, "/", path);
    return absolutePath;
}

char** directory_get_files(char* directory);
char** directory_get_directories(char* directory);

#elif WINDOWS_PLATFORM
#error "Not implimented!"
#endif

#endif
