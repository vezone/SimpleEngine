#ifndef MEMORY_LIST_H
#define MEMORY_LIST_H

#include "Types.h"

typedef struct MemoryBlock
{
    struct MemoryBlock* Prev;
    struct MemoryBlock* Next;
    void* Address;
    const char* File;
    i32 Line;
    //NOTE(bies): this is size wo header
    u64 PtrSize;
} MemoryBlock;

typedef struct MemoryList
{
    i32 IsInitialized;
    i32 Size;
    i32 Count;
    MemoryBlock* Begin;
    MemoryBlock* End;
} MemoryList;

void block_create(MemoryBlock* block, void* address, u64 size, const char* file, i32 line);
char* block_to_string(MemoryBlock* block);
void block_show(MemoryBlock* block);
void list_init(MemoryList* list);
void list_add(MemoryList* list, MemoryBlock* element);
void list_add_in_pointer_order(MemoryList* list, MemoryBlock* element);
void list_remove(MemoryList* list, MemoryBlock* element);
void list_clean(MemoryList* list);

#endif
