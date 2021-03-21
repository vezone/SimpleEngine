#ifndef VECTOR3
#define VECTOR3

#include "Utils/Types.h"
#include "Math/MathTypes.h"
#include "Math/BaseMath.h"

force_inline void
vector3f_create(Vector3f vec, f32 x, f32 y, f32 z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

force_inline void
vector3f_copy(Vector3f src, Vector3f dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

force_inline void
vector3f_fill_zero(Vector3f src)
{
    src[0] = src[1] = src[2] = 0.0f;
}

force_inline void
vector3f_fill_value(Vector3f src, f32 value)
{
    src[0] = src[1] = src[2] = value;
}

force_inline void
vector3f_fill_max(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = MAX(v1[0], v2[0]);
    dest[1] = MAX(v1[1], v2[1]);
    dest[2] = MAX(v1[2], v2[2]);
}

force_inline void
vector3f_fill_max_ref(Vector3f dest, Vector3f v2)
{
    dest[0] = MAX(dest[0], v2[0]);
    dest[1] = MAX(dest[1], v2[1]);
    dest[2] = MAX(dest[2], v2[2]);
}

force_inline void
vector3f_fill_min(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = MIN(v1[0], v2[0]);
    dest[1] = MIN(v1[1], v2[1]);
    dest[2] = MIN(v1[2], v2[2]);
}

force_inline void
vector3f_fill_min_ref(Vector3f dest, Vector3f v2)
{
    dest[0] = MIN(dest[0], v2[0]);
    dest[1] = MIN(dest[1], v2[1]);
    dest[2] = MIN(dest[2], v2[2]);
}

force_inline void
vector3f_negate(Vector3f vec, Vector3f dest)
{
    dest[0] = -vec[0];
    dest[1] = -vec[1];
    dest[2] = -vec[2];
}

force_inline void
vector3f_negate_ref(Vector3f dest)
{
    dest[0] = -dest[0];
    dest[1] = -dest[1];
    dest[2] = -dest[2];
}

force_inline f32
vector3f_length(Vector3f vec)
{
    f32 length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    return length;
}

force_inline f32
vector3f_distance(Vector3f v1, Vector3f v2)
{
    f32 distance = sqrt(pow2f(v1[0] - v2[0]) + pow2f(v1[1] - v2[1]) + pow2f(v1[2] - v2[2]));
    return distance;
}

force_inline void
vector3f_add(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = v1[0] + v2[0];
    dest[1] = v1[1] + v2[1];
    dest[2] = v1[2] + v2[2];
}

force_inline void
vector3f_add_ref(Vector3f dest, Vector3f v2)
{
    dest[0] += v2[0];
    dest[1] += v2[1];
    dest[2] += v2[2];
}

force_inline void
vector3f_addf32(Vector3f v1, f32 num, Vector3f dest)
{
    dest[0] = v1[0] + num;
    dest[1] = v1[1] + num;
    dest[2] = v1[2] + num;
}

force_inline void
vector3f_addf32_ref(Vector3f dest, f32 num)
{
    dest[0] += num;
    dest[1] += num;
    dest[2] += num;
}

force_inline void
vector3f_addi32(Vector3f v1, i32 num, Vector3f dest)
{
    dest[0] = v1[0] + num;
    dest[1] = v1[1] + num;
    dest[2] = v1[2] + num;
}

force_inline void
vector3f_addi32_ref(Vector3f dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
    dest[2] += num;
}

force_inline void
vector3f_sub(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = v1[0] - v2[0];
    dest[1] = v1[1] - v2[1];
    dest[2] = v1[2] - v2[2];
}

force_inline void
vector3f_sub_ref(Vector3f dest, Vector3f v2)
{
    dest[0] -= v2[0];
    dest[1] -= v2[1];
    dest[2] -= v2[2];
}

force_inline void
vector3f_subf32(Vector3f v1, f32 num, Vector3f dest)
{
    dest[0] = v1[0] - num;
    dest[1] = v1[1] - num;
    dest[2] = v1[2] - num;
}

force_inline void
vector3f_subf32_ref(Vector3f dest, f32 num)
{
    dest[0] -= num;
    dest[1] -= num;
    dest[2] -= num;
}

force_inline void
vector3f_subi32(Vector3f v1, i32 num, Vector3f dest)
{
    dest[0] = v1[0] - num;
    dest[1] = v1[1] - num;
    dest[2] = v1[2] - num;
}

force_inline void
vector3f_subi32_ref(Vector3f dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
    dest[2] -= num;
}

force_inline void
vector3f_mul(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = v1[0] * v2[0];
    dest[1] = v1[1] * v2[1];
    dest[2] = v1[2] * v2[2];
}

force_inline void
vector3f_mul_ref(Vector3f dest, Vector3f v2)
{
    dest[0] *= v2[0];
    dest[1] *= v2[1];
    dest[2] *= v2[2];
}

force_inline void
vector3f_mulf32(Vector3f v1, f32 num, Vector3f dest)
{
    dest[0] = v1[0] * num;
    dest[1] = v1[1] * num;
    dest[2] = v1[2] * num;
}

force_inline void
vector3f_mulf32_ref(Vector3f dest, f32 num)
{
    dest[0] *= num;
    dest[1] *= num;
    dest[2] *= num;
}

force_inline void
vector3f_muli32(Vector3f v1, i32 num, Vector3f dest)
{
    dest[0] = v1[0] * num;
    dest[1] = v1[1] * num;
    dest[2] = v1[2] * num;
}

force_inline void
vector3f_muli32_ref(Vector3f dest, i32 num)
{
    dest[0] *= num;
    dest[1] *= num;
    dest[2] *= num;
}

force_inline void
vector3f_div(Vector3f v1, Vector3f v2, Vector3f dest)
{
    dest[0] = v1[0] / v2[0];
    dest[1] = v1[1] / v2[1];
    dest[2] = v1[2] / v2[2];
}

force_inline void
vector3f_div_ref(Vector3f dest, Vector3f v2)
{
    dest[0] /= v2[0];
    dest[1] /= v2[1];
    dest[2] /= v2[2];
}

force_inline void
vector3f_divf32(Vector3f v1, f32 num, Vector3f dest)
{
    dest[0] = v1[0] / num;
    dest[1] = v1[1] / num;
    dest[2] = v1[2] / num;
}

force_inline void
vector3f_divf32_ref(Vector3f dest, f32 num)
{
    dest[0] /= num;
    dest[1] /= num;
    dest[2] /= num;
}

force_inline void
vector3f_divi32(Vector3f v1, i32 num, Vector3f dest)
{
    dest[0] = v1[0] / num;
    dest[1] = v1[1] / num;
    dest[2] = v1[2] / num;
}

force_inline void
vector3f_divi32_ref(Vector3f dest, i32 num)
{
    dest[0] /= num;
    dest[1] /= num;
    dest[2] /= num;
}

#endif
