#ifndef VECTOR2
#define VECTOR2

#include "Utils/Types.h"
#include "Math/MathTypes.h"
#include "Math/BaseMath.h"

#define v2(x, y) (v2 v; v[0] = x; v[1] = y)

force_inline void
vector2f_create(Vector2f vec, f32 x, f32 y)
{
    vec[0] = x;
    vec[1] = y;
}

force_inline void
vector2f_copy(Vector2f src, Vector2f dest)
{
    dest[0] = src[0];
    dest[1] = src[1];
}

force_inline void
vector2f_fill_zero(Vector2f src)
{
    src[0] = src[1] = 0.0f;
}

force_inline void
vector2f_fill_value(Vector2f src, f32 value)
{
    src[0] = src[1] = value;
}

force_inline void
vector2f_fill_max(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = MAX(v1[0], v2[0]);
    dest[1] = MAX(v1[1], v2[1]);
}

force_inline void
vector2f_fill_max_ref(Vector2f dest, Vector2f v2)
{
    dest[0] = MAX(dest[0], v2[0]);
    dest[1] = MAX(dest[1], v2[1]);
}

force_inline void
vector2f_fill_min(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = MIN(v1[0], v2[0]);
    dest[1] = MIN(v1[1], v2[1]);
}

force_inline void
vector2f_fill_min_ref(Vector2f dest, Vector2f v2)
{
    dest[0] = MIN(dest[0], v2[0]);
    dest[1] = MIN(dest[1], v2[1]);
}

force_inline void
vector2f_negate(Vector2f vec, Vector2f dest)
{
    dest[0] = -vec[0];
    dest[1] = -vec[1];
}

force_inline void
vector2f_negate_ref(Vector2f dest)
{
    dest[0] = -dest[0];
    dest[1] = -dest[1];
}

force_inline f32
vector2f_length(Vector2f vec)
{
    f32 length = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
    return length;
}

force_inline f32
vector2f_distance(Vector2f v1, Vector2f v2)
{
    f32 distance = sqrt(pow2f(v1[0] - v2[0]) + pow2f(v1[1] - v2[1]));
    return distance;
}

force_inline void
vector2f_add(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = v1[0] + v2[0];
    dest[1] = v1[1] + v2[1];
}

force_inline void
vector2f_add_ref(Vector2f dest, Vector2f v2)
{
    dest[0] += v2[0];
    dest[1] += v2[1];
}

force_inline void
vector2f_addf32(Vector2f v1, f32 num, Vector2f dest)
{
    dest[0] = v1[0] + num;
    dest[1] = v1[1] + num;
}

force_inline void
vector2f_addf32_ref(Vector2f dest, f32 num)
{
    dest[0] += num;
    dest[1] += num;
}

force_inline void
vector2f_addi32(Vector2f v1, i32 num, Vector2f dest)
{
    dest[0] = v1[0] + num;
    dest[1] = v1[1] + num;
}

force_inline void
vector2f_addi32_ref(Vector2f dest, i32 num)
{
    dest[0] += num;
    dest[1] += num;
}

force_inline void
vector2f_sub(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = v1[0] - v2[0];
    dest[1] = v1[1] - v2[1];
}

force_inline void
vector2f_sub_ref(Vector2f dest, Vector2f v2)
{
    dest[0] -= v2[0];
    dest[1] -= v2[1];
}

force_inline void
vector2f_subf32(Vector2f v1, f32 num, Vector2f dest)
{
    dest[0] = v1[0] - num;
    dest[1] = v1[1] - num;
}

force_inline void
vector2f_subf32_ref(Vector2f dest, f32 num)
{
    dest[0] -= num;
    dest[1] -= num;
}

force_inline void
vector2f_subi32(Vector2f v1, i32 num, Vector2f dest)
{
    dest[0] = v1[0] - num;
    dest[1] = v1[1] - num;
}

force_inline void
vector2f_subi32_ref(Vector2f dest, i32 num)
{
    dest[0] -= num;
    dest[1] -= num;
}

force_inline void
vector2f_mul(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = v1[0] * v2[0];
    dest[1] = v1[1] * v2[1];
}

force_inline void
vector2f_mul_ref(Vector2f dest, Vector2f v2)
{
    dest[0] *= v2[0];
    dest[1] *= v2[1];
}

force_inline void
vector2f_mulf32(Vector2f v1, f32 num, Vector2f dest)
{
    dest[0] = v1[0] * num;
    dest[1] = v1[1] * num;
}

force_inline void
vector2f_mulf32_ref(Vector2f dest, f32 num)
{
    dest[0] *= num;
    dest[1] *= num;
}

force_inline void
vector2f_muli32(Vector2f v1, i32 num, Vector2f dest)
{
    dest[0] = v1[0] * num;
    dest[1] = v1[1] * num;
}

force_inline void
vector2f_muli32_ref(Vector2f dest, i32 num)
{
    dest[0] *= num;
    dest[1] *= num;
}

force_inline void
vector2f_div(Vector2f v1, Vector2f v2, Vector2f dest)
{
    dest[0] = v1[0] / v2[0];
    dest[1] = v1[1] / v2[1];
}

force_inline void
vector2f_div_ref(Vector2f dest, Vector2f v2)
{
    dest[0] /= v2[0];
    dest[1] /= v2[1];
}

force_inline void
vector2f_divf32(Vector2f v1, f32 num, Vector2f dest)
{
    dest[0] = v1[0] / num;
    dest[1] = v1[1] / num;
}

force_inline void
vector2f_divf32_ref(Vector2f dest, f32 num)
{
    dest[0] /= num;
    dest[1] /= num;
}

force_inline void
vector2f_divi32(Vector2f v1, i32 num, Vector2f dest)
{
    dest[0] = v1[0] / num;
    dest[1] = v1[1] / num;
}

force_inline void
vector2f_divi32_ref(Vector2f dest, i32 num)
{
    dest[0] /= num;
    dest[1] /= num;
}

#endif
