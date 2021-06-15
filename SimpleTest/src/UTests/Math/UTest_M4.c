#include "UTest_M4.h"
#include "UTests/Test.h"
#include "Math/M4.h"

void
m4_mul_v4_test()
{
    m4 matrix = (m4) {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
    };

    v4 vector = (v4) { 2.0f, 2.0f, 2.0f, 1.0f };

    v4 result;
    m4_mul_v4(result, matrix, vector);
}

void
m4_test()
{
    TEST(m4_mul_v4_test());
}
