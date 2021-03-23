#include "UTest_ECS.h"

#include "UTests/Test.h"
#include "ECS/ECS.h"
#include "ECS/Components.h"
#include "Utils/Logger.h"
#include "Utils/String.h"

typedef struct A
{
    u32 AId;
    u32 AAge;
} A;

typedef struct B
{
    u32 BId;
    u32 BAge;
} B;

typedef struct C
{
    u32 CId;
    u32 CAge;
} C;

typedef struct D
{
    u32 CId;
    u32 CAge;
} D;

//NOTE(bies): we should check if component exist in talbe, if it not assert(0) it
//NOTE(bies): return array<void*> for each component from components
u64*
get_component_data(World* world, void* data, i32 count, ...)
{
    va_list components;
    u64 size = 0;
    u32 number;
    u64* result = NULL;

    ComponentStorage* storage = &world->Storage;

    va_start(components, count);

    for (i32 i = 0; i < count; i++)
    {
	u32 componentId = va_arg(components, u32);
	ComponentData componentData = hmget(storage->Components, componentId);
	u64 ptr = u64(data) + size;
	array_push(result, ptr);
	GWARNING("Size: %lu, Ptr: %ld\n", size, ptr);
	size += componentData.Size;
    }

    va_end(components);

    return result;
}

/*

    World world;
    world_init(&world);
    ECS_REGISTER_COMPONENT(&world, PositionComponent);
    ECS_REGISTER_COMPONENT(&world, SpriteComponent);


    u64 entityId;
    ECS_ENTITY_CREATE(&world, &entityId);
    ECS_ENTITY_ADD_COMPONENT(&world, entityId, PositionComponent);
    ECS_ENTITY_ADD_COMPONENT(&world, entityId, SpriteComponent);

    for now public api contains COMPONENT_GET_ID_BY_TYPE(world, SpriteComponent ) function

    ECS_ENTITY_REMOVE_COMPONENT(&world, entityId, COMPONENT_GET_ID_BY_TYPE(SpriteComponent));

    ECS_ENTITY_SET_COMPONENT(&world, entityId, COMPONENT_GET_ID_BY_TYPE(SpriteComponent), SOME_VALUE);

    PositionComponent* positionComponent = ECS_ENTITY_GET(&world, entityId, PositionComponent);

    ECS_QUERY(&world, "PositionComponent, SpriteComponent");

*/

void
world_init_test()
{
    World world;
    world_init(&world);

    Condition(world.Id == 0);
    Condition(world.LastEntityId == ENTITIES_FIRST_ID);
    Condition(world.Archetypes == NULL);
    Condition(world.ArchetypesStorage == NULL);
    Condition(world.Storage.NameToId == NULL);
    Condition(world.Storage.IdToName == NULL);
    Condition(world.Storage.IdToSize == NULL);
    Condition(world.Storage.Components == NULL);
    Condition(world.Storage.LastId == COMPONENTS_FIRST_ID);
}

void
ecs_register_component_test()
{
    World world;
    world_init(&world);
    ECS_REGISTER_COMPONENT(&world, A);
    ECS_REGISTER_COMPONENT(&world, B);
    ECS_REGISTER_COMPONENT(&world, C);
    ECS_REGISTER_COMPONENT(&world, D);

    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(&world, A) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(&world, B) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(&world, C) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(&world, D) == 1);
}

void
ecs_get_component_id_by_type_test()
{
    World world;
    world_init(&world);
    ECS_REGISTER_COMPONENT(&world, A);
    ECS_REGISTER_COMPONENT(&world, B);

    u32 aId = ECS_GET_COMPONENT_ID_BY_TYPE(&world, A);
    u32 bId = ECS_GET_COMPONENT_ID_BY_TYPE(&world, B);

    Condition(ECS_GET_COMPONENT_ID_BY_TYPE(&world, A) == COMPONENTS_FIRST_ID);
    Condition(ECS_GET_COMPONENT_ID_BY_TYPE(&world, B) == (COMPONENTS_FIRST_ID + 1));
}

