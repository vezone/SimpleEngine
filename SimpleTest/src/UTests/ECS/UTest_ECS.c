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

    va_start(components, number);

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

void ecs_init_test()
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

    u32 aId = 0;
    u32 bId = 0;
    ECS_GET_COMPONENT_ID_BY_TYPE(&world, A, &aId);
    ECS_GET_COMPONENT_ID_BY_TYPE(&world, B, &bId);

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

    GWARNING("Entity Player Id: %ld\n", playerId);
}

void ecs_test()
{
    TEST(ecs_init_test());
}
