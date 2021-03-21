#include "UTest_BaseMath.h"

#include "UTests/Test.h"
#include "Math/BaseMath.h"

static void
number_rank_test()
{
    Condition(number_rank(1000) == 3);
}

static void
number_of_rank_test()
{
    Condition(number_of_rank(1, 3) == 1000);
    Condition(number_of_rank(12, 3) == 12000);
}

void
base_math_test()
{
    TEST(number_rank_test());
    TEST(number_of_rank_test());
}
