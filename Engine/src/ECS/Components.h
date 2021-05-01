#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <cglm/cglm.h>
#include "Graphics/Texture2D.h"

typedef struct PositionComponent
{
    mat4 Transform;
} PositionComponent;

typedef struct SpriteComponent
{
    vec4 Color;
    Texture2D Texture;
} SpriteComponent;


#define PositionComponent(transform) ((PositionComponent) { .Transform = transform })
#define SpriteComponent(color) ((SpriteComponent) { .Color = color })

#endif
