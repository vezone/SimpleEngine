#ifndef SCENE_H
#define SCENE_H

#include "ECS.h"
#include <Graphics/Renderer2D/EditorCamera.h>
#include <Utils/Types.h>

typedef struct Scene
{
    u32 ID;
    World* World;
} Scene;

typedef struct Entity
{
    const char* Name;
    EntityID ID;
} Entity;

//#define entity_add_component(world, entity, component, value) ecs_set(world, entity, component, value);
Entity entity_create(Scene* scene, const char* name);
void scene_create(Scene* scene, EditorCamera* camera);
void scene_on_update(Scene* scene);
void scene_destroy(Scene* scene);

Entity* scene_get_entities();

#endif
