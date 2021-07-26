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
static i32 g_IsEditorScene = 0;
static EditorCamera* g_EditorCamera = NULL;
static Entity* g_Entities = NULL;

Entity
entity_create_with_random_name(Scene* scene)
{
    // length: 16
    char name[] = "NameToBeModified";
    for (i32 i = 0; i < 16; i++)
    {
	name[i] = (char) (rand() % ('z' + 1 - 'a') + 'a');
    }

    const char* iName = istring(name);
    Entity entity = entity_create(scene, iName);
    return entity;
}

Entity
entity_create(Scene* scene, const char* name)
{
    EntityID entityId = ECS_ENTITY_CREATE(scene->World);

    ECS_ENTITY_ADD_COMPONENT(scene->World, entityId, TagComponent);
    ECS_ENTITY_SET_COMPONENT(scene->World, entityId, TagComponent, TagComponent_(name, entityId));
    ECS_ENTITY_ADD_COMPONENT(scene->World, entityId, TransformComponent);
    ECS_ENTITY_SET_COMPONENT(scene->World, entityId, TransformComponent, TransformComponent_(v3_(1.0f, 1.0f, 1.0f), v3_(1.0f, 1.0f, 0.0f), v3_(0.0f, 0.0f, 0.0f)));

    Entity entity = (Entity) { .Name = name, .ID = entityId };
    array_push(g_Entities, entity);

    return entity;
}

void
scene_create(Scene* scene, EditorCamera* camera)
{
    // Initialize ECS
    scene->ID = g_SceneLastID;
    scene->World = world_create();

    ECS_REGISTER_COMPONENT(scene->World, TagComponent);
    ECS_REGISTER_COMPONENT(scene->World, TransformComponent);
    ECS_REGISTER_COMPONENT(scene->World, SpriteComponent);
    ECS_REGISTER_COMPONENT(scene->World, CameraComponent);

    ++g_SceneLastID;
}

void
scene_on_update(Scene* scene)
{
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

    if (primaryCamera)
    {
	ECSQueryResult queryResult = ECS_ARCHETYPE_GET(scene->World, TagComponent,TransformComponent,SpriteComponent);

	PRINT_SINGLE("Count: %d\n", queryResult.Count);
	while (ECS_QUERY_RESULT_NEXT(queryResult))
	{
	    TagComponent* tagComponent = ECS_QUERY_RESULT_GET(queryResult, TagComponent);
	    TransformComponent* transform = ECS_QUERY_RESULT_GET(queryResult, TransformComponent);
	    SpriteComponent* sprite = ECS_QUERY_RESULT_GET(queryResult, SpriteComponent);

	    PRINT_MANY(2, "Color: %f %f %f\n", sprite->Color[0], sprite->Color[1], sprite->Color[2]);

	    DO_MANY_TIME(M4_PRINT(transform->Transform), 2);

	    renderer_submit_rectanglet(transform->Transform, sprite->Color, sprite->Texture, tagComponent->ID);
	}
    }
}

void
scene_destroy(Scene* scene)
{
}

Entity*
scene_get_entities()
{
    return g_Entities;
}
