#include "UTest_V2.h"
#include "UTests/Test.h"
#include <Math/Math.h>

#if 0
v2_assign(v2 v, v2 values)
v2_length(v2 v)
v2_distance(v2 a, v2 b)
v2_normalize(v2 v)
v2_add(v2 v, i32 num, v2 dest)
v2_add_ref(v2 dest, i32 num)
v2_sub(v2 v, i32 num, v2 dest)
v2_sub_ref(v2 dest, i32 num)
v2_mul(v2 v, i32 num, v2 dest)
v2_mul_ref(v2 dest, i32 num)
v2_div(v2 v, i32 num, v2 dest)
v2_div_ref(v2 dest, i32 num)
#endif

void
v2__test()
{
    v2 size = v2_(12, 45);

    Condition(size[0] == 12);
    Condition(size[1] == 45);
}

void
v2_assign_test()
{
    v2 v;
    v2_assign(v, 12, 45);

    Condition(v[0] == 12);
    Condition(v[1] == 45);
}

void
v2_length_test()
{
    v2 v = v2_(2, 3);
    f32 length = v2_length(v);
    F32_Is_Equal(length, 3.6055);
    F32_Value(length);
}

void
v2_distance_test()
{
    v2 a = v2_(2, 3);
    v2 b = v2_(5, 8);
    f32 distance = v2_distance(a, b);
    F32_Value(distance);
    F32_Is_Equal(distance, 5.8309);
}

void
v2_normalize_test()
{
    v2 a = v2_(2, 3);
    v2_normalize(a);
    f32 length = v2_length(a);
    F32_Value(length);
    F32_Is_Equal(length, 1.0);
}

void
v2_add_test()
{
    v2 a = v2_(2, 6);
    v2 b = v2_(8, 4);
    v2 c;

    v2_add(a, b, c);
    V2_Is_Equal(c, v2_(10, 10));
}

void
v2_add_ref_test()
{
    v2 a = v2_(2, 6);
    v2 b = v2_(8, 4);
    v2_add_ref(a, b);
    V2_Is_Equal(a, v2_(10, 10));
}

void
v2_sub_test()
{
    v2 a = v2_(2, 6);
    v2 b = v2_(8, 4);
    v2 c;
    v2_sub(a, b, c);
    V2_Is_Equal(c, v2_(-6, 2));
}

void
v2_sub_ref_test()
{
    v2 a = v2_(8, 6);
    v2 b = v2_(4, 3);
    v2_sub_ref(a, b);
    V2_Is_Equal(a, v2_(4, 3));
}

void
v2_mul_test()
{
    v2 a = v2_(2, 4);
    v2 b = v2_(3, 5);
    v2 c;
    v2_mul(a, b, c);
    V2_Is_Equal(c, v2_(6, 20));
}

void
v2_mul_ref_test()
{
    v2 a = v2_(2, 4);
    v2 b = v2_(3, 5);
    v2_mul_ref(a, b);
    V2_Is_Equal(a, v2_(6, 20));
}

void
v2_scale_test()
{
    v2 a = v2_(2, 4);
    v2_scale(a, 5);
    V2_Is_Equal(a, v2_(10, 20));
}

void
v2_div_test()
{
    v2 a = v2_(6, 10);
    v2 b = v2_(3, 5);
    v2 c;
    v2_div(a, b, c);
    V2_Is_Equal(c, v2_(2, 2));
    V2_Value(c);
}

void
v2_div_ref_test()
{
    v2 a = v2_(6, 10);
    v2 b = v2_(3, 5);
    v2_div_ref(a, b);
    V2_Is_Equal(a, v2_(2, 2));
    V2_Value(a);
}

void
v2_dot_test()
{
    v2 a = v2_(2, 5);
    v2 b = v2_(3, 2);
    f32 dot = v2_dot(a, b);
    F32_Value(dot);
    F32_Is_Equal(dot, 16.0);
}

void
v2_test()
{
    TEST(v2__test());
    TEST(v2_assign_test());
    TEST(v2_length_test());
    TEST(v2_distance_test());
    TEST(v2_normalize_test());
    TEST(v2_add_test());
    TEST(v2_add_ref_test());
    TEST(v2_sub_test());
    TEST(v2_sub_ref_test());
    TEST(v2_mul_test());
    TEST(v2_mul_ref_test());
    TEST(v2_scale_test());
    TEST(v2_div_test());
    TEST(v2_div_ref_test());
    TEST(v2_dot_test());
}
