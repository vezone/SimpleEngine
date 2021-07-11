#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <Utils/Types.h>
#include <Math/MathTypes.h>
#include "Math/M4.h"

typedef struct TransformComponent
{
    m4 Transform;
} TransformComponent;

#define TransformComponent_(position, scale, rotation) _position_component(position, scale, rotation)
#define TransformComponent_Position(x, y, z) _position_component(v3_(x, y, z), v3_(1.0f, 1.0f, 1.0f), v3_(0.0f, 0.0f, 0.0f))
#define TransformComponent_PositionV3(xyz) _position_component(xyz, v3_(1.0f, 1.0f, 1.0f), v3_(0.0f, 0.0f, 0.0f))

force_inline TransformComponent
_position_component(v3 position, v3 scale, v3 rotation)
{
    TransformComponent component;
    m4_transform(position, scale, rotation, component.Transform);
    return component;
}


#endif
