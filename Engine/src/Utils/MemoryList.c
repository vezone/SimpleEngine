#include "MemoryList.h"
#include "StringBuilder.h"
#include "Logger.h"

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

void
block_create(MemoryBlock* block, void* address, u64 size, const char* file, i32 line)
{
    block->Address = address;
    block->PtrSize = size;
    block->File = file;
    block->Line = line;
}

char*
block_to_string(MemoryBlock* block)
{
    const char* file = block->File ? block->File : "None";
    char* builder = NULL;
    string_builder_appendf(builder, "File: %s, Line: %d, PtrSize: %ld Prev: %ld Curr: %ld Next: %ld", file, block->Line, block->PtrSize, u64(block->Prev), u64(block), u64(block->Next));
    return builder;
}

void
block_show(MemoryBlock* block)
{
    printf("Size: %lu ", block->PtrSize);
    print_address("Prev", (void*) block->Prev);
    print_address("Current", (void*) block);
    print_address("Next", (void*) block->Next);
    printf("\n");
}

void
list_init(MemoryList* list)
{
    list->Size = 0;
    list->Count = 0;
    list->Begin = NULL;
    list->End = NULL;
}

void
list_add(MemoryList* list, MemoryBlock* element)
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
list_add_in_pointer_order(MemoryList* list, MemoryBlock* element)
{
    if (list->Begin == NULL || list->End == NULL)
    {
	list->Begin = element;
	list->End = element;
    }
    else
    {
	for (MemoryBlock* ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
	{
	    if (u64(ptr) > u64(element))
	    {
		if (ptr == list->Begin)
		{
		    MemoryBlock* begin = list->Begin;
		    list->Begin = element;
		    element->Prev = NULL;
		    element->Next = begin;
		}
		else
		{
		    MemoryBlock* prev = ptr->Prev;
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
list_remove(MemoryList* list, MemoryBlock* element)
{
    MemoryBlock* prev = element->Prev;
    MemoryBlock* next = element->Next;

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

void
list_clean(MemoryList* list)
{
    while (list->Begin != NULL)
    {
	block_show(list->Begin);
	list_remove(list, list->Begin);
    }

    list->Count = 0;
    list->Size = 0;
}
