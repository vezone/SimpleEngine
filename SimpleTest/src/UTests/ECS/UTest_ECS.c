#include "UTest_ECS.h"

#include <time.h>

#include "UTests/Test.h"
#include "ECS/ECS.h"
#include "ECS/Components/SpriteComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "Utils/Logger.h"
#include "Utils/String.h"
#include "Utils/MemoryAllocator.h"

typedef struct A { u32 AId; u32 AAge; } A;
typedef struct B { u32 BId; u32 BAge; } B;
typedef struct C { u32 CId; u32 CAge; } C;
typedef struct D { u32 CId; u32 CAge; } D;

/*

    World world;
    world_create(&world);
    ECS_REGISTER_COMPONENT(&world, PositionComponent);
    ECS_REGISTER_COMPONENT(&world, SpriteComponent);


    EntityID entityId = ECS_ENTITY_CREATE(&world);
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
ecs_get_component_id_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    u32 aId = ECS_GET_COMPONENT_ID(world, A);
    u32 bId = ECS_GET_COMPONENT_ID(world, B);

    Condition(ECS_GET_COMPONENT_ID(world, A) == COMPONENTS_FIRST_ID);
    Condition(ECS_GET_COMPONENT_ID(world, B) == (COMPONENTS_FIRST_ID + 1));
}

void
ecs_entity_add_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    EntityID playerId = ECS_ENTITY_CREATE(world);

    Condition(playerId == ENTITIES_FIRST_ID);
    Condition(WORLD_HAS_ENTITY(world, playerId) == 1);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) != 1);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) != 1);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A));
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B));

    const ComponentID* components = _ecs_entity_get_components_id(world, playerId);
    for (i32 i = 0; i < array_count(components); i++)
    {
	Int_Value(components[i]);
    }
}

void
ecs_entity_add_real_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, TransformComponent);
    ECS_REGISTER_COMPONENT(world, SpriteComponent);

    for (i32 i = 0; i < 3; i++)
    {
	EntityID playerId = ECS_ENTITY_CREATE(world);
	ECS_ENTITY_ADD_COMPONENT(world, playerId, TransformComponent);
	ECS_ENTITY_ADD_COMPONENT(world, playerId, SpriteComponent);
	ECS_ENTITY_SET_COMPONENT(world, playerId, TransformComponent, TransformComponent_(v3_(1.0,1.0,1.0), v3_(1.0,1.0,1.0), v3_(0.0,0.0,0.0)));
	ECS_ENTITY_SET_COMPONENT(world, playerId, SpriteComponent, SpriteComponent_Color(v3_(1.0,1.0,0.0)));

	Condition(array_count(world->Archetypes));
    }

    ECSQueryResult queryResult = ECS_ARCHETYPE_GET(world, TransformComponent,SpriteComponent);
    Int_Value(queryResult.Count);

    while (ECS_QUERY_RESULT_NEXT(queryResult))
    {
	TransformComponent* tc = ECS_QUERY_RESULT_GET(queryResult, TransformComponent);
	SpriteComponent* sc = ECS_QUERY_RESULT_GET(queryResult, SpriteComponent);

	M4_Value(tc->Transform);
	V4_Value(sc->Color);

	GERROR("%d\n", queryResult.Current);
    }
    GERROR("END!\n");
}

void
ecs_get_component_name_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, TransformComponent);
    ECS_REGISTER_COMPONENT(world, SpriteComponent);

    const char* tcName = ECS_GET_COMPONENT_NAME(world, 0);
    const char* scName = ECS_GET_COMPONENT_NAME(world, 1);

    String_Value(tcName);
    String_Equal(tcName, "TransformComponent");

    String_Value(scName);
    String_Equal(scName, "SpriteComponent");
}

void
ecs_entity_get_components_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);

    EntityID playerId = ECS_ENTITY_CREATE(world);

    Condition(playerId == ENTITIES_FIRST_ID);
    Condition(WORLD_HAS_ENTITY(world, playerId) == 1);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) == 0);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) == 0);

    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);

    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, A) == 1);
    Condition(ECS_ENTITY_HAS_COMPONENT(world, playerId, B) == 1);

    const ComponentID* components = _ecs_entity_get_components_id(world, playerId);
    Condition(array_count(components) == 2);
    Condition(components[0] == COMPONENTS_FIRST_ID);
    Condition(components[1] == COMPONENTS_FIRST_ID + 1);


    ArchetypeRecord record = _ecs_entity_get_archetype_record(world, playerId);
    Condition(record.Archetype->ComponentsSize == (sizeof(A) + sizeof(B)));
    Int_Value(record.Archetype->ComponentsSize);
}

void
ecs_entity_set_component_test()
{
    World* world = world_create();
    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);
    EntityID playerId = ECS_ENTITY_CREATE(world);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);
    ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 203  , .AAge = 27 }));
    ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 17083, .BAge = 59 }));

    A* a = ECS_ENTITY_GET_COMPONENT(world, ENTITIES_FIRST_ID, A);
    B* b = ECS_ENTITY_GET_COMPONENT(world, ENTITIES_FIRST_ID, B);

    Condition(a != NULL);
    Condition(b != NULL);

    A ta = (A) { .AId = 203  , .AAge = 27 };
    B tb = (B) { .BId = 17083, .BAge = 59 };

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

    EntityID playerId = ECS_ENTITY_CREATE(world);

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
	Condition(a->AId == 991);
	Condition(a->AAge == 127);
	Int_Value(a->AId);
	Int_Value(a->AAge);
    }
}

void
ecs_archetype_get_test()
{
    World* world = world_create();

    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);
    ECS_REGISTER_COMPONENT(world, C);

    srand(time(NULL));
    i32 i;
    i32 count = 10;
    EntityID playerId;

    playerId = ECS_ENTITY_CREATE(world);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);
    ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 1, .AAge = 2 }));
    ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 3, .BAge = 4 }));

    playerId = ECS_ENTITY_CREATE(world);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);
    ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 5, .AAge = 6 }));
    ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 7, .BAge = 8 }));

    playerId = ECS_ENTITY_CREATE(world);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
    ECS_ENTITY_ADD_COMPONENT(world, playerId, B);
    ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 9, .AAge = 10 }));
    ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 11, .BAge = 12 }));

    // NOTE(bies): старые данные остаются в предыдущем архетипе, наверное не критично, но может запутать в будущем
    ECS_ENTITY_ADD_COMPONENT(world, playerId, C);

    Condition(array_count(world->Archetypes) == 3);
    Int_Value(array_count(world->Archetypes));

    /*
      FIX: 12 Archetype instead of 2 not ok!
      NOTE(bies): архетипы добавляются нормально, проблема в query
      NOTE(bies): (archetype->Size / archetype->ComponentsSize) WRONG !!!
      NOTE(bies): все ок, так как в архетипе A,B у нас 4 элемента

      TODO: remove all 32 from string
      ECS_ARCHETYPE_GET(world, A, B) > ECS_ARCHETYPE_GET(world, "A,B")
    */
    ECSQueryResult queryResult = ECS_ARCHETYPE_GET(world, A,B);

    Condition(queryResult.Data != NULL);
    Condition(queryResult.Components != NULL);
    Condition(queryResult.World == world);
    Condition(queryResult.Offset == 0);
    Condition(queryResult.Current == -1);
    Condition(queryResult.Count == 2);
    Int_Value(queryResult.Count);

    while (ECS_QUERY_RESULT_NEXT(queryResult))
    {
	A* a = ECS_QUERY_RESULT_GET(queryResult, A);
	B* b = ECS_QUERY_RESULT_GET(queryResult, B);

	Int_Value(a->AId);
	Int_Value(a->AAge);
	Int_Value(b->BId);
	Int_Value(b->BAge);

	Int_Value(queryResult.Current);
    }
}

