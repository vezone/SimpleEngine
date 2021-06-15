#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <cglm/cglm.h>
#include <Graphics/Texture2D.h>
#include <Utils/Types.h>
#include <Math/V2.h>
#include <Math/V4.h>

typedef struct SpriteComponent
{
    i8 IsTextured;
    v4 Color;
    v2 Size;
    Texture2D Texture;
} SpriteComponent;

#define SpriteComponent(color, size, texture) _sprite_component(color, size, texture)
#define SpriteComponent_Color(color, size) _sprite_component_color(color, size)
#define SpriteComponent_Texture(texture, size) _sprite_component_texture(texture, size)

force_inline SpriteComponent
_sprite_component(v4 color, v2 size, Texture2D texture)
{
    SpriteComponent component;
    v4_assign(component.Color, color);
    v2_assign(component.Size, size[0], size[1]);
    component.IsTextured = 1;
    component.Texture = texture;
    return component;
}

force_inline SpriteComponent
_sprite_component_color(v4 color, v2 size)
{
    SpriteComponent component;
    component.IsTextured = 0;
    v4_assign(component.Color, color);
    v2_assign(component.Size, size[0], size[1]);
    return component;
}

force_inline SpriteComponent
_sprite_component_texture(Texture2D texture, v2 size)
{
    SpriteComponent component;
    component.IsTextured = 1;
    component.Texture = texture;
    v2_assign(component.Size, size[0], size[1]);
    return component;
}

#endif
