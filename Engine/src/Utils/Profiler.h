#ifndef PROFILER_H
#define PROFILER_H

#ifdef LINUX_PLATFORM
#include <time.h>
#elif WINDOWS_PLATFORM
#include <profileapi.h>
#endif

#include "Types.h"

#define PROFILER_NS_TO_S(ns)   (ns / (1000 * 1000 * 1000))
#define PROFILER_NS_TO_MS(ns)  (ns / (1000 * 1000))
#define PROFILER_NS_TO_MCS(ns) (ns / (1000))

typedef enum ProfilerTimeType
{
    PROFILER_TIME_NS = 0,
    PROFILER_TIME_MCS,
    PROFILER_TIME_MS,
    PROFILER_TIME_S,
} ProfilerTimeType;

static i64 g_DeltaInNanoseconds;

#ifdef LINUX_PLATFORM
static struct timespec g_StartTime;
static struct timespec g_EndTime;
#elif WINDOWS_PLATFORM
// NOTE: check if delta in nanoseconds
static i64 g_StartTime;
static i64 g_EndTime;
#endif

force_inline void
profiler_start()
{
#ifdef LINUX_PLATFORM
    clock_gettime(CLOCK_REALTIME, &g_StartTime);
#elif WINDOWS_PLATFORM
    QueryPerformanceCounter(&g_StartTime);
#endif
}

force_inline void
profiler_end()
{
#ifdef LINUX_PLATFORM
    clock_gettime(CLOCK_REALTIME, &g_EndTime);
    g_DeltaInNanoseconds = (1000 * 1000 * 1000 * (g_EndTime.tv_sec - g_StartTime.tv_sec)) + (g_EndTime.tv_nsec - g_StartTime.tv_nsec);
#elif WINDOWS_PLATFORM
    QueryPerformanceCounter(&g_EndTime);
    g_DeltaInNanoseconds = g_EndTime - g_StartTime;
#endif

}

force_inline ProfilerTimeType
profiler_get_time_type()
{
    if (PROFILER_NS_TO_S(g_DeltaInNanoseconds))
    {
	return PROFILER_TIME_S;
    }
    else if (PROFILER_NS_TO_MS(g_DeltaInNanoseconds))
    {
	return PROFILER_TIME_MS;
    }
    else if (PROFILER_NS_TO_MCS(g_DeltaInNanoseconds))
    {
	return PROFILER_TIME_MCS;
    }
    else if (g_DeltaInNanoseconds)
    {
	return PROFILER_TIME_NS;
    }

    vassert(0 && "");
    return PROFILER_TIME_NS;
}

force_inline i64
profiler_get_nanoseconds()
{
    return g_DeltaInNanoseconds;
}

force_inline i64
profiler_get_microseconds()
{
    return g_DeltaInNanoseconds / 1000;
}

force_inline i64
profiler_get_milliseconds()
{
    return g_DeltaInNanoseconds / (1000 * 1000);
}

force_inline i64
profiler_get_seconds()
{
    return g_DeltaInNanoseconds / (1000 * 1000 * 1000);
}

force_inline f64
profiler_get_microseconds_as_float()
{
    return ((f64)g_DeltaInNanoseconds) / 1000;
}

force_inline f64
profiler_get_milliseconds_as_float()
{
    return ((f64)g_DeltaInNanoseconds) / (1000 * 1000);
}

force_inline f64
profiler_get_seconds_as_float()
{
    return ((f64)g_DeltaInNanoseconds) / (1000 * 1000 * 1000);
}

force_inline void
profiler_print()
{
    ProfilerTimeType timeType = profiler_get_time_type();

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%ld %s\n", profiler_get_microseconds(), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%ld %s\n", profiler_get_milliseconds(), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%ld %s\n", profiler_get_seconds(), "s");
	break;
    default:
	vassert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

force_inline void
profiler_print_as_float()
{
    ProfilerTimeType timeType = profiler_get_time_type();

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%.4f %s\n", profiler_get_microseconds_as_float(), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%.4f %s\n", profiler_get_milliseconds_as_float(), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%.4f %s\n", profiler_get_seconds_as_float(), "s");
	break;
    default:
	vassert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

static char g_TimeString[512];

force_inline char*
profiler_get_string()
{
    ProfilerTimeType timeType = profiler_get_time_type();

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	vstring_format(g_TimeString, "%ld %s", profiler_get_nanoseconds(), "ns");
	break;
    case PROFILER_TIME_MCS:
	vstring_format(g_TimeString, "%ld %s", profiler_get_microseconds(), "mcs");
	break;
    case PROFILER_TIME_MS:
	vstring_format(g_TimeString, "%ld %s", profiler_get_milliseconds(), "ms");
	break;
    case PROFILER_TIME_S:
	vstring_format(g_TimeString, "%ld %s", profiler_get_seconds(), "s");
	break;
    default:
	vassert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }

    return (char*)g_TimeString;
}

force_inline char*
profiler_get_string_as_float()
{
    ProfilerTimeType timeType = profiler_get_time_type();

    switch (timeType)
    {
    case PROFILER_TIME_NS:
    {
	f64 temp = profiler_get_nanoseconds();
	vstring_format(g_TimeString, "%f %s\n", temp, "ns");
	break;
    }
    case PROFILER_TIME_MCS:
    {
	f64 temp = profiler_get_microseconds_as_float();
	vstring_format(g_TimeString, "%f %s\n", temp, "mcs");
	break;
    }
    case PROFILER_TIME_MS:
    {
	f64 temp = profiler_get_milliseconds_as_float();
	vstring_format(g_TimeString, "%f %s\n", temp, "ms");
	break;
    }
    case PROFILER_TIME_S:
    {
	f64 temp = profiler_get_seconds_as_float();
	vstring_format(g_TimeString, "%f %s\n", temp, "s");
	break;
    }
    default:
    {
	vassert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
    }

    return (char*)g_TimeString;
}

#endif
