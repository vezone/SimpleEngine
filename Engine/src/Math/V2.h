#ifndef V2_H
#define V2_H

#include "Math.h"
#include "BaseMath.h"
#include "MathTypes.h"

force_inline f32
v2_assign_xy(v2 dest, f32 x, f32 y)
{
    dest[0] = x;
    dest[1] = y;
}

force_inline f32
v2_dot(v2 a, v2 b)
{
    return a[0] * b[0] + a[1] * b[1];
}

force_inline f32
v2_length(v2 v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

force_inline f32
v2_distance(v2 a, v2 b)
{
    f32 distance = sqrtf(pow2f(a[0] - b[0]) + pow2f(a[1] - b[1]));
    return distance;
}

force_inline f32
v2_distance_pow(v2 a, v2 b)
{
    f32 distance = pow2f(a[0] - b[0]) + pow2f(a[1] - b[1]);
    return distance;
}

force_inline void
v2_normalize(v2 v)
{
    f32 length = v2_length(v);
    v[0] /= length;
    v[1] /= length;
}

force_inline void
v2_copy(v2 dest, v2 b)
{
    dest[0] = b[0];
    dest[1] = b[1];
}

/*
    Arithmetic's operations
*/
force_inline void
v2_add(v2 dest, v2 a, v2 b)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
}

force_inline void
v2_sub(v2 dest, v2 a, v2 b)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
}

force_inline void
v2_mul(v2 dest, v2 a, v2 b)
{
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
}

force_inline void
v2_div(v2 dest, v2 a, v2 b)
{
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
}

force_inline void
v2_addv(v2 dest, v2 b, i32 num)
{
    dest[0] = b[0] * num;
    dest[1] = b[1] * num;
}

force_inline void
v2_subv(v2 dest, v2 b, i32 num)
{
    dest[0] = b[0] - num;
    dest[1] = b[1] - num;
}

force_inline void
v2_mulv(v2 dest, v2 b, i32 num)
{
    dest[0] = b[0] * num;
    dest[1] = b[1] * num;
}

force_inline void
v2_divv(v2 dest, v2 b, i32 num)
{
    dest[0] = b[0] - num;
    dest[1] = b[1] - num;
}

force_inline void
v2_add_ref(v2 dest, v2 b)
{
    dest[0] += b[0];
    dest[1] += b[1];
}

force_inline void
v2_sub_ref(v2 dest, v2 b)
{
    dest[0] -= b[0];
    dest[1] -= b[1];
}

force_inline void
v2_mul_ref(v2 dest, v2 b)
{
    dest[0] *= b[0];
    dest[1] *= b[1];
}

force_inline void
v2_div_ref(v2 dest, v2 b)
{
    dest[0] /= b[0];
    dest[1] /= b[1];
}

force_inline void
v2_addv_ref(v2 dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
}

force_inline void
v2_subv_ref(v2 dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
}

force_inline void
v2_mulv_ref(v2 dest, i32 num)
{
    dest[0] *= num;
    dest[1] *= num;
}

force_inline void
v2_divv_ref(v2 dest, i32 num)
{
    dest[0] /= num;
    dest[1] /= num;
}

#endif
