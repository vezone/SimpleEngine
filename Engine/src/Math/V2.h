#ifndef V2_H
#define V2_H

#include "math.h"
#include "BaseMath.h"
#include "MathTypes.h"

force_inline void
v2_assign(v2 v, f32 x, f32 y)
{
    v[0] = x;
    v[1] = y;
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

force_inline void
v2_normalize(v2 v)
{
    f32 length = v2_length(v);
    v[0] /= length;
    v[1] /= length;
}

force_inline void
v2_add(v2 a, v2 b, v2 dest)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
}

force_inline void
v2_add_ref(v2 a, v2 b)
{
    a[0] += b[0];
    a[1] += b[1];
}

force_inline void
v2_addv(v2 v, i32 num, v2 dest)
{
    dest[0] = v[0] + num;
    dest[1] = v[1] + num;
}

force_inline void
v2_addv_ref(v2 dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
}

force_inline void
v2_sub(v2 a, v2 b, v2 dest)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
}

force_inline void
v2_sub_ref(v2 a, v2 b)
{
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
}

force_inline void
v2_subv(v2 v, i32 num, v2 dest)
{
    dest[0] = v[0] - num;
    dest[1] = v[1] - num;
}

force_inline void
v2_subv_ref(v2 dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
}

force_inline void
v2_mul(v2 a, v2 b, v2 dest)
{
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
}

force_inline void
v2_mul_ref(v2 a, v2 b)
{
    a[0] = a[0] * b[0];
    a[1] = a[1] * b[1];
}

force_inline void
v2_mulv(v2 v, i32 num, v2 dest)
{
    dest[0] = v[0] * num;
    dest[1] = v[1] * num;
}

// v2_mulv_ref
force_inline void
v2_scale(v2 dest, i32 num)
{
    dest[0] *= num;
    dest[1] *= num;
}

force_inline void
v2_div(v2 a, v2 b, v2 dest)
{
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
}

force_inline void
v2_div_ref(v2 a, v2 b)
{
    a[0] = a[0] / b[0];
    a[1] = a[1] / b[1];
}

/*
   v2_dot(a, b) == v2_length(a) * v2_length(b) * cos(v2_angle_between(a, b))
*/
force_inline f32
v2_dot(v2 a, v2 b)
{
    return a[0] * b[0] + a[1] * b[1];
}

#endif
