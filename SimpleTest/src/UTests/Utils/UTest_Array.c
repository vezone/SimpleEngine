#include "UTest_Array.h"

#include "UTests/Test.h"

force_inline i32*
array_init()
{
    i32* indices = NULL;

    array_push(indices, 0);
    array_push(indices, 1);
    array_push(indices, 2);
    array_push(indices, 2);
    array_push(indices, 3);
    array_push(indices, 0);

    return indices;
}

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
array_test()
{
    TEST(array_push_test());
    TEST(array_copy_test());
}
