#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <cglm/cglm.h>
#include "Graphics/Texture2D.h"
#include "Math/V2.h"
#include "Math/V3.h"
#include "Math/V4.h"

typedef struct PositionComponent
{
    v3 Position;
} PositionComponent;

typedef struct SpriteComponent
{
    i8 IsTextured;
    v4 Color;
    v2 Size;
    Texture2D Texture;
} SpriteComponent;

#define PositionComponent_(position) internal_position_component(position)

#define SpriteComponent(color, size, texture) internal_sprite_component(color, size, texture)
#define SpriteComponent_Color(color, size) internal_sprite_component_color(color, size)
#define SpriteComponent_Texture(texture, size) internal_sprite_component_texture(texture, size)

force_inline PositionComponent
internal_position_component(v3 position)
{
    PositionComponent component;
    v3_assign(component.Position, position);
    return component;
}

force_inline SpriteComponent
internal_sprite_component(v4 color, v2 size, Texture2D texture)
{
    SpriteComponent component;
    v4_assign(component.Color, color);
    v2_assign(component.Size, size);
    component.IsTextured = 1;
    component.Texture = texture;
    return component;
}

force_inline SpriteComponent
internal_sprite_component_color(v4 color, v2 size)
{
    SpriteComponent component;
    component.IsTextured = 0;
    v4_assign(component.Color, color);
    v2_assign(component.Size, size);
    return component;
}

force_inline SpriteComponent
internal_sprite_component_texture(Texture2D texture, v2 size)
{
    SpriteComponent component;
    component.IsTextured = 1;
    component.Texture = texture;
    v2_assign(component.Size, size);
    return component;
}

#endif
