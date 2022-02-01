#ifndef V3_H
#define V3_H

#include "Math.h"
#include "BaseMath.h"
#include "MathTypes.h"

force_inline f32
v3_assign_xyz(v3 dest, f32 x, f32 y, f32 z)
{
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}

force_inline f32
v3_dot(v3 a, v3 b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

force_inline void
v3_cross(v3 dest, v3 a, v3 b)
{
    dest[0] = a[1] * b[2] - a[2] * b[1];
    dest[1] = a[2] * b[0] - a[0] * b[2];
    dest[2] = a[0] * b[1] - a[1] * b[0];
}

force_inline f32
v3_length(v3 v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

force_inline f32
v3_distance(v3 a, v3 b)
{
    f32 distance = sqrtf(pow2f(a[0] - b[0]) + pow2f(a[1] - b[1]) + pow2f(a[2] - b[2]));
    return distance;
}

force_inline f32
v3_distance_pow(v3 a, v3 b)
{
    f32 distance = pow2f(a[0] - b[0]) + pow2f(a[1] - b[1]) + pow2f(a[2] - b[2]);
    return distance;
}

force_inline void
v3_normalize(v3 v)
{
    f32 length = v3_length(v);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
}

force_inline void
v3_normalize_to(v3 dest, v3 v)
{
    f32 length = v3_length(v);
    dest[0] = length * v[0];
    dest[1] = length * v[1];
    dest[2] = length * v[2];
}

force_inline void
v3_negative(v3 v)
{
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

force_inline void
v3_negative_to(v3 dest, v3 v)
{
    dest[0] = -v[0];
    dest[1] = -v[1];
    dest[2] = -v[2];
}

force_inline void
v3_copy(v3 dest, v3 b)
{
    dest[0] = b[0];
    dest[1] = b[1];
    dest[2] = b[2];
}

/*
    Arithmetic's operations
*/
force_inline void
v3_add(v3 dest, v3 a, v3 b)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

force_inline void
v3_sub(v3 dest, v3 a, v3 b)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
}

force_inline void
v3_mul(v3 dest, v3 a, v3 b)
{
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
    dest[2] = a[2] * b[2];
}

force_inline void
v3_div(v3 dest, v3 a, v3 b)
{
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
    dest[2] = a[2] / b[2];
}

force_inline void
v3_addv(v3 dest, v3 b, i32 num)
{
    dest[0] = b[0] * num;
    dest[1] = b[1] * num;
    dest[2] = b[2] * num;
}

force_inline void
v3_subv(v3 dest, v3 b, i32 num)
{
    dest[0] = b[0] - num;
    dest[1] = b[1] - num;
    dest[2] = b[2] - num;
}

force_inline void
v3_mulv(v3 dest, v3 b, i32 num)
{
    dest[0] = b[0] * num;
    dest[1] = b[1] * num;
    dest[2] = b[2] * num;
}

force_inline void
v3_divv(v3 dest, v3 b, i32 num)
{
    dest[0] = b[0] - num;
    dest[1] = b[1] - num;
    dest[2] = b[2] - num;
}

force_inline void
v3_add_ref(v3 dest, v3 b)
{
    dest[0] += b[0];
    dest[1] += b[1];
    dest[2] += b[2];
}

force_inline void
v3_sub_ref(v3 dest, v3 b)
{
    dest[0] -= b[0];
    dest[1] -= b[1];
    dest[2] -= b[2];
}

force_inline void
v3_mul_ref(v3 dest, v3 b)
{
    dest[0] *= b[0];
    dest[1] *= b[1];
    dest[2] *= b[2];
}

force_inline void
v3_div_ref(v3 dest, v3 b)
{
    dest[0] /= b[0];
    dest[1] /= b[1];
    dest[2] /= b[2];
}

force_inline void
v3_addv_ref(v3 dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
    dest[2] += num;
}

force_inline void
v3_subv_ref(v3 dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
    dest[2] -= num;
}

force_inline void
v3_mulv_ref(v3 dest, i32 num)
{
    dest[0] *= num;
    dest[1] *= num;
    dest[2] *= num;
}

force_inline void
v3_divv_ref(v3 dest, i32 num)
{
    dest[0] /= num;
    dest[1] /= num;
    dest[2] /= num;
}

/* Convertation */
force_inline void
v3_v4(v3 dest, v3 v)
{
    dest[0] = v[0];
    dest[1] = v[1];
    dest[2] = v[2];
}

#endif
