#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include "MathTypes.h"
#include "Utils/Types.h"


#if LINUX_PLATFORM
force_inline void
perspective(f32 near, f32 far, f32 aspect, f32 fov, m4 r)
{
    f32 viewRange = near - far;
    f32 temp1 = 1.0f / tanf(0.5f * fov);

    r[0][0] = temp1 / aspect;
    r[0][1] = 0.0f;
    r[0][2] = 0.0f;
    r[0][3] = 0.0f;

    r[1][0] = 0.0f;
    r[1][1] = temp1;
    r[1][2] = 0.0f;
    r[1][3] = 0.0f;

    r[2][0] = 0.0f;
    r[2][1] = 0.0f;
    r[2][2] = (-near - far) / viewRange;
    r[3][2] = (2.0f * near * far) / viewRange;

    r[3][0] = 0.0f;
    r[3][1] = 0.0f;
    r[3][2] = 1.0f;
    r[3][3] = 0.0f;
}
#elif WINDOWS_PLATFORM
void perspective(f32 nearVal, f32 farVal, f32 aspect, f32 fov, m4 r);
#endif
#endif
