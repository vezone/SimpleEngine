#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#ifdef LINUX_PLATFORM
#define NEW_LINE "\n"
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define ROOT_DIRECTORY "/"
#elif WINDOWS_PLATFORM
#define NEW_LINE "\r\n"
#define PATH_SEPARATOR "\\"
#define PATH_SEPARATOR_STRING "\\"
#define ROOT_DIRECTORY "C:\\"
#endif

#endif
