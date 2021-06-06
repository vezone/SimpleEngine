#include "Scene.h"
#include "Components.h"
#include "Graphics/Renderer2D/Renderer2D.h"

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

Entity
entity_create(Scene* scene, const char* name)
{
    EntityID entityId = ECS_ENTITY_CREATE(scene->World);
    return (Entity) { .Name = name, .ID = entityId };
}

void
scene_create(Scene* scene)
{
    scene->ID = g_SceneLastID;
    scene->World = world_create();
    ECS_REGISTER_COMPONENT(scene->World, PositionComponent);
    ECS_REGISTER_COMPONENT(scene->World, SpriteComponent);
}

void
scene_on_update(Scene* scene, f32 timestep)
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