void
ecs_entity_add_component_test()
{
    World world;
    world_init(&world);
    ECS_REGISTER_COMPONENT(&world, A);
    ECS_REGISTER_COMPONENT(&world, B);

    u64 playerId;
    ECS_ENTITY_CREATE(&world, &playerId);

    Condition(playerId == ENTITIES_FIRST_ID);
    Condition(ECS_IS_ENTITY_REGISTERED(&world, playerId) == 1);

    Condition(ENTITY_HAS_COMPONENT(&world, playerId, A) != 1);
    Condition(ENTITY_HAS_COMPONENT(&world, playerId, B) != 1);

    ECS_ENTITY_ADD_COMPONENT(&world, playerId, A);
    Condition(ENTITY_HAS_COMPONENT(&world, playerId, A) == 1);
    ECS_ENTITY_ADD_COMPONENT(&world, playerId, B);
    Condition(ENTITY_HAS_COMPONENT(&world, playerId, B) == 1);


    i32* components = _ecs_entity_get_components(&world, playerId);
}

//TODO(bies): need to check archetype creation
void
ecs_archetype_check()
{
#if 0
    //TODO(bies): remove this in the future
    //CHECK
    i32 isAdded = hmgeti(world->ArchetypesStorage, entityId);
    GERROR("IS ADDED: %d\n", isAdded);
    ComponentID* comps = _ecs_entity_get_components(world, entityId);
    GERROR("Component's: %s\n", comps == NULL?"NULL":"NOT NULL");
    for (i32 i = 0; i < array_count(comps); i++)
    {
	GERROR("Component: %d\n", comps[i]);
    }
#endif
}

void
ecs_init_test()
{
    World world;
    world_init(&world);
    ECS_REGISTER_COMPONENT(&world, PositionComponent);
    ECS_REGISTER_COMPONENT(&world, SpriteComponent);

    void* data = memory_allocate(sizeof(A) + sizeof(B));

    ECS_REGISTER_COMPONENT(&world, A);
    ECS_REGISTER_COMPONENT(&world, B);
    A a = (A) { .AId = 37, .AAge = 23 };
    B b = (B) { .BId = 1337, .BAge = 1223 };
    memcpy(data, &a, sizeof(A));
    memcpy(data + sizeof(A), &b, sizeof(B));

    u32 aId = ECS_GET_COMPONENT_ID_BY_TYPE(&world, A);
    u32 bId = ECS_GET_COMPONENT_ID_BY_TYPE(&world, B);

    GWARNING("AId: %d, BId: %d\n", aId, bId);

    u64* result = get_component_data(&world, data, 2, aId, bId);
    {
	A* a = (A*) result[0];
	B* b = (B*) result[1];
	GWARNING("a.AId: %d, a.AAge: %d\n", a->AId, a->AAge);
	GWARNING("b.BId: %d, b.BAge: %d\n", b->BId, b->BAge);
    }

    u64 playerId;
    ECS_ENTITY_CREATE(&world, &playerId);
    ECS_ENTITY_ADD_COMPONENT(&world, playerId, A);

    i32* components = _ecs_entity_get_components(&world, playerId);
    GERROR("component's count: %d\n", array_count(components));
    for (i32 i = 0; i < array_count(components); i++)
    {
	char* name = ECS_GET_COMPONENT_NAME_BY_ID(&world, components[i]);
	GWARNING("component id: %d, name: %s\n", components[i], name);
    }

    GWARNING("Entity Player Id: %ld\n", playerId);
}

void ecs_test()
{
    TEST(world_init_test());
    TEST(ecs_register_component_test());
    TEST(ecs_get_component_id_by_type_test());
    TEST(ecs_entity_add_component_test());

    TEST(ecs_archetype_check());

    TEST(ecs_init_test());
}
