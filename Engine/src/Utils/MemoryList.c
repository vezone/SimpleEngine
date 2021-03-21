#include "MemoryList.h"

#include <stdlib.h>
#include "Logger.h"
#include "StringBuilder.h"

#define BLOCK_GET_PTR(block) (u64(block) + sizeof(Block))

void
print_address(const char* text, void* address)
{
    u64 addr = u64(address);
    if (addr)
    {
	u64 lowAddress = addr % 1000;
	char add[3];
	if (lowAddress < 100 && lowAddress >= 10)
	{
	    add[0] = '0';
	    add[1] = '\0';
	}
	else if (lowAddress < 10)
	{
	    add[0] = '0';
	    add[1] = '0';
	    add[2] = '\0';
	}
	else
	{
	    add[0] = '\0';
	    add[1] = ' ';
	    add[2] = ' ';
	}

	printf("%s: %lu"YELLOW("%s")YELLOW("%lu "), text, addr / 1000,  add, lowAddress);
    }
    else
    {
	printf("%s: "RED("      NULL      "), text);
    }
}

Block*
block_create(void* address, u64 size, char* file, i32 line)
{
    Block* newBlock = (Block*) address;
    newBlock->PtrSize = size;
#ifdef ENGINE_DEBUG
    newBlock->File = file;
    newBlock->Line = line;
#endif

    return newBlock;
}

char*
block_to_string(Block* block)
{
#ifdef ENGINE_DEBUG
    char* file = block->File ? block->File : "None";
#endif
    StringBuilder builder;
    vstring_builder_init(&builder);
#ifdef ENGINE_DEBUG
    vstring_builder_append_format(&builder, "File: %s, Line: %d, PtrSize: %ld Prev: %ld Curr: %ld Next: %ld", file, block->Line, block->PtrSize, u64(block->Prev), u64(block), u64(block->Next));
#else
    vstring_builder_append_format(&builder, "PtrSize: %ld Prev: %ld Curr: %ld Next: %ld", block->PtrSize, u64(block->Prev), u64(block), u64(block->Next));
#endif
    return builder.Buffer;
}

void
block_show(Block* block)
{
    printf("Size: %lu ", block->PtrSize);
    print_address("Prev", (void*) block->Prev);
    print_address("Current", (void*) block);
    print_address("Next", (void*) block->Next);
    print_address("Ptr", (void*) BLOCK_GET_PTR(block));
    printf("\n");
}

void
list_init(List* list)
{
    list->Size = 0;
    list->Count = 0;
    list->Begin = NULL;
    list->End = NULL;
}

void
list_add(List* list, Block* element)
{
    if (list->End == NULL)
    {
	list->Begin = element;
	list->End = element;
	element->Prev = NULL;
	element->Next = NULL;
    }
    else
    {
	// [prev:0 cur: 1 next:2] [prev: 1 cur: 2: next 0]
	list->End->Next = element;
	element->Prev = list->End;
	element->Next = NULL;

	list->End = element;
    }

    list->Size += element->PtrSize;
    list->Count++;
}

void
list_add_in_pointer_order(List* list, Block* element)
{
    if (list->Begin == NULL || list->End == NULL)
    {
	list->Begin = element;
	list->End = element;
    }
    else
    {
	for (Block* ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
	{
	    if (u64(ptr) > u64(element))
	    {
		if (ptr == list->Begin)
		{
		    Block* begin = list->Begin;
		    list->Begin = element;
		    element->Prev = NULL;
		    element->Next = begin;
		}
		else
		{
		    Block* prev = ptr->Prev;
		    prev->Next = element;
		    ptr->Prev = element;
		    element->Prev = prev;
		    element->Next = ptr;
		}
	    }
	    else if (u64(ptr) < u64(element) && ptr == list->End)
	    {
		ptr->Next = element;
		list->End = element;
		element->Prev = ptr;
	    }
	}
    }

    list->Size += element->PtrSize;
    list->Count++;
}

void
list_remove(List* list, Block* element)
{
    Block* prev = element->Prev;
    Block* next = element->Next;

    if (list->Count == 1)
    {
	list->Begin = NULL;
	list->End = NULL;
    }
    else
    {
	//begin
	if (element == list->Begin)
	{
	    element->Next->Prev = NULL;
	    list->Begin = element->Next;
	}
	//end
	else if (element == list->End)
	{
	    element->Prev->Next = NULL;
	    list->End = element->Prev;
	}
	//mid
	else
	{
	    element->Prev->Next = element->Next;
	    element->Next->Prev = element->Prev;
	}

	//delete links on neighboring elements
	element->Prev = NULL;
	element->Next = NULL;
    }

    list->Size -= element->PtrSize;
    list->Count--;
}

Block*
list_find_data(List* list, void* data)
{
    u64 address = (u64) data;
    for (Block* ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
    {
	u64 ptrAddress = BLOCK_GET_PTR(ptr);
	if (ptrAddress == address)
	{
	    return ptr;
	}
    }

    return NULL;
}

void
list_clean(List* list)
{
    while (list->Begin != NULL)
    {
	block_show(list->Begin);
	list_remove(list, list->Begin);
    }

    list->Count = 0;
    list->Size = 0;
}

void
list_show(List* list, const char* info)
{
    printf("%s\nList count: %d\n", info, list->Count);
    printf("list->Begin: %lu\n", (u64)list->Begin);
    for (Block* ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
    {
	block_show(ptr);
    }
}

u8
list_validate(List* list)
{
    if (list->Count <= 0 || list->Begin == NULL || list->End == NULL)
    {
	return 0;
    }

    u64 beg = u64(list->Begin);
    u64 end = u64(list->End);

    Block* ptr;
    for (ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
    {
	u64 addr = u64(ptr);
	assert(ptr >= beg && ptr <= end);
    }

    return 1;
}
