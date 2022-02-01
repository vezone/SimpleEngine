#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <Math/MathTypes.h>
#include "Math/M4.h"

typedef struct TransformComponent
{
    m4 Transform;
} TransformComponent;

#define TransformComponent_(position, scale, rotation) _transform_from_tsr(position, scale, rotation)
#define TransformComponent_Position(x, y, z) _transform_from_tsr(v3_(x, y, z), v3_(1.0f, 1.0f, 1.0f), v3_(0.0f, 0.0f, 0.0f))
#define TransformComponent_PositionV3(xyz) _transform_from_tsr(xyz, v3_(1.0f, 1.0f, 1.0f), v3_(0.0f, 0.0f, 0.0f))

#define TransformComponent_Array(arr) _transform_from_array(arr)

force_inline TransformComponent
_transform_from_tsr(v3 translation, v3 scale, v3 rotation)
{
    TransformComponent component;
    m4_transform(translation, scale, rotation, component.Transform);
    return component;
}

force_inline TransformComponent
_transform_from_array(f32* arr)
{
    TransformComponent component;
    component.Transform[0][0] = arr[0];
    component.Transform[0][1] = arr[1];
    component.Transform[0][2] = arr[2];
    component.Transform[0][3] = arr[3];

    component.Transform[1][0] = arr[4];
    component.Transform[1][1] = arr[5];
    component.Transform[1][2] = arr[6];
    component.Transform[1][3] = arr[7];

    component.Transform[2][0] = arr[8];
    component.Transform[2][1] = arr[9];
    component.Transform[2][2] = arr[10];
    component.Transform[2][3] = arr[11];

    component.Transform[3][0] = arr[12];
    component.Transform[3][1] = arr[13];
    component.Transform[3][2] = arr[14];
    component.Transform[3][3] = arr[15];

    return component;
}

#endif
