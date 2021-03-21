#include "ECS.h"
#include "Components.h"
#include "Utils/Array.h"
#include "Utils/MemoryAllocator.h"

static u64 g_WorldsCount = 0;

u32
_ecs_component_get_id_by_type(World* world, const char* componentName, u32* componentId)
{
    ComponentStorage* storage = &world->Storage;
    i32 isComponentAlreadyRegistered = hmgeti(storage->NameToId, componentName);
    assert(isComponentAlreadyRegistered);

    *componentId = hmget(storage->NameToId, componentName);
}

void
_ecs_register_component(World* world, const char* componentName, u32 componentSize)
{
    ComponentStorage* storage = &world->Storage;

    i32 isComponentAlreadyRegistered = hmgeti(storage->NameToId, componentName);
    if (isComponentAlreadyRegistered != -1)
    {
	GERROR("Trying to add component that already registered! [%d]\n", isComponentAlreadyRegistered);
	assert(0);
    }

    u32 componentId = storage->LastId;
    ComponentData data = (ComponentData) { .Size = componentSize };
    hmput(storage->Components, componentId, data);
    hmput(storage->NameToId, componentName, componentId);

    ++storage->LastId;
}

u32*
_ecs_entity_get_components(World* world, u32 entityId)
{
    return NULL;
}

void
_ecs_register_archetype(World* world, u32* componentsId)
{
    Archetype archetype;
    archetype.Components = componentsId;
    archetype.Data = NULL;

    array_push(world->Archetypes, archetype);
}

i32
_ecs_get_archetype_index(World* world, u32* componentsId)
{
    i32 index = -1;

    for (i32 i = 0; i < array_count(world->Archetypes); i++)
    {
	u32* components = world->Archetypes[i].Components;
	for (i32 c = 0; c < array_count(components); c++)
	{
	    if (components[c] != componentsId[c])
	    {
		index = -1;
		break;
	    }

	    index = i;
	}
    }

    return index;
}

void
world_init(World* world)
{
    world->Id = g_WorldsCount;

    world->LastEntityId = ENTITIES_FIRST_ID;

    world->Archetypes = NULL;

#if 0
    world->EntityIds = NULL;
#endif

    world->Storage.NameToId = NULL;
    world->Storage.Components = NULL;
    world->Storage.LastId = COMPONENTS_FIRST_ID;

    g_WorldsCount++;
}
