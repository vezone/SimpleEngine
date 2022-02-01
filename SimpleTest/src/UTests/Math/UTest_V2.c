#include "UTest_V2.h"
#include "UTests/Test.h"
#include <Math/Math.h>

void
v2_dot_test()
{
    v2 a = { 2, 3 };
    v2 b = { 5, 8 };
    f32 dot = v2_dot(a, b);
    F32_Value(dot);
    F32_Is_Equal(dot, 34.0f);
}

void
v2_length_test()
{
    v2 v = { 2, 3 };
    f32 length = v2_length(v);
    F32_Is_Equal(length, 3.605551f);
    F32_Value(length);
}

void
v2_distance_test()
{
    v2 a = { 2, 3 };
    v2 b = { 5, 8 };
    f32 distance = v2_distance(a, b);
    F32_Value(distance);
    F32_Is_Equal(distance, 5.830952f);
}

force_inline f32
v2_distance_pow_test()
{
    v2 a = { 2.0f, 3.0f };
    v2 b = { 3.0f, 2.0f };
    f32 distance = v2_distance(a, b);
    f32 distanceMul = distance * distance;
    f32 distancePow = v2_distance_pow(a, b);

    F32_Value(distancePow);
    F32_Is_Equal(distancePow, distanceMul);
}

void
v2_normalize_test()
{
    v2 a = { 2, 3 };
    v2_normalize(a);
    f32 length = v2_length(a);
    F32_Value(length);
    F32_Is_Equal(length, 1.0);
}

force_inline void
v2_copy_test()
{
    v2 a = { 1.44f, 1.44f };
    v2 dest;
    v2_copy(dest, a);

    V2_Value(dest);
}

void
v2_test()
{
    TEST(v2_dot_test());
    TEST(v2_length_test());
    TEST(v2_distance_test());
    TEST(v2_distance_pow_test());
    TEST(v2_normalize_test());
    TEST(v2_copy_test());
}
