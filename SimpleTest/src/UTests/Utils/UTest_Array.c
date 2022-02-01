#include "UTest_Array.h"

#include "UTests/Test.h"
#include <Utils/SimpleStandardLibrary.h>

force_inline i32*
array_init()
{
    TimeState state;
    profiler_start(&state);
    i32* indices = NULL;

    array_push(indices, 0);
    array_push(indices, 1);
    array_push(indices, 2);
    array_push(indices, 2);
    array_push(indices, 3);
    array_push(indices, 0);
    profiler_end(&state);
    String_Value(profiler_get_string(&state));

    return indices;
}

typedef struct TestType0
{
    i32 A;
    i32 B;
} TestType0;

void
array_push_test()
{
    i32* indices = array_init();

    Condition(indices[0] == 0);
    Condition(indices[1] == 1);
    Condition(indices[2] == 2);
    Condition(indices[3] == 2);
    Condition(indices[4] == 3);
    Condition(indices[5] == 0);

    Condition(array_count(indices) == 6);

    array_push(indices, (i32){337});

    TestType0* someArray = NULL;
    array_push(someArray, (TestType0) { 123, 223 });
    Condition(someArray[0].A == 123);
    Condition(someArray[0].B == 223);
}

void
array_copy_test()
{
    i32* indices = array_init();
    i32* indicesCopy = (i32*)array_copy((const void*)indices);

    Condition(indicesCopy[0] == 0);
    Condition(indicesCopy[1] == 1);
    Condition(indicesCopy[2] == 2);
    Condition(indicesCopy[3] == 2);
    Condition(indicesCopy[4] == 3);
    Condition(indicesCopy[5] == 0);

    Condition(array_count(indicesCopy) == 6);
    Int_Value(array_count(indicesCopy));
}

void
array_push_at_test()
{
    i32* indices = NULL;
    array_push_at(indices, 0, 0);
    array_push_at(indices, 1, 1);
    array_push_at(indices, 2, 2);
    array_push_at(indices, 3, 2);
    array_push_at(indices, 4, 3);
    array_push_at(indices, 5, 0);
    Condition(indices[0] == 0);
    Condition(indices[1] == 1);
    Condition(indices[2] == 2);
    Condition(indices[3] == 2);
    Condition(indices[4] == 3);
    Condition(indices[5] == 0);

    i32* indices2 = NULL;
    indices2 = array_reserve(indices2, 6);
    array_push_at(indices2, 0, 0);
    array_push_at(indices2, 1, 1);
    array_push_at(indices2, 2, 2);
    array_push_at(indices2, 3, 2);
    array_push_at(indices2, 4, 3);
    array_push_at(indices2, 5, 0);
    Condition(indices2[0] == 0);
    Condition(indices2[1] == 1);
    Condition(indices2[2] == 2);
    Condition(indices2[3] == 2);
    Condition(indices2[4] == 3);
    Condition(indices2[5] == 0);
}

void
array_reserve_test()
{
    i32* indices = NULL;
    indices = array_reserve(indices, 6);

    Condition(indices != NULL);

    array_push(indices, 0);
    array_push(indices, 1);
    array_push(indices, 2);
    array_push(indices, 2);
    array_push(indices, 3);
    array_push(indices, 0);

    Condition(indices[0] == 0);
    Condition(indices[1] == 1);
    Condition(indices[2] == 2);
    Condition(indices[3] == 2);
    Condition(indices[4] == 3);
    Condition(indices[5] == 0);

    Condition(array_count(indices) == 6);
    Int_Value(array_count(indices));

    Condition(array_capacity(indices) == 6);
    Int_Value(array_capacity(indices));
}

void
array_clear_test()
{
    i32* array = NULL;

    array_push(array, 0);
    array_push(array, 1);
    array_push(array, 2);
    array_push(array, 3);
    array_push(array, 4);
    array_push(array, 5);

    array_clear(array);

    Condition(array[0] == 0);
    Condition(array[1] == 0);
    Condition(array[2] == 0);
    Condition(array[3] == 0);
    Condition(array[4] == 0);
    Condition(array[5] == 0);

    array[0] = 9;
    array[1] = 9;
    array[2] = 9;
    array[3] = 9;
    array[4] = 9;
    array[5] = 9;

    Condition(array[0] == 9);
    Condition(array[1] == 9);
    Condition(array[2] == 9);
    Condition(array[3] == 9);
    Condition(array[4] == 9);
    Condition(array[5] == 9);

    //GERROR("%lld %lld\n", );
    array_free(array);
}

void
array_crash_test()
{
    TimeState state;
    profiler_start(&state);

    i32* array = NULL;
    i32 count = 100000000;
    for (i32 i = 0; i < count; i++)
    {
	array_push(array, i);
    }
    profiler_end(&state);
    String_Value(profiler_get_string_as_float(&state));

    Condition(array[count - 1] == (count - 1));
    Int_Value(array[count - 1]);
}

void
array_remove_test()
{
    i32* arr = NULL;

    array_push(arr, 0);
    array_push(arr, 1);
    array_push(arr, 2);
    array_push(arr, 3);
    array_push(arr, 4);
    array_push(arr, 5);

    i32 i, count = array_count(arr);
    for (i = 0; i < count; ++i)
    {
	I32_Value(arr[i]);
    }

    array_remove(arr, 3);

    Condition(arr[0] == 0);
    Condition(arr[1] == 1);
    Condition(arr[2] == 2);
    Condition(arr[3] == 4);
    Condition(arr[4] == 5);
}

void
array_test()
{
    TEST(array_push_test());
    TEST(array_copy_test());
    TEST(array_push_at_test());
    TEST(array_reserve_test());
    TEST(array_clear_test());
    TEST(array_remove_test());
}
