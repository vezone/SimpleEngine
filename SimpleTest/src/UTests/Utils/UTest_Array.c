#include "UTest_Array.h"

#include "UTests/Test.h"

void
array_push_test()
{
    i32* indices = NULL;

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

    Condition(array_len(indices) == 6);
}

void
array_test()
{
    TEST(array_push_test());
}
