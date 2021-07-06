#ifndef M4_H
#define M4_H

#include "MathTypes.h"
#include "BaseMath.h"
#include "V3.h"
#include "V4.h"

/* Base operation */

force_inline void
m4_copy(m4 a, m4 b)
{
    b[0][0] = a[0][0]; b[0][1] = a[0][1]; b[0][2] = a[0][2]; b[0][3] = a[0][3];
    b[1][0] = a[1][0]; b[1][1] = a[1][1]; b[1][2] = a[1][2]; b[1][3] = a[1][3];
    b[2][0] = a[2][0]; b[2][1] = a[2][1]; b[2][2] = a[2][2]; b[2][3] = a[2][3];
    b[3][0] = a[3][0]; b[3][1] = a[3][1]; b[3][2] = a[3][2]; b[3][3] = a[3][3];
}

force_inline void
m4_set_identity(m4 m)
{
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;

    m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = 0; m[1][2] = 0; m[1][3] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0;
}

force_inline void
m4_mul_v4(m4 m, v4 v, v4 r)
{
    r[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    r[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    r[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    r[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
}

force_inline void
m4_mul_v3(m4 m, v3 v, v3 r)
{
    v4 r4;
    v4 xyzw = v4_(v[0], v[1], v[2], 1.0f);
    m4_mul_v4(m, xyzw, r4);

    r[0] = r4[0];
    r[1] = r4[1];
    r[2] = r4[2];
}

force_inline void
m4_mul(m4 m1, m4 m2, m4 r)
{
    /*
      Column major order for OpenGL

      1 2 3 4     1 2 3 4
      2 2 3 3  *  2 2 3 3
      4 4 2 1     4 4 2 1
      3 3 1 4     3 3 1 4

     */

    f32 l00 = m1[0][0], l01 = m1[0][1], l02 = m1[0][2], l03 = m1[0][3],
	l10 = m1[1][0], l11 = m1[1][1], l12 = m1[1][2], l13 = m1[1][3],
	l20 = m1[2][0], l21 = m1[2][1], l22 = m1[2][2], l23 = m1[2][3],
	l30 = m1[3][0], l31 = m1[3][1], l32 = m1[3][2], l33 = m1[3][3],

	r00 = m2[0][0], r01 = m2[0][1], r02 = m2[0][2], r03 = m2[0][3],
	r10 = m2[1][0], r11 = m2[1][1], r12 = m2[1][2], r13 = m2[1][3],
	r20 = m2[2][0], r21 = m2[2][1], r22 = m2[2][2], r23 = m2[2][3],
	r30 = m2[3][0], r31 = m2[3][1], r32 = m2[3][2], r33 = m2[3][3];

    // column
    r[0][0] = l00 * r00 + l10 * r01 + l20 * r02 + l30 * r03;
    r[0][1] = l01 * r00 + l11 * r01 + l21 * r02 + l31 * r03;
    r[0][2] = l02 * r00 + l12 * r01 + l22 * r02 + l32 * r03;
    r[0][3] = l03 * r00 + l13 * r01 + l23 * r02 + l33 * r03;

    r[1][0] = l00 * r10 + l10 * r11 + l20 * r12 + l30 * r13;
    r[1][1] = l01 * r10 + l11 * r11 + l21 * r12 + l31 * r13;
    r[1][2] = l02 * r10 + l12 * r11 + l22 * r12 + l32 * r13;
    r[1][3] = l03 * r10 + l13 * r11 + l23 * r12 + l33 * r13;

    r[2][0] = l00 * r20 + l10 * r21 + l20 * r22 + l30 * r23;
    r[2][1] = l01 * r20 + l11 * r21 + l21 * r22 + l31 * r23;
    r[2][2] = l02 * r20 + l12 * r21 + l22 * r22 + l32 * r23;
    r[2][3] = l03 * r20 + l13 * r21 + l23 * r22 + l33 * r23;

    r[3][0] = l00 * r30 + l10 * r31 + l20 * r32 + l30 * r33;
    r[3][1] = l01 * r30 + l11 * r31 + l21 * r32 + l31 * r33;
    r[3][2] = l02 * r30 + l12 * r31 + l22 * r32 + l32 * r33;
    r[3][3] = l03 * r30 + l13 * r31 + l23 * r32 + l33 * r33;
}

force_inline void
m4_mul_m4(m4 r, m4 m1, m4 m2)
{
    /*
      Column major order for OpenGL

      1 2 3 4     1 2 3 4
      2 2 3 3  *  2 2 3 3
      4 4 2 1     4 4 2 1
      3 3 1 4     3 3 1 4

     */

    f32 l00 = m1[0][0], l01 = m1[0][1], l02 = m1[0][2], l03 = m1[0][3],
	l10 = m1[1][0], l11 = m1[1][1], l12 = m1[1][2], l13 = m1[1][3],
	l20 = m1[2][0], l21 = m1[2][1], l22 = m1[2][2], l23 = m1[2][3],
	l30 = m1[3][0], l31 = m1[3][1], l32 = m1[3][2], l33 = m1[3][3],

	r00 = m2[0][0], r01 = m2[0][1], r02 = m2[0][2], r03 = m2[0][3],
	r10 = m2[1][0], r11 = m2[1][1], r12 = m2[1][2], r13 = m2[1][3],
	r20 = m2[2][0], r21 = m2[2][1], r22 = m2[2][2], r23 = m2[2][3],
	r30 = m2[3][0], r31 = m2[3][1], r32 = m2[3][2], r33 = m2[3][3];

    // column
    r[0][0] = l00 * r00 + l10 * r01 + l20 * r02 + l30 * r03;
    r[0][1] = l01 * r00 + l11 * r01 + l21 * r02 + l31 * r03;
    r[0][2] = l02 * r00 + l12 * r01 + l22 * r02 + l32 * r03;
    r[0][3] = l03 * r00 + l13 * r01 + l23 * r02 + l33 * r03;

    r[1][0] = l00 * r10 + l10 * r11 + l20 * r12 + l30 * r13;
    r[1][1] = l01 * r10 + l11 * r11 + l21 * r12 + l31 * r13;
    r[1][2] = l02 * r10 + l12 * r11 + l22 * r12 + l32 * r13;
    r[1][3] = l03 * r10 + l13 * r11 + l23 * r12 + l33 * r13;

    r[2][0] = l00 * r20 + l10 * r21 + l20 * r22 + l30 * r23;
    r[2][1] = l01 * r20 + l11 * r21 + l21 * r22 + l31 * r23;
    r[2][2] = l02 * r20 + l12 * r21 + l22 * r22 + l32 * r23;
    r[2][3] = l03 * r20 + l13 * r21 + l23 * r22 + l33 * r23;

    r[3][0] = l00 * r30 + l10 * r31 + l20 * r32 + l30 * r33;
    r[3][1] = l01 * r30 + l11 * r31 + l21 * r32 + l31 * r33;
    r[3][2] = l02 * r30 + l12 * r31 + l22 * r32 + l32 * r33;
    r[3][3] = l03 * r30 + l13 * r31 + l23 * r32 + l33 * r33;
}

force_inline void
m4_mul_rot(m4 a, m4 b, m4 dest)
{
    f32 a00 = a[0][0], a01 = a[0][1], a02 = a[0][2], a03 = a[0][3],
	a10 = a[1][0], a11 = a[1][1], a12 = a[1][2], a13 = a[1][3],
	a20 = a[2][0], a21 = a[2][1], a22 = a[2][2], a23 = a[2][3],
	a30 = a[3][0], a31 = a[3][1], a32 = a[3][2], a33 = a[3][3],

	b00 = b[0][0], b01 = b[0][1], b02 = b[0][2],
	b10 = b[1][0], b11 = b[1][1], b12 = b[1][2],
	b20 = b[2][0], b21 = b[2][1], b22 = b[2][2];

    dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02;
    dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02;
    dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02;
    dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02;

    dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12;
    dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12;
    dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12;
    dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12;

    dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22;
    dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22;
    dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22;
    dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22;

    dest[3][0] = a30;
    dest[3][1] = a31;
    dest[3][2] = a32;
    dest[3][3] = a33;
}

/* Object transformation operation */

force_inline void
m4_translate(m4 m, v3 translation)
{
    m[3][0] = translation[0];
    m[3][1] = translation[1];
    m[3][2] = translation[2];
}

force_inline void
m4_scale_v3(m4 m, v3 v, m4 dest)
{
    v4_scale(m[0], v[0], dest[0]);
    v4_scale(m[1], v[1], dest[1]);
    v4_scale(m[2], v[2], dest[2]);
    v4_copy(m[3], dest[3]);
}

force_inline void
m4_rotate_x(m4 m, f32 rad, m4 dest)
{
    m4 identity = M4_IDENTITY;
    f32 cosV, sinV;
    cosV = cos(rad);
    sinV = sin(rad);
    identity[1][1] =  cosV;
    identity[1][2] =  sinV;
    identity[2][1] = -sinV;
    identity[2][2] =  cosV;

    m4_mul_rot(m, identity, dest);
}

force_inline void
m4_rotate_y(m4 m, f32 rad, m4 dest)
{
    m4 identity = M4_IDENTITY;
    f32 cosV, sinV;
    cosV = cos(rad);
    sinV = sin(rad);

    identity[0][0] =  cosV;
    identity[0][2] = -sinV;
    identity[2][0] =  sinV;
    identity[2][2] =  cosV;

    m4_mul_rot(m, identity, dest);
}

force_inline void
m4_rotate_z(m4 m, f32 rad, m4 dest)
{
    m4 identity = M4_IDENTITY;
    f32 cosV, sinV;
    cosV = cos(rad);
    sinV = sin(rad);

    identity[0][0] =  cosV;
    identity[0][1] =  sinV;
    identity[1][0] = -sinV;
    identity[1][1] =  cosV;

    m4_mul_rot(m, identity, dest);
}

force_inline void
m4_transform(v3 position, v3 scale, v3 rotation, m4 transform)
{
    m4_set_identity(transform);

    m4 translationMat = M4_IDENTITY;
    m4 rotationMat = M4_IDENTITY;
    m4 scaleMat = M4_IDENTITY;
    v3 scaleVec = v3_(scale[0], scale[1], 1.0f);

    m4_translate(translationMat, position);
    m4_rotate_z(rotationMat, rad(rotation[2]), rotationMat);
    m4_scale_v3(scaleMat, scaleVec, scaleMat);

    m4_mul(translationMat, rotationMat, transform);
    m4_mul(transform, scaleMat, transform);
}

force_inline i32
m4_transform_decompose(m4 transform, v4 translation, v3 rotation, v3 scale)
{
    f32 zero = 0.0f;
    f32 one = 1.0f;
    m4 localMatrix;
    m4_copy(transform, localMatrix);

    // Normalize the matrix.
    if (f32_equal(localMatrix[3][3], zero))
    {
	return 0;
    }

    // First, isolate perspective.  This is the messiest.
    i32 shouldIsolatePerspective = (!f32_equal(localMatrix[0][3], zero) ||	!f32_equal(localMatrix[1][3], zero) || !f32_equal(localMatrix[2][3], zero));
    if (shouldIsolatePerspective)
    {
	// Clear the perspective partition
	localMatrix[0][3] = (f32) 0;
	localMatrix[1][3] = (f32) 0;
	localMatrix[2][3] = (f32) 0;
	localMatrix[3][3] = (f32) 1;
    }

    // Next take care of translation (easy).
    translation[0] = localMatrix[3][0];
    translation[1] = localMatrix[3][1];
    translation[2] = localMatrix[3][2];
    translation[3] = localMatrix[3][3];

    v3 row[3];
    // Now get scale and shear.
    for (i32 i = 0; i < 3; ++i)
	for (i32 j = 0; j < 3; ++j)
	    row[i][j] = localMatrix[i][j];


    // TODO(bies): fix it
    // Compute X scale factor and normalize first row.
    // scale[0] = length(row[0]);
    scale[0] = v3_length(row[0]);
    //row[0] = detail::scale(row[0], one);
    scale[1] = v3_length(row[1]);
    // row[1] = detail::scale(row[1], one);
    scale[2] = v3_length(row[2]);
    // row[2] = detail::scale(row[2], one);
    // TODO(bies): fix it


    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
    v3 Pdum3 = cross(row[1], row[2]); // v3Cross(row[1], row[2], Pdum3);
    if (dot(row[0], Pdum3) < 0)
    {
	for (i32 i = 0; i < 3; i++)
	{
	    scale[i] *= f32(-1);
	    row[i]   *= f32(-1);
	}
    }
#endif

    rotation[1] = asin(-row[0][2]);
    if (cos(rotation[1]) != 0)
    {
	rotation[0] = atan2(row[1][2], row[2][2]);
	rotation[2] = atan2(row[0][1], row[0][0]);
    }
    else
    {
	rotation[0] = atan2(-row[2][0], row[1][1]);
	rotation[2] = 0;
    }

    return 1;
}

#endif
