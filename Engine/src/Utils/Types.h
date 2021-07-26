#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
// for size_t
#include <stddef.h>
#include <assert.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define i8(x)  ((i8)(x))
#define i16(x) ((i16)(x))
#define i32(x) ((i32)(x))
#define i64(x) ((i64)(x))
#define u8(x)  ((u8)(x))
#define u16(x) ((u16)(x))
#define u32(x) ((u32)(x))
#define u64(x) ((u64)(x))
#define f32(x) ((f32)(x))
#define f64(x) ((f64)(x))

#define TYPE_REVERSE(x) (x = !(x))

#define I8_MAX 127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775807

#define U8_MAX 255
#define U16_MAX 65535
#define U32_MAX 4294967295
#define U64_MAX 18446744073709551615

#define OffsetOf(Type, Field) ( (u64) (&(((Type*)0)->Field)) )
#define AlignmentOf(Type) ((u64)&(((struct{char c; Type i;}*)0)->i))
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define TO_STRING(x) #x
#define FMOD(x, v) (f32) (((i32)(x)) % v + ((x) - (i32)(x)))
#define KB(x) ((i64)1024 * (i64)x)
#define MB(x) ((i64)1024 * KB(x))
#define GB(x) ((i64)1024 * MB(x))
#define nullptr ((void*) 0)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define TOKB(x) (((f64) x) / 1024)
#define TOMB(x) (((f64) TOKB(x)) / 1024)
#define TOGB(x) (((f64) TOMB(x)) / 1024)

#define ABS(x) ((x > 0) ? x : -x)

#define DEFAULT(type) ((type) { })

#ifndef NULL
#define NULL ((void*)0)
#endif

#if LINUX_PLATFORM
#define force_inline static inline __attribute((always_inline))
#elif WINDOWS_PLATFORM
#define force_inline static
#endif

#define PRINT_ONES(func, msg) { static i8 flag = 1; if (flag) { flag = 0; func(msg); } }
#define PRINT_ONESF(func, format, ...) { static i8 flag = 1; if (flag) { flag = 0; func(format, ##__VA_ARGS__); } }
#define PRINT_ONES3(func, msg, out) { static i8 flag = 1; if (flag) { flag = 0; func(msg, out); } }

#define foreach(type, el, list)					\
    static i32 type##file##index_temp = 0;			\
    for (type* el = list[type##file##index_temp]; type##file##index_temp < array_len(list); type##file##index_temp++)

#define vassert(condition) assert(condition)

#define vassert_message(message, condition) \
    {					    \
	GERROR(message);		    \
	assert(condition);		    \
    }

#endif
