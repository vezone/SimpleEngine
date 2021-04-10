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
    u64 offset = 0;
    u32 number;
    u64* result = NULL;

    ComponentStorage* storage = &world->Storage;

    va_start(components, count);

    for (i32 i = 0; i < count; i++)
    {
	u32 componentId = va_arg(components, u32);
	u32 componentSize = hmget(storage->IdToSize, componentId);
	u64 ptr = u64(data) + offset;
	array_push(result, ptr);
	GWARNING("Offset: %lu, Ptr: %ld\n", offset, ptr);
	offset += componentSize;
    }

    va_end(components);

    return result;
}

/*

    World world;
    world_create(&world);
    ECS_REGISTER_COMPONENT(&world, PositionComponent);
    ECS_REGISTER_COMPONENT(&world, SpriteComponent);


    u64 entityId = ECS_ENTITY_CREATE(&world);
    ECS_ENTITY_ADD_COMPONENT(&world, entityId, PositionComponent);
    ECS_ENTITY_ADD_COMPONENT(&world, entityId, SpriteComponent);

    for now public api contains COMPONENT_GET_ID_BY_TYPE(world, SpriteComponent ) function

    ECS_ENTITY_REMOVE_COMPONENT(&world, entityId, COMPONENT_GET_ID_BY_TYPE(SpriteComponent));

    ECS_ENTITY_SET_COMPONENT(&world, entityId, COMPONENT_GET_ID_BY_TYPE(SpriteComponent), SOME_VALUE);

    PositionComponent* positionComponent = ECS_ENTITY_GET(&world, entityId, PositionComponent);

    ECS_QUERY(&world, "PositionComponent, SpriteComponent");

*/

void
world_create_test()
{
    World* world = world_create();

    Condition(world->Id == 0);
    Condition(world->LastEntityId == ENTITIES_FIRST_ID);
    Condition(world->Archetypes == NULL);
    Condition(world->ArchetypesStorage == NULL);
    Condition(world->Storage.NameToId == NULL);
    Condition(world->Storage.IdToName == NULL);
    Condition(world->Storage.IdToSize == NULL);
    Condition(world->Storage.LastId == COMPONENTS_FIRST_ID);
}

void
ecs_register_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);
    ECS_REGISTER_COMPONENT(world, C);
    ECS_REGISTER_COMPONENT(world, D);

    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, A) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, B) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, C) == 1);
    Condition(ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, D) == 1);
}

void
ecs_get_component_id_by_type_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u32 aId = ECS_GET_COMPONENT_ID_BY_TYPE(world, A);
    u32 bId = ECS_GET_COMPONENT_ID_BY_TYPE(world, B);

    Condition(ECS_GET_COMPONENT_ID_BY_TYPE(world, A) == COMPONENTS_FIRST_ID);
    Condition(ECS_GET_COMPONENT_ID_BY_TYPE(world, B) == (COMPONENTS_FIRST_ID + 1));
}

void
ecs_entity_add_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u64 playerId = ECS_ENTITY_CREATE(world);

    Condition(playerId == ENTITIES_FIRST_ID);
    Condition(WORLD_HAS_ENTITY(world, playerId) == 1);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) != 1);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) != 1);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A));
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B));

    i32* components = _ecs_entity_get_components_id(world, playerId);
    for (i32 i = 0; i < array_count(components); i++)
    {
	Int_Value(components[i]);
    }
}

void
ecs_entity_get_components_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u64 playerId = ECS_ENTITY_CREATE(world);

    Condition(playerId == ENTITIES_FIRST_ID);
    Condition(WORLD_HAS_ENTITY(world, playerId) == 1);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) == 0);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) == 0);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) == 1);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) == 1);

    ComponentID* components = _ecs_entity_get_components_id(world, playerId);
    Condition(array_count(components) == 2);
    Condition(components[0] == COMPONENTS_FIRST_ID);
    Condition(components[1] == COMPONENTS_FIRST_ID + 1);

    const char* componentName = _ecs_get_component_name_by_id(world, components[0]);
    String_Value(componentName);
    String_IsEquals(componentName, "A");

    componentName = _ecs_get_component_name_by_id(world, components[1]);
    String_Value(componentName);
    String_IsEquals(componentName, "B");

    ArchetypeRecord record = _ecs_get_archetype_record(world, playerId);
    Condition(record.Archetype->ComponentsSize == (sizeof(A) + sizeof(B)));
    Int_Value(record.Archetype->ComponentsSize);
}

void
ecs_entity_set_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u64 playerId = ECS_ENTITY_CREATE(world);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    A ta = (A) { .AId = 203, .AAge = 27 };
    B tb = (B) { .BId = 17083, .BAge = 59 };
    ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 203, .AAge = 27 }));
    ECS_ENTITY_SET_COMPONENT(world, playerId, B, tb);

    A* a = ECS_ENTITY_GET_COMPONENT(world, playerId, A);
    B* b = ECS_ENTITY_GET_COMPONENT(world, playerId, B);

    Condition(a != NULL);
    Condition(b != NULL);

    Condition(ta.AId == a->AId);
    Condition(ta.AAge == a->AAge);
    Condition(tb.BId == b->BId);
    Condition(tb.BAge == b->BAge);

    Int_Value(a->AId);
    Int_Value(a->AAge);
    Int_Value(b->BId);
    Int_Value(b->BAge);
}

void
ecs_entity_set_component_twice_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u64 playerId = ECS_ENTITY_CREATE(world);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    {
	ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 203, .AAge = 27 }));
	ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 17083, .BAge = 59 }));
	A* a = ECS_ENTITY_GET_COMPONENT(world, playerId, A);
	B* b = ECS_ENTITY_GET_COMPONENT(world, playerId, B);
	Int_Value(a->AId);
	Int_Value(a->AAge);
    }

    {
	ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 991, .AAge = 127 }));
	A* a = ECS_ENTITY_GET_COMPONENT(world, playerId, A);
	Int_Value(a->AId);
	Int_Value(a->AAge);
    }
}

void ecs_test()
{
    TEST(world_create_test());
    TEST(ecs_register_component_test());
    TEST(ecs_get_component_id_by_type_test());
    TEST(ecs_entity_add_component_test());

    TEST(ecs_entity_get_components_test());
    TEST(ecs_entity_set_component_test());
    TEST(ecs_entity_set_component_twice_test());
}
