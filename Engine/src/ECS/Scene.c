#include "Scene.h"

#include <stdlib.h>
#include <time.h>

#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"

#include <Graphics/Renderer2D/Renderer2D.h>
#include <Utils/Asset.h>
#include <Math/M4IO.h>
#include <Utils/SimpleStandardLibrary.h>

//#include "Components.h"

/*
const PositionComponent *p = ecs_get(world, e, PositionComponent);

If you need a pointer that you can modify, use ecs_get_mut:
Position *p = ecs_get_mut(world, e, Position, NULL);

Components can be removed with ecs_remove:
ecs_remove(world, e, Position);

4km
*/

static u32 g_SceneLastID = 0;

//Not in use
static i32 g_IsEditorScene = 0;
static EditorCamera* g_EditorCamera = NULL;

void
scene_create(Scene* scene)
{
    // Initialize ECS
    scene->ID = g_SceneLastID;
    scene->World = world_create();
    scene->Entities = NULL;
    scene->DefaultEntitiesIndex = 0;

    ECS_REGISTER_COMPONENT(scene->World, TagComponent);
    ECS_REGISTER_COMPONENT(scene->World, TransformComponent);
    ECS_REGISTER_COMPONENT(scene->World, SpriteComponent);
    ECS_REGISTER_COMPONENT(scene->World, CameraComponent);

    ++g_SceneLastID;
}

void
scene_copy(Scene* dest, Scene* scene)
{
    dest->ID = scene->ID;
    dest->World = world_create();
    dest->Entities = NULL;
    dest->DefaultEntitiesIndex = 0;

    ECS_REGISTER_COMPONENT(dest->World, TagComponent);
    ECS_REGISTER_COMPONENT(dest->World, TransformComponent);
    ECS_REGISTER_COMPONENT(dest->World, SpriteComponent);
    ECS_REGISTER_COMPONENT(dest->World, CameraComponent);
}

#if 0
//In sandbox:
Camera* primaryCamera = NULL;
ECSQueryResult cameraQueryResult = ECS_ARCHETYPE_GET(scene->World, TagComponent,TransformComponent,CameraComponent);
while (ECS_QUERY_RESULT_NEXT(cameraQueryResult))
{
    CameraComponent* cameraComponent = ECS_QUERY_RESULT_GET(cameraQueryResult, CameraComponent);
    TransformComponent* transform = ECS_QUERY_RESULT_GET(cameraQueryResult, TransformComponent);
    if (cameraComponent->IsPrimary)
    {
	primaryCamera = &cameraComponent->Camera;
    }
}
#endif

void
scene_on_update(Scene* scene, const Camera* camera)
{
    if (!camera || camera == NULL)
	return;

    ECSQueryResult queryResult = ECS_ARCHETYPE_GET(scene->World, TagComponent,TransformComponent,SpriteComponent);

    DO_MANY_TIME(GLOG("Count: %d\n", queryResult.Count), 1);
    while (ECS_QUERY_RESULT_NEXT(queryResult))
    {
	TagComponent* tagComponent = ECS_QUERY_RESULT_GET(queryResult, TagComponent);
	TransformComponent* transform = ECS_QUERY_RESULT_GET(queryResult, TransformComponent);
	SpriteComponent* sprite = ECS_QUERY_RESULT_GET(queryResult, SpriteComponent);

	DO_MANY_TIME(GLOG("Color: %f %f %f\n", sprite->Color[0], sprite->Color[1], sprite->Color[2]), 2);
	DO_MANY_TIME(M4_PRINT(transform->Transform), 2);

	if (sprite->IsTextured)
	{
	    // GSUCCESS("%s %d %d\n", tagComponent->Name, sprite->Texture->Width, sprite->Texture->Height);
	}

	renderer_submit_rectanglet(transform->Transform, sprite->Color, sprite->Texture, tagComponent->ID);
    }
}

void
scene_destroy(Scene* scene)
{
    scene->ID = -1;

    world_destroy(scene->World);
    scene->World = NULL;
    array_free(scene->Entities);
    scene->Entities = NULL;
}

Entity
scene_entity_create(Scene* scene, const char* name)
{
    EntityID entityId = ECS_ENTITY_CREATE(scene->World);

    ECS_ENTITY_ADD_COMPONENT(scene->World, entityId, TagComponent);
    ECS_ENTITY_SET_COMPONENT(scene->World, entityId, TagComponent, TagComponent_(name, entityId));
    ECS_ENTITY_ADD_COMPONENT(scene->World, entityId, TransformComponent);
    ECS_ENTITY_SET_COMPONENT(scene->World, entityId, TransformComponent, TransformComponent_(v3_(1.0f, 1.0f, 1.0f), v3_(1.0f, 1.0f, 0.0f), v3_(0.0f, 0.0f, 0.0f)));

    Entity entity = (Entity) { .Name = name, .ID = entityId };
    array_push(scene->Entities, entity);

    return entity;
}

Entity
scene_entity_create_default(Scene* scene)
{
    char buf[128];
    sprintf(buf, "%d", scene->DefaultEntitiesIndex);
    char* name = string_concat3("Default Entity (", (const char*) buf, ")");
    const char* iName = istring(name);

    memory_free(name);

    ++scene->DefaultEntitiesIndex;

    Entity entity = scene_entity_create(scene, iName);

    return entity;
}

void
scene_entity_destroy(Scene* scene, Entity entity)
{
    i32 i, count = array_count(scene->Entities);
    for (i = 0; i < count; ++i)
    {
	Entity en = scene->Entities[i];
	if (en.ID == entity.ID)
	{
	    break;
	}
    }

#if 0
    --count;

    for (i = i; i < count; ++i)
    {
	scene->Entities[i] = scene->Entities[i + 1];
    }
#endif

    size_t addr = (size_t) scene->Entities;
    i32 delCount = count - i;

    array_remove(scene->Entities, i);

    ECS_ENTITY_DESTROY(scene->World, entity.ID);
}
