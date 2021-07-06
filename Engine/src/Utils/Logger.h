#ifndef LOGGER_H
#define LOGGER_H

#include "Types.h"

//GLBOBAL
#define DEBUG_MODE 0

#define ISGLOBALLOG 1
#define ISGLOBALSUCCESS 1

#if DEBUG_MODE == 0
#define ISGLOBALERROR 1
#define ISGLOBALWARNING 1
#define ISGLOBALDEBUG 1
#else
#define ISGLOBALERROR 0
#define ISGLOBALWARNING 0
#define ISGLOBALDEBUG 0
#endif

#define ISFORMATTOSTRING 1

//Shader
#define ISSHADERDEBUG 0
//Buffers
#define ISBUFFERDEBUG 0
//Textures
#define ISTEXTUREDEBUG 0

#define PLATFORM_LINUX 1
#if PLATFORM_LINUX == 1
#include "Platform/Linux/LinuxLogger.h"
#else
//Create windows logger
#endif

#include "BaseHelper.h"

#define PRINT(format, ...) printf(format, ##__VA_ARGS__)
#define PRINT_SINGLE(format, ...)					\
    {									\
	DO_SINGLE_TIME(printf(format, ##__VA_ARGS__));			\
    }
#define PRINT_MANY(count, format, ...)					\
    {									\
	DO_MANY_TIME(printf(format, ##__VA_ARGS__), count);		\
    }

#if ISGLOBALLOG == 1
#define GLOG(format, ...) PLOG(format, ##__VA_ARGS__)
#else
#define GLOG(format, ...)
#endif

#if ISGLOBALSUCCESS == 1
#define GSUCCESS(format, ...) PSUCCESS(format, ##__VA_ARGS__)
#else
#define GSUCCESS(format, ...)
#endif

#if ISGLOBALERROR == 1
#define GERROR(format, ...) PERROR(format, ##__VA_ARGS__)
#else
#define GERROR(format, ...)
#endif

#if ISGLOBALWARNING == 1
#define GWARNING(format, ...) PWARNING(format, ##__VA_ARGS__)
#else
#define GWARNING(format, ...)
#endif

#if ISGLOBALDEBUG == 1
#define GDEBUG(format, ...) PDEBUG(format, ##__VA_ARGS__)
#define GLDEBUG(format, file, line, ...) PGLDEBUG(format, file, line, ##__VA_ARGS__)
#else
#define GDEBUG(format, ...)
#define GLDEBUG(format, file, line, ...)
#endif

#define GPRINTI32(var) GLOG(#var": %d\n", var)
#define GLOGS(x) { int loggerLock123 = 1; if (loggerLock123){ PLOG(x); } }

#if ISFORMATTOSTRING == 1
#define GFORMAT(string, format, ...) sprintf(string, format, __VA_ARGS__)
#else
#define GFORMAT(string, format, ...)
#endif


#if ISSHADERDEBUG == 1
#define SHADERDEBUG(format, ...) GLOG(format, ##__VA_ARGS__)
#else
#define SHADERDEBUG(format, ...)
#endif

#if ISBUFFERDEBUG == 1
#define BUFFERDEBUG(format, ...) GLOG(format, ##__VA_ARGS__)
#else
#define BUFFERDEBUG(format, ...)
#endif


#if ISTEXTUREDEBUG == 1
#define TEXTUREDEBUG(format, ...) GLOG(format, ##__VA_ARGS__)
#else
#define TEXTUREDEBUG(format, ...)
#endif

#endif
