#ifndef V4_H
#define V4_H

#include "math.h"
#include "BaseMath.h"
#include "MathTypes.h"

#define v4_is_direction(v) (((v)[3] == 0) ? 1 : 0)
#define v4_is_position(v) (((v)[3] == 1) ? 1 : 0)

force_inline void
v4_assign(v4 a, v4 b)
{
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
    a[3] = b[3];
}

force_inline void
v4_copy(v4 a, v4 b)
{
    b[0] = a[0];
    b[1] = a[1];
    b[2] = a[2];
    b[3] = a[3];
}

force_inline f32
v4_length(v4 v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

force_inline f32
v4_distance(v4 a, v4 b)
{
    f32 distance = sqrt(pow2f(a[0] - b[0]) + pow2f(a[1] - b[1]) + pow2f(a[2] - b[2]) + pow2f(a[3] - b[3]));
    return distance;
}

force_inline void
v4_normalize(v4 v)
{
    f32 length = v4_length(v);
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    v[3] /= length;
}

force_inline void
v4_add(v4 a, v4 b, v4 dest)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
    dest[3] = a[3] + b[3];
}

force_inline void
v4_add_ref(v4 a, v4 b)
{
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
    a[3] += b[3];
}

force_inline void
v4_addv(v4 v, i32 num, v4 dest)
{
    dest[0] = v[0] + num;
    dest[1] = v[1] + num;
    dest[2] = v[2] + num;
    dest[3] = v[3] + num;
}

force_inline void
v4_addv_ref(v4 dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
    dest[2] += num;
    dest[3] += num;
}

force_inline void
v4_sub(v4 a, v4 b, v4 dest)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
    dest[3] = a[3] - b[3];
}

force_inline void
v4_sub_ref(v4 a, v4 b)
{
    a[0] = a[0] - b[0];
    a[1] = a[1] - b[1];
    a[2] = a[2] - b[2];
    a[3] = a[3] - b[3];
}

force_inline void
v4_subv(v4 v, i32 num, v4 dest)
{
    dest[0] = v[0] - num;
    dest[1] = v[1] - num;
    dest[2] = v[2] - num;
    dest[3] = v[3] - num;
}

force_inline void
v4_subv_ref(v4 dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
    dest[2] -= num;
    dest[3] -= num;
}

force_inline void
v4_mul(v4 a, v4 b, v4 dest)
{
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
    dest[2] = a[2] * b[2];
    dest[3] = a[3] * b[3];
}

force_inline void
v4_mul_ref(v4 a, v4 b)
{
    a[0] = a[0] * b[0];
    a[1] = a[1] * b[1];
    a[2] = a[2] * b[2];
    a[3] = a[3] * b[3];
}

force_inline void
v4_mulv(v4 v, i32 num, v4 dest)
{
    dest[0] = v[0] * num;
    dest[1] = v[1] * num;
    dest[2] = v[2] * num;
    dest[3] = v[3] * num;
}

force_inline void
v4_scale(v4 a, f32 num, v4 dest)
{
    dest[0] = a[0] * num;
    dest[1] = a[1] * num;
    dest[2] = a[2] * num;
    dest[3] = a[3] * num;
}

force_inline void
v4_div(v4 a, v4 b, v4 dest)
{
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
    dest[2] = a[2] / b[2];
    dest[3] = a[3] / b[3];
}

force_inline void
v4_div_ref(v4 a, v4 b)
{
    a[0] = a[0] / b[0];
    a[1] = a[1] / b[1];
    a[2] = a[2] / b[2];
    a[3] = a[3] / b[3];
}

force_inline f32
v4_dot(v4 a, v4 b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

#endif
