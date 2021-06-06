#include "UTest_V2.h"
#include "UTests/Test.h"
#include "Math/V2.h"

void v2_create_test()
{
    v2 size = v2(1, 0);
    Condition(size[0] == 1);
    Condition(size[1] == 0);
}

void v2_test()
{
    TEST(v2_create_test());
}
