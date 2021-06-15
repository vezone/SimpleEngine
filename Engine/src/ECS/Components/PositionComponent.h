#ifndef POSITION_COMPONENT_H
#define POSITION_COMPONENT_H

#include <Utils/Types.h>
#include <Math/MathTypes.h>
#include "Math/V3.h"

typedef struct PositionComponent
{
    v3 Position;
} PositionComponent;

#define PositionComponent_(position) _position_component(position)

force_inline PositionComponent
_position_component(v3 position)
{
    PositionComponent component;
    v3_assign(component.Position, position);
    return component;
}


#endif
