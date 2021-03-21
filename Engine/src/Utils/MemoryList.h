#ifndef MEMORY_LIST_H
#define MEMORY_LIST_H

#include "Utils/Types.h"

//we shouldn't save void* Ptr;
#ifdef ENGINE_DEBUG
typedef struct Block
{
    struct Block* Prev;
    struct Block* Next;
    const char* File;
    i32 Line;
    //NOTE(bies): this is size wo header
    u64 PtrSize;
} Block;
#else
typedef struct Block
{
    struct Block* Prev;
    struct Block* Next;
    //NOTE(bies): this is size wo header
    u64 PtrSize;
} Block;
#endif

typedef struct List
{
    u32 Size;
    u32 Count;
    Block* Begin;
    Block* End;
} List;

void print_address(const char* text, void* address);

Block* block_create(void* address, u64 size, char* file, i32 line);
char* block_to_string(Block* block);
void block_show(Block* block);

void list_init(List* list);
void list_add(List* list, Block* element);
void list_add_in_pointer_order(List* list, Block* element);
void list_remove(List* list, Block* element);
Block* list_find_data(List* list, void* data);
void list_clean(List* list);
void list_show(List* list, const char* info);
u8 list_validate(List* list);

#endif
