#ifndef V4_H
#define V4_H

#include "MathTypes.h"

force_inline void
v4_assign(v4 v, v4 values)
{
    v[0] = values[0];
    v[1] = values[1];
    v[2] = values[2];
    v[3] = values[3];
}

#endif
