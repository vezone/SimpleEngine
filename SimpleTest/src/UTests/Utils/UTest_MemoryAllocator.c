#include "UTest_MemoryAllocator.h"

#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "Utils/Logger.h"
#include "Utils/MemoryAllocator.h"
#include "Utils/Array.h"
#include "UTests/Test.h"

static Memory* g_Memory;
static Memory g_TempMemory;

typedef struct v3
{
    u8 X;
    u8 Y;
    u8 Z;
} v3;

typedef struct v2
{
    u8 X;
    u8 Y;
} v2;

typedef struct Entity
{
    u32 ID;
    v2 Scale;
    v3 Position;
} Entity;

#define ResetTemp() g_TempMemory.IsInitialized = 0;	\
    memory_init(&g_TempMemory);

#define CheckAllocation(data)			\
{\
    Block* ptr = list_find_data(&g_TempMemory.AllocList, data);\
    assert(ptr);					       \
}


static void
array_allocation_test()
{
    ResetTemp();

    u32 count = 5;
    i32* data = (i32*) memory_allocate_w(&g_TempMemory, count*sizeof(i32));
    CheckAllocation(data);

    for (i32 i = 0; i < count; i++)
    {
	data[i] = i;
    }

    //FIX(bies): memory_free doesn't work
    Condition(data[0] == 0);
    Condition(data[1] == 1);
    Condition(data[2] == 2);
    Condition(data[3] == 3);
    Condition(data[4] == 4);

    memory_free_w(&g_TempMemory, data);

    Condition(g_TempMemory.AllocList.Count == 0);
    Condition(g_TempMemory.FreeList.Count  == 1);
}

static void
big_allocation_test()
{
    ResetTemp();

    //90 mb
    void* data0 = memory_allocate_w(&g_TempMemory, MB(15));
    void* data1 = memory_allocate_w(&g_TempMemory, MB(5));
    void* data2 = memory_allocate_w(&g_TempMemory, MB(25));
    void* data3 = memory_allocate_w(&g_TempMemory, MB(15));
    void* data4 = memory_allocate_w(&g_TempMemory, MB(30));

    CheckAllocation(data0);
    CheckAllocation(data1);
    CheckAllocation(data2);
    CheckAllocation(data3);
    CheckAllocation(data4);

    Condition(data0 != NULL);
    Condition(data1 != NULL);
    Condition(data2 != NULL);
    Condition(data3 != NULL);
    Condition(data4 != NULL);

    memory_free_w(&g_TempMemory, data1);
    memory_free_w(&g_TempMemory, data2);
    memory_free_w(&g_TempMemory, data3);

    Int_Value(g_TempMemory.AllocList.Count);
    Int_Value(g_TempMemory.FreeList.Count);
    Condition(g_TempMemory.AllocList.Count == 2);
    Condition(g_TempMemory.FreeList.Count == 3);
}

static void
defragmentation_test()
{
    ResetTemp();

    void* data0 = memory_allocate_w(&g_TempMemory, MB(15));
    void* data1 = memory_allocate_w(&g_TempMemory, MB(4));
    void* data2 = memory_allocate_w(&g_TempMemory, MB(28));
    void* data3 = memory_allocate_w(&g_TempMemory, MB(13));
    void* data4 = memory_allocate_w(&g_TempMemory, MB(23));
    void* data5 = memory_allocate_w(&g_TempMemory, MB(2));

    CheckAllocation(data0);
    CheckAllocation(data1);
    CheckAllocation(data2);
    CheckAllocation(data3);
    CheckAllocation(data4);
    CheckAllocation(data5);

    memory_free_w(&g_TempMemory, data1);
    memory_free_w(&g_TempMemory, data2);
    memory_free_w(&g_TempMemory, data3);
    memory_free_w(&g_TempMemory, data4);

    //NOTE(bies): это запустит процесс дефрагментации
    void* otherData = memory_allocate_w(&g_TempMemory, 12);
    CheckAllocation(otherData);

    Condition(otherData != NULL);

    memory_free_w(&g_TempMemory, otherData);
    memory_free_w(&g_TempMemory, data0);
    memory_free_w(&g_TempMemory, data5);
}

