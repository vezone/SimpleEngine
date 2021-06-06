#ifndef V3_H
#define V3_H

#include "MathTypes.h"

force_inline void
v3_assign(v3 v, v3 values)
{
    v[0] = values[0];
    v[1] = values[1];
    v[2] = values[2];
}

#endif
