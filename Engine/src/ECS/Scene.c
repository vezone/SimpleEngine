#include "Scene.h"
#include "Components/PositionComponent.h"
#include "Components/SpriteComponent.h"

#include <Utils/Asset.h>

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
entity_create(Scene* scene, const char* name)
{
    EntityID entityId = ECS_ENTITY_CREATE(scene->World);
    PositionComponent p1 = PositionComponent_(((v3) { 1.0f, 1.0f, 1.0f }));

    ECS_ENTITY_ADD_COMPONENT(scene->World, entityId, PositionComponent);
    ECS_ENTITY_SET_COMPONENT(scene->World, entityId, PositionComponent, p1);

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

    if (camera != NULL)
    {
	g_IsEditorScene = 1;
	g_EditorCamera = camera;
    }

    ECS_REGISTER_COMPONENT(scene->World, PositionComponent);
    ECS_REGISTER_COMPONENT(scene->World, SpriteComponent);

    ++g_SceneLastID;
}

void
scene_on_update(Scene* scene)
{
    ECSQueryResult queryResult = ECS_ARCHETYPE_GET(scene->World, "PositionComponent,SpriteComponent");

    while (ECS_QUERY_RESULT_NEXT(queryResult))
    {
	PositionComponent* position =
	    ECS_QUERY_RESULT_GET(queryResult, PositionComponent);
	SpriteComponent* sprite =
	    ECS_QUERY_RESULT_GET(queryResult, SpriteComponent);

	if (sprite->IsTextured)
	{
	    renderer_submit_rectangle(position->Position, sprite->Size, NULL, &sprite->Texture);
	}
	else
	{
	    renderer_submit_colored_rectangle(position->Position, sprite->Size, sprite->Color);
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
