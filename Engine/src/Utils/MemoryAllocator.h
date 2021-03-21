#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include "MemoryList.h"
#include "Array.h"
#include "String.h"

typedef struct Memory
{
    i8 IsInitialized;
    u32 Count;
    u64 TotalSize;
    u64 Offset;
    void* Memory;
    List AllocList;
    List FreeList;
} Memory;

// This is for external calls (Unit Testing)
void* memory_allocate_w(Memory* memory, u64 size);
void  memory_free_w(Memory* memory, void* data);
void* memory_reallocate_w(Memory* memory, void* dataToRealloc, u64 newSize);

void* internal_memory_allocate(u64 size);

#ifdef ENGINE_DEBUG

#endif

void  memory_init(Memory* memory);
#ifdef ENGINE_RELEASE
#define memory_allocate(sizeForAllocation) internal_memory_allocate(sizeForAllocation)
#endif
void* memory_reallocate(void* dataToRealloc, u64 newSize);
void  memory_free(void* data);
void  memory_free_all();

Memory* memory_allocator_get();
void memory_allocator_set(Memory* memory);
void memory_allocator_reset();
void memory_print_heap_snapshot();
void lista_show();

//NOTE(bies): we need List<string> для всех scope'ов, которые мы объявили, эти scope'ы должны мапаться на аллокации, которые мы произвели, SCOPE_DELETE(scopeName) удаляет все аллокации сделанные внутри scope'а. SCOPE_NEW не аллоцирует память в случае, если скоуп уже сушествует
#define SCOPE_NEW(scopeName)
#define SCOPE_DELETE(scopeName)

#ifdef ENGINE_DEBUG

typedef struct IndividualAllocInfo
{
    i32 Line;
    u64 Size;
} IndividualAllocInfo;

typedef struct FileAllocInfo
{
    const char* Name;
    IndividualAllocInfo* Allocations;
} FileAllocInfo;

void* debug_memory_allocate(const char* file, i32 line, u64 size);
void debug_memory_free(const char* file, i32 line, void* data);
FileAllocInfo* debug_memory_get_allocations();

#define memory_allocate(sizeForAllocation) debug_memory_allocate(__FILE__, __LINE__, sizeForAllocation)
//#define memory_free(dataToFree) debug_memory_free(__FILE__, __LINE__, dataToFree)

#endif

#endif
