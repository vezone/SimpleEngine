#include "ECS.h"
#include "Components.h"
#include "Utils/Array.h"
#include "Utils/MemoryAllocator.h"

static u64 g_WorldsCount = 0;

/*
  ECS INTERNAL
*/

ComponentID
_ecs_get_component_id_by_name(World* world, const char* componentName)
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

const char*
_ecs_get_component_name_by_id(World* world, ComponentID componentId)
{
    ComponentStorage* storage = &world->Storage;
    if (hmgeti(storage->IdToName, componentId) != -1)
    {
	const char* componentName = hmget(storage->IdToName, componentId);
	return componentName;
    }

    assert(0);
    return NULL;
}

Archetype*
_ecs_get_archetype(World* world, ComponentID* components, u32 size)
{
    i32 containsAllComponents;
    i32 archetypeCount = array_count(world->Archetypes);
    i32 componentsCount = array_count(components);

    for (i32 a = 0; a < archetypeCount; a++)
    {
	containsAllComponents = 1;

	Archetype* archetype = world->Archetypes[a];
	ComponentID* archetypeComponentsIds = archetype->Components;
	u32 componentsSize = archetype->ComponentsSize;

	if (componentsSize != size || array_count(archetypeComponentsIds) != componentsCount)
	{
	    containsAllComponents = -1;
	    continue;
	}

	for (i32 c = 0; c < componentsCount; c++)
	{
	    ComponentID archetypeComponentId = archetypeComponentsIds[c];
	    ComponentID componentId = components[c];
	    GWARNING("Archetype component: %d, componentId: %d\n", archetypeComponentId, componentId);
	    if (archetypeComponentId != componentId)
	    {
		containsAllComponents = -1;
		break;
	    }
	}

	if (containsAllComponents == 1)
	{
	    return world->Archetypes[a];
	}
    }

    return NULL;
}

i8
_ecs_is_type_registered_as_component(World* world, const char* componentName);

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
    shput(storage->NameToId, componentName, componentId);
    hmput(storage->IdToName, componentId, componentName);
    hmput(storage->IdToSize, componentId, componentSize);

    ++storage->LastId;
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

ArchetypeRecord
_ecs_get_archetype_record(World* world, EntityID entityId)
{
    ArchetypeRecord archetypeRecord = hmget(world->ArchetypesStorage, entityId);
    vassert(archetypeRecord.Archetype != NULL);
    return archetypeRecord;
}

/*
  ENTITY RELATED
 */
i8
_ecs_entity_has_component(World* world, EntityID entityId, const char* componentName)
{
    if (world->ArchetypesStorage == NULL || hmgeti(world->ArchetypesStorage, entityId) == -1)
    {
	return 0;
    }

    ComponentID* components = _ecs_entity_get_components_id(world, entityId);
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

ComponentID*
_ecs_entity_get_components_id(World* world, EntityID entityId)
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
    ComponentID componentId = _ecs_get_component_id_by_name(world, componentName);
    ComponentID* componentsId = _ecs_entity_get_components_id(world, entityId);
    array_push(componentsId, componentId);

    u32 componentSize = 0;
    ComponentStorage* storage = &world->Storage;
    for (i32 i = 0; i < array_count(componentsId); i++)
    {
	componentSize += hmget(storage->IdToSize, componentsId[i]);
    }

    Archetype* archetype = _ecs_get_archetype(world, componentsId, componentSize);
    if (archetype == NULL)
    {
	archetype = _ecs_register_archetype(world, componentsId);
    }

    _ecs_entity_register_archetype(world, entityId, archetype);
}

void*
_ecs_entity_get_component(World* world, EntityID entityId, const char* componentName)
{
    vassert(world->ArchetypesStorage != NULL);
    vassert(hmgeti(world->ArchetypesStorage, entityId) != -1);

    u32 offset = 0;
    ComponentID componentId = shget(world->Storage.NameToId, componentName);
    ArchetypeRecord record = hmget(world->ArchetypesStorage, entityId);
    void* data = (void*) (record.Archetype->Data + record.Offset);
    ComponentID* components = record.Archetype->Components;

    for (i32 c = 0; c < array_count(components); c++)
    {
	if (componentId == components[c])
	{
	    return (void*) (data + offset);
	}

	offset += hmget(world->Storage.IdToSize, components[c]);
    }

    return (void*) (data + offset);
}

void
_ecs_entity_set_component(World* world, EntityID entityId, const char* componentName, u32 componentSize, void* value)
{
    ArchetypeRecord record = hmget(world->ArchetypesStorage, entityId);
    Archetype* archetype = record.Archetype;
    ComponentStorage* storage = &world->Storage;
    ComponentID componentId = shget(storage->NameToId, componentName);

    i32 componentOffset = 0;
    ComponentID* componentsId = archetype->Components;
    for (i32 i = 0; i < array_count(componentsId); i++)
    {
	if (componentId == componentsId[i])
	{
	    break;
	}

	componentOffset += hmget(storage->IdToSize, componentId);
    }

    if (archetype->Data == NULL)
    {
	archetype->Data = memory_allocate(archetype->ComponentsSize);
    }
    void* componentDataAddress = archetype->Data + record.Offset + componentOffset;

    memcpy(componentDataAddress, value, componentSize);
}

void
_ecs_entity_register_archetype(World* world, EntityID entityId, Archetype* archetype)
{
    ComponentStorage* storage = &world->Storage;
    ArchetypeStorage* archetypesStorage = world->ArchetypesStorage;
    ArchetypeRecord prevArchetypeRecord = hmget(archetypesStorage, entityId);
    Archetype* prevArchetype = prevArchetypeRecord.Archetype;

    if (archetypesStorage != NULL && (hmgeti(archetypesStorage, entityId) != -1) && prevArchetype->Data != NULL)
    {
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
    }
}

World*
world_create()
{
    World* world = (World*) memory_allocate(sizeof(World));
    world->Id = g_WorldsCount;

    world->LastEntityId = ENTITIES_FIRST_ID;

    world->Archetypes = NULL;
    world->ArchetypesStorage = NULL;

    world->Storage.NameToId = NULL;
    world->Storage.IdToName = NULL;
    world->Storage.IdToSize = NULL;
    world->Storage.LastId = COMPONENTS_FIRST_ID;

    g_WorldsCount++;

    return world;
}
