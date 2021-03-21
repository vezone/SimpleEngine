/*

After you freeing data manually set it to NULL:
    memory_free(data);
    data = NULL;

Before freeing you shouldn't check if data == NULL, because this code is already in function.


   NOTE(bies): можно сделать препроцессор для Scope'а
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#include "MemoryAllocator.h"
#include "Utils/Logger.h"

#ifndef ALLOCATOR_TOTAL_MEMORY
#define ALLOCATOR_TOTAL_MEMORY MB(1000)
#endif

#define EMPTY_SPACE ('C')
#define DefragmentationWontHelp (size > (freeSize + memory->FreeList.Size))
#define DefragmentationWillHelp ((size > freeSize) && !DefragmentationWontHelp)
#define ShouldBeDefragmented (DefragmentationWillHelp || (fragmentation_get() > 50))

static i32 g_BlockSize = sizeof(Block);
//TODO: change Memory g_Memory; to Memory* g_Memory
static Memory* g_Memory = NULL;
static i8 g_IsMemoryInitialized = 0;
static i8 g_PercentThreshold = 70;

force_inline f32
fragmentation_get()
{
    return f32(100.0 * g_Memory->FreeList.Size / g_Memory->TotalSize);
}

force_inline u64
get_free_memory_size()
{
    return u64(g_Memory->TotalSize - g_Memory->FreeList.Size - g_Memory->AllocList.Size);
}

void*
memory_allocate_w(Memory* memory, u64 size)
{
    Memory* prevMemory = memory_allocator_get();
    memory_allocator_set(memory);
    void* data = internal_memory_allocate(size);
    memory_allocator_set(prevMemory);

    return data;
}

void*
memory_reallocate_w(Memory* memory, void* dataToRealloc, u64 newSize)
{
    Memory* prevMemory = memory_allocator_get();
    memory_allocator_set(memory);
    void* data = memory_reallocate(dataToRealloc, newSize);
    memory_allocator_set(prevMemory);

    return data;
}

void
memory_free_w(Memory* memory, void* data)
{
    Memory* prevMemory = memory_allocator_get();
    memory_allocator_set(memory);
    memory_free(data);
    memory_allocator_set(prevMemory);
}


void
memory_init(Memory* memory)
{
    if (memory->IsInitialized)
	return;

    if (memory->Memory)
	free(memory->Memory);

    GLOG(MAGNETA("Allocated: %f MB")"\n", TOMB(ALLOCATOR_TOTAL_MEMORY));
    memory->IsInitialized = 1;
    memory->Memory = malloc(ALLOCATOR_TOTAL_MEMORY);
    memory->Count = 0;
    memory->Offset = 0;
    memory->TotalSize = ALLOCATOR_TOTAL_MEMORY;

    list_init(&memory->AllocList);
    list_init(&memory->FreeList);
}

void
memory_defragmentation(Memory* memory)
{
    u64 currentAddress = (u64) memory->Memory;
    List newAllocList;
    list_init(&newAllocList);

    for (Block* ptr = memory->AllocList.Begin; ptr != NULL; ptr = ptr->Next)
    {
	if (u64(ptr) == currentAddress)
	{
	    currentAddress += ptr->PtrSize + g_BlockSize;
	    continue;
	}

	Block* newBlock = block_create(currentAddress, ptr->PtrSize, "None", -1);
	void* newData = (void*) (u64(newBlock) - g_BlockSize);
	void* ptrData = (void*) (u64(ptr) - g_BlockSize);
	memcpy(newData, ptrData, ptr->PtrSize);

	list_add(&newAllocList, newBlock);

	currentAddress += newBlock->PtrSize + g_BlockSize;
    }

    list_clean(&memory->FreeList);

    memory->AllocList = newAllocList;
}


void*
internal_memory_allocate(u64 size)
{
    Memory* memory = g_Memory;

    if (memory == NULL)
    {
	g_Memory = (Memory*) malloc(sizeof(Memory));
	memory = g_Memory;
	memory->IsInitialized = 0;
	memory->Memory = NULL;
	memory_init(memory);
    }

    u64 freeSize = get_free_memory_size();

    if (DefragmentationWontHelp)
    {
	GWARNING("Out of memory, defragmentation won't help! Allocating memory with malloc\n");
	GWARNING("free memory size: %f mb needed memory: %f mb\n", TOMB(get_free_memory_size()), TOMB(size));

	assert(0);

	return malloc(size);
    }
    else if (ShouldBeDefragmented)
    {
	//defragmentation there
#ifdef ENGINE_TEST
	memory_print_heap_snapshot();
#endif

	GLOG("Defragmentation "GREEN("starts!")"\n");
	assert(0);
	memory_defragmentation(memory);
	GLOG("Defragmentation "GREEN("ends!")"\n");

#ifdef ENGINE_TEST
	memory_print_heap_snapshot();
#endif
    }

    u64 newAddress = u64(memory->Memory) + memory->Offset;
    Block* newBlock = block_create(newAddress, size, "None", -1);
    void* data = (void*) (newAddress + g_BlockSize);

    vassert((u64(data) - g_BlockSize) == u64(newAddress));

    list_add(&memory->AllocList, newBlock);
    list_validate(&memory->AllocList);

    memory->Offset += g_BlockSize + newBlock->PtrSize;
    ++memory->Count;

    return data;
}

void*
memory_reallocate(void* dataToRealloc, u64 newSize)
{
    Block* prevBlock = list_find_data(&g_Memory->AllocList, dataToRealloc);

    //TODO(bies): realloc anyway
    if (prevBlock == NULL)
    {
	GERROR("realloc: prevBlock == NULL!\n");
	void* result = realloc(dataToRealloc, newSize);
	if (!result)
	{
	    GERROR("For some reason realloc is failed!\n");
	}

	return result;
    }

    void* data = (void*) (u64(prevBlock) + g_BlockSize);
    void* newData = internal_memory_allocate(newSize);
    memcpy(newData, data, prevBlock->PtrSize);

    list_remove(&g_Memory->AllocList, prevBlock);
    list_add(&g_Memory->FreeList, prevBlock);
    list_validate(&g_Memory->AllocList);

    return newData;
}

void
memory_free(void* data)
{
    vassert(data);

    Block* res = list_find_data(&g_Memory->AllocList, data);
    if (res == NULL)
    {
	free(data);
    }
    else
    {
	Block* ptr = (Block*) (u64(data) - g_BlockSize);
	memset(data, EMPTY_SPACE, ptr->PtrSize);

	u64 udata = u64(data);
	u64 ubegin = u64(g_Memory->AllocList.Begin);
	u64 dif = udata - ubegin;
	i8 isOk = (dif == g_BlockSize);

	list_remove(&g_Memory->AllocList, ptr);
	list_add(&g_Memory->FreeList, ptr);

	data = NULL;
    }

    list_validate(&g_Memory->AllocList);
}

void
memory_free_all()
{
    if (g_Memory->Memory)
    {
	free(g_Memory->Memory);
	g_Memory->IsInitialized = 0;
    }
}

Memory*
memory_allocator_get()
{
    if (g_Memory == NULL)
    {
	g_Memory = (Memory*) malloc(sizeof(Memory));
	g_Memory->IsInitialized = 0;
	g_Memory->Memory = NULL;

	memory_init(g_Memory);
    }

    return g_Memory;
}

void
memory_allocator_set(Memory* memory)
{
    g_Memory = memory;
}

void
memory_print_heap_snapshot()
{
    printf("\n    Heap snapshot    \n");
    printf( "______________________\n");

    u64 size = (u64) g_Memory->AllocList.Size;
    if (size > MB(1))
    {
	printf("Alloc memory: %lu MB\n", size / MB(1));
    }
    else if (size > KB(1))
    {
	printf("Alloc memory: %lu KB\n", size / KB(1));
    }
    else
    {
	printf("Alloc memory: %lu Bytes\n", size);
    }

    size = get_free_memory_size();
    if (size > MB(1))
    {
	printf("Free memory: %f MB\n", ((f64)size) / MB(1));
    }
    else if (size > KB(1))
    {
	printf("Free memory: %lu KB\n", size / KB(1));
    }
    else
    {
	printf("Free memory: %lu Bytes\n", size);
    }

    size = (u64) g_Memory->FreeList.Size;
    if (size > MB(1))
    {
	printf("Trash memory: %lu MB\n", size / MB(1));
    }
    else if (size > KB(1))
    {
	printf("Trash memory: %lu KB\n", size / KB(1));
    }
    else
    {
	printf("Trash memory: %lu Bytes\n", size);
    }

    const i64 totalMemory = ALLOCATOR_TOTAL_MEMORY;
    printf("Total: %ld mb\n", totalMemory / MB(1));
    f32 fragmentation = fragmentation_get();
    if (fragmentation > 1)
    {
	printf("Fragmentation: %d \n\n", (u32) fragmentation);
    }
    else
    {
	printf("Fragmentation: %f \n\n", fragmentation);
    }
}

void
lista_show()
{
    list_show(&g_Memory->AllocList, "Alloc List");
}

#ifdef ENGINE_DEBUG

void* debug_memory_allocate(const char* file, i32 line, u64 size)
{
    void* data = internal_memory_allocate(size);
    Block* block = list_find_data(&g_Memory->AllocList, data);

    block->File = istring(file);
    block->Line = line;

    return data;
}

void debug_memory_free(const char* file, i32 line, void* data)
{
}

#endif
