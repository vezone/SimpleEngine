#ifndef SCENE_H
#define SCENE_H

#include "ECS.h"
#include <Graphics/Camera/EditorCamera.h>

typedef struct Entity
{
    const char* Name;
    EntityID ID;
} Entity;

typedef struct Scene
{
    u32 ID;
    i32 DefaultEntitiesIndex;
    World* World;
    Entity* Entities;
} Scene;

void scene_create(Scene* scene);
void scene_copy(Scene* dest, Scene* scene);
void scene_on_update(Scene* scene, const Camera* camera);
void scene_destroy(Scene* scene);

Entity scene_entity_create(Scene* scene, const char* name);
Entity scene_entity_create_default(Scene* scene);
void scene_entity_destroy(Scene* scene, Entity entity);


#endif