void
ecs_crash_test()
{
    World* world = world_create();

    ECS_REGISTER_COMPONENT(world, A);
    ECS_REGISTER_COMPONENT(world, B);
    ECS_REGISTER_COMPONENT(world, C);

    i32 count = 50000;
    for (i32 i = 0; i < count; i++)
    {
	EntityID playerId;
	playerId = ECS_ENTITY_CREATE(world);
	ECS_ENTITY_ADD_COMPONENT(world, playerId, A);
	ECS_ENTITY_ADD_COMPONENT(world, playerId, B);
	ECS_ENTITY_SET_COMPONENT(world, playerId, A, ((A) { .AId = 1, .AAge = 2 }));
	ECS_ENTITY_SET_COMPONENT(world, playerId, B, ((B) { .BId = 3, .BAge = 4 }));
    }

    ECSQueryResult queryResult = ECS_ARCHETYPE_GET(world, A,B);

    Condition(queryResult.Count == count);
    Int_Value(queryResult.Count);

    while (ECS_QUERY_RESULT_NEXT(queryResult))
    {
	A* a = ECS_QUERY_RESULT_GET(queryResult, A);
	B* b = ECS_QUERY_RESULT_GET(queryResult, B);

	Int_Value(queryResult.Current);
    }
}

#define CHECK_MEMORY() GWARNING("Allocated memory: %d\n", memory_helper_get_allocated_size())

void ecs_test()
{
    CHECK_MEMORY();

    TEST(world_create_test());
    TEST(ecs_register_component_test());

    TEST(ecs_get_component_id_test());
    TEST(ecs_get_component_name_test());

    TEST(ecs_entity_add_component_test());
    TEST(ecs_entity_add_real_component_test());
    CHECK_MEMORY();

    TEST(ecs_entity_get_components_test());

    TEST(ecs_entity_set_component_test());
    TEST(ecs_entity_set_component_twice_test());

    //NOTE(bies): here we got an error
    TEST(ecs_archetype_get_test());
    CHECK_MEMORY();

    TEST(ecs_crash_test());
    CHECK_MEMORY();
}
