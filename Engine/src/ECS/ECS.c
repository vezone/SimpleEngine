#include "ECS.h"
#include "Components.h"
#include "Utils/Array.h"
#include "Utils/MemoryAllocator.h"

static u64 g_WorldsCount = 0;

ComponentID
_ecs_get_component_id_by_type(World* world, const char* componentName)
{
    ComponentStorage* storage = &world->Storage;
    i32 isComponentAlreadyRegistered = shgeti(storage->NameToId, componentName);
    if (isComponentAlreadyRegistered == -1)
    {
	return -1;
    }

    ComponentID componentId = shget(storage->NameToId, componentName);
    return componentId;
}

char*
_ecs_get_component_name_by_id(World* world, ComponentID componentId)
{
    ComponentStorage* storage = &world->Storage;
    if (hmgeti(storage->IdToName, componentId))
    {
	char* componentName = hmget(storage->IdToName, componentId);
	return componentName;
    }

    assert(0);
    return NULL;
}

i8
_entity_has_component(World* world, EntityID entityId, const char* componentName)
{
    if (world->ArchetypesStorage == NULL || hmgeti(world->ArchetypesStorage, entityId) == -1)
    {
	return -1;
    }

    ArchetypeRecord record = hmget(world->ArchetypesStorage, entityId);
    ComponentID* components = record.Archetype->Components;

    vassert(shgeti(world->Storage.NameToId, componentName) != -1);
    vassert(components != NULL);

    ComponentID componentId = shget(world->Storage.NameToId, componentName);

    i32 i;
    i32 count = array_count(components);
    for (i = 0; i < count; i++)
    {
	if (components[i] == componentId)
	{
	    return 1;
	}
    }

    return 0;
}

void
_ecs_register_component(World* world, const char* componentName, u32 componentSize)
{
    ComponentStorage* storage = &world->Storage;

    i32 isComponentAlreadyRegistered = shgeti(storage->NameToId, componentName);
    if (isComponentAlreadyRegistered != -1)
    {
	GERROR("Trying to add component that already registered! [%d]\n", isComponentAlreadyRegistered);
	assert(0);
    }

    ComponentID componentId = storage->LastId;
    ComponentData data = (ComponentData) { .Size = componentSize };
    hmput(storage->Components, componentId, data);
    shput(storage->NameToId, componentName, componentId);
    hmput(storage->IdToName, componentId, componentName);
    hmput(storage->IdToSize, componentId, componentSize);

    ++storage->LastId;
}

ComponentID*
_ecs_entity_get_components(World* world, EntityID entityId)
{
    ArchetypeStorage* archetypesStorage = world->ArchetypesStorage;
    i32 isArchetypeRecordExist = hmgeti(archetypesStorage, entityId);
    if (isArchetypeRecordExist == -1)
    {
	GWARNING("Trying to get archetype for entity wo components!\n");
	return NULL;
    }

    ArchetypeRecord archetypeRecord = hmget(archetypesStorage, entityId);
    return archetypeRecord.Archetype->Components;
}

void
_ecs_entity_add_component(World* world, EntityID entityId, const char* componentName)
{
    ComponentID* componentsId = _ecs_entity_get_components(world, entityId);
    ComponentID componentId = _ecs_get_component_id_by_type(world, componentName);
    array_push(componentsId, componentId);
    i32 index = _ecs_get_archetype_index(world, componentsId);
    Archetype* archetype = NULL;
    if (index != -1)
    {
	archetype = world->Archetypes[index];
    }
    else
    {
	archetype = _ecs_register_archetype(world, componentsId);
    }

    _ecs_entity_register_archetype(world, entityId, archetype);
}

void
_ecs_entity_register_archetype(World* world, EntityID entityId, Archetype* archetype)
{
    ComponentStorage* storage = &world->Storage;
    ArchetypeStorage* archetypesStorage = world->ArchetypesStorage;

    if (archetypesStorage != NULL && (hmgeti(archetypesStorage, entityId) != -1))
    {
	ArchetypeRecord prevArchetypeRecord = hmget(archetypesStorage, entityId);
	Archetype* prevArchetype = prevArchetypeRecord.Archetype;
	if (prevArchetype == archetype)
	{
	    GWARNING("Trying register archetype to entity that alread registered!\n");
	    return;
	}

	// вырезаем старые данные из Data, потом берем все что после и вставляем в data Address
	u64 beginDataAddress = u64(prevArchetype->Data) + prevArchetypeRecord.Offset;
	u64 endDataAddress = beginDataAddress + prevArchetype->ComponentsSize;
	u32 cutDataSize = u64(prevArchetype->Data) + prevArchetype->Size - endDataAddress;
	memcpy(((void*)beginDataAddress), ((void*)endDataAddress), cutDataSize);
	prevArchetype->Size -= prevArchetype->ComponentsSize;

	if (archetype->Size >= archetype->Capacity || (archetype->Size + archetype->ComponentsSize) >= archetype->Capacity)
	{
	    archetype->Capacity = 2 * archetype->Capacity + 1;
	    archetype->Data = memory_reallocate(archetype->Data, archetype->Capacity);
	}
	u32 currentEntityOffset = archetype->Size;
	memcpy(archetype->Data + currentEntityOffset, (void*)beginDataAddress, prevArchetype->ComponentsSize);
	archetype->Size += archetype->ComponentsSize;

	ArchetypeRecord archetypeRecord;
	archetypeRecord.Offset = currentEntityOffset;
	archetypeRecord.Archetype = archetype;

	hmput(archetypesStorage, entityId, archetypeRecord);
    }
    else
    {
	ArchetypeRecord archetypeRecord;
	archetypeRecord.Offset = 0;
	archetypeRecord.Archetype = archetype;

	hmput(world->ArchetypesStorage, entityId, archetypeRecord);

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
    }
}

Archetype*
_ecs_register_archetype(World* world, ComponentID* componentsId)
{
    ComponentStorage* storage = &world->Storage;
    u32 componentSize = 0;
    i32 componentsIdCount = array_count(componentsId);
    for (i32 i = 0; i < componentsIdCount; i++)
    {
	componentSize += hmget(storage->IdToSize, componentsId[i]);
    }

    Archetype* archetype = (Archetype*) memory_allocate(sizeof(Archetype));
    archetype->Data = NULL;
    archetype->Components = componentsId;
    archetype->Size = 0;
    archetype->Capacity = 0;
    archetype->ComponentsSize = componentSize;

    array_push(world->Archetypes, archetype);

    return archetype;
}

i32
_ecs_get_archetype_index(World* world, ComponentID* componentsId)
{
    i32 index = -1;

    for (i32 i = 0; i < array_count(world->Archetypes); i++)
    {
	ComponentID* components = world->Archetypes[i]->Components;
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
    world->ArchetypesStorage = NULL;

    world->Storage.NameToId = NULL;
    world->Storage.IdToName = NULL;
    world->Storage.IdToSize = NULL;
    world->Storage.Components = NULL;
    world->Storage.LastId = COMPONENTS_FIRST_ID;

    g_WorldsCount++;
}
