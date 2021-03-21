#include "Scene.h"
#include "Components.h"


//#include "Components.h"

/*
const PositionComponent *p = ecs_get(world, e, PositionComponent);

If you need a pointer that you can modify, use ecs_get_mut:
Position *p = ecs_get_mut(world, e, Position, NULL);

Components can be removed with ecs_remove:
ecs_remove(world, e, Position);

4km
*/

Entity entity_create(Scene* scene, const char* name)
{
#if 0
    ecs_world_t* world = scene->World;
    ecs_entity_t entity = ecs_new(world, 0);
    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    Entity result;
    result.FlecsEntity = entity;

    PositionComponent posComponent;
    glm_mat4_copy(transform, posComponent.Transform);
    ECS_ENTITY(world, MyEntity, 0);
    entity = ecs_set(world, MyEntity, PositionComponent, posComponent);
    ecs_set(world, entity, PositionComponent, posComponent);

    u32 length = vstring_length(name);
    vstring_assign(result.Name, name, length);

    return result;
#endif
    return (Entity) { };
}

void scene_create(Scene* scene)
{
    world_init(&scene->World);
    ECS_REGISTER_COMPONENT(&scene->World, PositionComponent);
    ECS_REGISTER_COMPONENT(&scene->World, SpriteComponent);
}

void scene_on_update(Scene* scene, f32 timestep)
{
#if 0
    //get all entities iter
    ecs_query_t *query = ecs_query_new(scene->World, "PositionComponent, SpriteComponent");
    ecs_iter_t it = ecs_query_iter(query);

    while (ecs_query_next(&it))
    {
	PositionComponent* position = ecs_column(&it, PositionComponent, 1);
	SpriteComponent* sprite = ecs_column(&it, SpriteComponent, 2);
	//
    }
#endif
}

void scene_destroy(Scene* scene)
{
    //ecs_fini(scene->World);
}