static void
memory_reallocate_test()
{
    ResetTemp();

    i32 i = 0;
    i32* array = (i32*) memory_allocate_w(&g_TempMemory, 10 * sizeof(i32));
    CheckAllocation(array);
    for (i = 0; i < 10; i++)
    {
	array[i] = i;
    }

    array = memory_reallocate_w(&g_TempMemory, array, 15 * sizeof(i32));
    CheckAllocation(array);

    Condition(array != NULL);

    Condition(array[0] == 0);
    Condition(array[1] == 1);
    Condition(array[2] == 2);
    Condition(array[3] == 3);
    Condition(array[4] == 4);
    Condition(array[5] == 5);
    Condition(array[6] == 6);
    Condition(array[7] == 7);
    Condition(array[8] == 8);
    Condition(array[9] == 9);

    memory_free_w(&g_TempMemory, array);
}

static void
free_in_weird_order_test()
{
    ResetTemp();

    Entity* entity00 = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity0  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity1  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity2  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity3  = memory_allocate_w(&g_TempMemory, sizeof(Entity));

    Condition(g_TempMemory.AllocList.Count == 5);

    memory_free_w(&g_TempMemory, entity0);
    memory_free_w(&g_TempMemory, entity1);
    memory_free_w(&g_TempMemory, entity3);
    memory_free_w(&g_TempMemory, entity00);
    memory_free_w(&g_TempMemory, entity2);

    Int_Value(g_TempMemory.FreeList.Count);
    Condition(g_TempMemory.FreeList.Count == 5);
}

static void
free_step_by_step_test()
{
    ResetTemp();

    Entity* entity00 = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity0  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity1  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity2  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    Entity* entity3  = memory_allocate_w(&g_TempMemory, sizeof(Entity));

    memory_free_w(&g_TempMemory, entity00);
    memory_free_w(&g_TempMemory, entity0);
    memory_free_w(&g_TempMemory, entity1);
    memory_free_w(&g_TempMemory, entity2);
    memory_free_w(&g_TempMemory, entity3);

    Condition(g_TempMemory.FreeList.Count != 0);
    Condition(g_TempMemory.FreeList.Count == 5);
}

static void
alot_allocations()
{
    ResetTemp();

    void* data;
    data = memory_allocate_w(&g_TempMemory, 100);
    memory_free_w(&g_TempMemory, data);
    data = memory_allocate_w(&g_TempMemory, 1200);
    memory_free_w(&g_TempMemory, data);
    data = memory_allocate_w(&g_TempMemory, 700);
    memory_free_w(&g_TempMemory, data);

    Entity* entity0  = memory_allocate_w(&g_TempMemory, sizeof(Entity));
    i32* indices = memory_allocate_w(&g_TempMemory, 17 * sizeof(i32));
    void* texture = memory_allocate_w(&g_TempMemory, 1920*1080*4);
    void* texture2 = memory_allocate_w(&g_TempMemory, 1920*1080*4);

    memory_free_w(&g_TempMemory, texture2);

    memory_print_heap_snapshot();

    lista_show();

    Condition(g_TempMemory.AllocList.Count == 3);
}

static void
list_test()
{
    List list;
    Block block1 = {
	.PtrSize = 30
    };
    Block block2 = {
	.PtrSize = 125
    };
    Block block3 = {
	.PtrSize = 25
    };
    Block block4 = {
	.PtrSize = 325
    };

    list_init(&list);
    list_add(&list, &block1);
    list_add(&list, &block2);
    list_add(&list, &block3);
    list_add(&list, &block4);

    Condition(block1.Next == &block2);
    Condition(block2.Prev == &block1);
    Condition(block2.Next == &block3);
    Condition(block3.Prev == &block2);
    Condition(block3.Next == &block4);
    Condition(block4.Prev == &block3);

    list_remove(&list, &block2);

    Condition(block1.Next == &block3);
    Condition(block3.Prev == &block1);
    Condition(block3.Next == &block4);
    Condition(block4.Prev == &block3);
}

void
memory_allocator_test()
{
    g_Memory = memory_allocator_get();

    TEST(array_allocation_test());
    TEST(big_allocation_test());
    TEST(defragmentation_test());
    TEST(memory_reallocate_test());

    TEST(free_in_weird_order_test());
    TEST(free_step_by_step_test());

    TEST(alot_allocations());

    TEST(list_test());
}
