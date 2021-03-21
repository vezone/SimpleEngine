#ifndef SCENE_H
#define SCENE_H

#include "ECS.h"
#include <Utils/Types.h>

typedef struct Scene
{
    World World;
} Scene;

typedef struct Entity
{
    char Name[128];
} Entity;

//#define entity_add_component(world, entity, component, value) ecs_set(world, entity, component, value);

Entity entity_create(Scene* scene, const char* name);
void scene_create(Scene* scene);
void scene_on_update(Scene* scene, f32 timestep);
void scene_destroy(Scene* scene);

#endif