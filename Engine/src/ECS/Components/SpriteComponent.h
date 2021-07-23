#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <Graphics/Texture2D.h>
#include <Utils/Types.h>
#include <Math/MathTypes.h>
#include <Math/V2.h>
#include <Math/V4.h>

typedef struct SpriteComponent
{
    i8 IsTextured;
    v4 Color;
    Texture2D* Texture;
} SpriteComponent;

#define SpriteComponent(color, texture) _sprite_component(color, texture)
#define SpriteComponent_Color(color) _sprite_component_color(color)
#define SpriteComponent_Texture(texture) _sprite_component_texture(texture)

force_inline SpriteComponent
_sprite_component(v4 color, Texture2D* texture)
{
    SpriteComponent component;
    component.IsTextured = 1;
    component.Texture = texture;
    v4_assign(component.Color, color);
    return component;
}

force_inline SpriteComponent
_sprite_component_color(v4 color)
{
    SpriteComponent component;
    component.IsTextured = 0;
    component.Texture = NULL;
    v4_assign(component.Color, color);
    return component;
}

force_inline SpriteComponent
_sprite_component_texture(Texture2D* texture)
{
    SpriteComponent component;
    component.IsTextured = 1;
    component.Texture = texture;
    v4_assign(component.Color, v4_(1.0f, 1.0f, 1.0f, 1.0f));
    return component;
}

#endif
