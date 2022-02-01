#ifndef ORTHOGRAPHIC_H
#define ORTHOGRAPHIC_H

#include "MathTypes.h"

force_inline void
orthographic(f32 left, f32 right, f32 bot, f32 top, f32 nearVal, f32 farVal, m4 dest)
{
    f32 rightLeft = right - left;
    f32 topBot = top - bot;
    f32 farNear = farVal - nearVal;

    dest[0][0] =  2.0f / rightLeft;
    dest[1][1] =  2.0f / topBot;
    dest[2][2] = -2.0f / farNear;
    dest[3][3] =  1.0f;

    dest[3][0] = -(right + left)   / rightLeft;
    dest[3][1] = -(top   + bot) / topBot;
    dest[3][2] = -(farVal   + nearVal)   / farNear;
}

#endif
