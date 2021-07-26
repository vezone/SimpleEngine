#ifndef V3_H
#define V3_H

#include "Math.h"
#include "BaseMath.h"
#include "MathTypes.h"

force_inline void
v3_v4(v4 v, v3 r)
{
    r[0] = v[0];
    r[1] = v[1];
    r[2] = v[2];
}

force_inline void
v3_assign(v3 a, v3 b)
{
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
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

force_inline void
v3_normalize(v3 v)
{
    f32 length = v3_length(v);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
}

force_inline void
v3_normalize_to(v3 v, v3 dest)
{
    f32 length = v3_length(v);
    dest[0] = length * v[0];
    dest[1] = length * v[1];
    dest[2] = length * v[2];
}

force_inline void
v3_add(v3 a, v3 b, v3 dest)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

force_inline void
v3_add_ref(v3 a, v3 b)
{
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
}

force_inline void
v3_addv(v3 v, i32 num, v3 dest)
{
    dest[0] = v[0] + num;
    dest[1] = v[1] + num;
    dest[2] = v[2] + num;
}

force_inline void
v3_addv_ref(v3 dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
    dest[2] += num;
}

force_inline void
v3_sub(v3 a, v3 b, v3 dest)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
}

force_inline void
v3_sub_ref(v3 a, v3 b)
{
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
    a[2] = a[2] - b[2];
}

force_inline void
v3_subv(v3 v, i32 num, v3 dest)
{
    dest[0] = v[0] - num;
    dest[1] = v[1] - num;
    dest[2] = v[2] - num;
}

force_inline void
v3_subv_ref(v3 dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
    dest[2] -= num;
}

force_inline void
v3_mul(v3 a, v3 b, v3 dest)
{
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
}

force_inline void
v3_mul_ref(v3 a, v3 b)
{
    a[0] = a[0] * b[0];
    a[1] = a[1] * b[1];
    a[2] = a[2] * b[2];
}

force_inline void
v3_mul_i32(v3 v, i32 num, v3 dest)
{
    dest[0] = v[0] * num;
    dest[1] = v[1] * num;
    dest[2] = v[2] * num;
}

force_inline void
v3_mul_f32(v3 v, f32 num, v3 dest)
{
    dest[0] = v[0] * num;
    dest[1] = v[1] * num;
    dest[2] = v[2] * num;
}

force_inline void
v3_div(v3 a, v3 b, v3 dest)
{
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
    dest[2] = a[2] / b[2];
}

force_inline void
v3_div_ref(v3 a, v3 b)
{
    a[0] = a[0] / b[0];
    a[1] = a[1] / b[1];
    a[2] = a[2] / b[2];
}

force_inline f32
v3_dot(v3 a, v3 b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

force_inline void
v3_cross(v3 a, v3 b, v3 r)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

force_inline void
v3_negative(v3 v)
{
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

force_inline void
v3_negative_to(v3 v, v3 dest)
{
    dest[0] = -v[0];
    dest[1] = -v[1];
    dest[2] = -v[2];
}

#endif
