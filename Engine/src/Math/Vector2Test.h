#ifndef VECTOR2TEST
#define VECTOR2TEST

#include "Vector2.h"

#include "Utils/Logger.h"

force_inline void
vector2f_test()
{
    Vector2f vec2Instance = {};
    vector2f_create(vec2Instance, 0.1f, 0.2f);
    PRINT("vec2Instance [%f %f]\n", vec2Instance[0], vec2Instance[1]);


    
}

#endif
