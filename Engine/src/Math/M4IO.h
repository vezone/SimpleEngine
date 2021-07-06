#ifndef M4IO_H
#define M4IO_H

#include "MathTypes.h"
#include "Utils/Logger.h"

force_inline void
m4_print(m4 m, const char* name)
{
    char sb[511];
    sprintf(sb, "%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n", m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);

    printf("M4 ("GREEN("%s")"): \n%s", name, sb);
}

#define M4_PRINT(m) m4_print(m, #m)

#endif
