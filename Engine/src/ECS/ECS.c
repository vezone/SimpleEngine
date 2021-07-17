#include "ECS.h"
#include <Utils/MemoryAllocator.h>

static u64 g_WorldsCount = 0;

#define archetype_realloc(arch, newSize)				\
    {									\
	if (newSize >= arch->Capacity)					\
	{								\
	    i32 newCapacity = MAX(newSize, 2 * arch->Capacity + 1);	\
	    arch->Capacity = newCapacity;				\
	    arch->Data = memory_reallocate(arch->Data, newCapacity);	\
	}								\
    }

#define archetype_set_component(arch, offset, componentSize, value)	\
    {									\
	vassert(arch != NULL);						\
	vassert(arch->Data != NULL);					\
									\
	i32 newSize = (offset + componentSize);				\
	archetype_realloc(arch, newSize);				\
	vassert(newSize <= arch->Capacity);				\
	memcpy(archetype->Data + offset, value, componentSize);		\
    }

// вырезаем старые данные из Data, потом берем все что после и вставляем в data address
// ****|****|****|****|
// ****|    |****|****|
// ****|****|****|
#define archetype_cut_bytes(arch, data, bytesAfterData)			\
    {									\
	void* end = (data + arch->ComponentsSize);			\
	memset(data, '\0', arch->ComponentsSize);			\
	if ((arch->Data + arch->Size) < end)				\
	{								\
	    memcpy(data, (data + arch->ComponentsSize), bytesAfterData); \
	}								\
	arch->Size -= arch->ComponentsSize;				\
    }

/*
  ECS UTILS
*/
force_inline i32
components_ids_equals(const ComponentID* first, const ComponentID* second, u32 firstSize, u32 secondSize)
{
    i32 c;
    i32 firstCount = array_count(first);
    i32 secondCount = array_count(second);

    if (firstSize != secondSize || firstCount != secondCount)
    {
	return -1;
    }

    for (c = 0; c < firstCount; c++)
    {
	ComponentID firstId = first[c];
	ComponentID secondId = second[c];

	if (firstId != secondId)
	{
	    return -1;
	}
    }

    return 1;
}

force_inline i32
array_is_equals(const i32* ids1, const i32* ids2)
{
    i32 c;
    i32 ids1Count = array_count(ids1);
    i32 ids2Count = array_count(ids2);

    if (ids1Count != ids2Count)
    {
	return 0;
    }

    for (c = 0; c < ids1Count; c++)
    {
	if (ids1[c] != ids2[c])
	{
	    return 0;
	}
    }

    return 1;
}

force_inline i32
array_find(const i32* array, i32 value)
{
    i32 count = array_count(array);
    for (i32 i = 0; i < count; i++)
    {
	if (array[i] == value)
	{
	    return i;
	}
    }

    return -1;
}

/*
  ECS INTERNAL
*/
ComponentID
_ecs_get_component_id_by_name(World* world, const char* componentName)
{
    ComponentStorage* storage = &world->Storage;
    i32 isComponentAlreadyRegistered = shash_geti(storage->NameToId, componentName);
    if (isComponentAlreadyRegistered == -1)
    {
	GERROR("Trying to get component %s that is not registered in the system!\n", componentName);
	vassert(isComponentAlreadyRegistered != -1);
    }

    ComponentID componentId = shash_get(storage->NameToId, componentName);
    return componentId;
}

const char*
_ecs_get_component_name_by_id(World* world, ComponentID componentId)
{
    ComponentStorage* storage = &world->Storage;
    if (hash_geti(storage->IdToName, componentId) != -1)
    {
	const char* componentName = hash_get(storage->IdToName, componentId);
	return componentName;
    }

    assert(0);
    return NULL;
}

Archetype*
_ecs_register_archetype(World* world, const ComponentID* componentsId, u32 componentsSize)
{
    i32 newCapacity = 2 * componentsSize + 1;
    i32 componentsIdCount = array_count(componentsId);
    Archetype* archetype = (Archetype*) memory_allocate(sizeof(Archetype));
    archetype->Data = memory_allocate(newCapacity);
    archetype->Components = componentsId;
    archetype->Size = 0;
    archetype->Capacity = newCapacity;
    archetype->ComponentsSize = componentsSize;

    array_push(world->Archetypes, archetype);

    return archetype;
}

Archetype*
_ecs_get_archetype(World* world, const ComponentID* components, u32 size)
{
    i32 a;
    i32 allComponentsIsSame;
    i32 archetypeCount = array_count(world->Archetypes);
    i32 componentsCount = array_count(components);
    Archetype* archetype;
    const ComponentID* archetypeComponents;

    for (a = 0; a < archetypeCount; a++)
    {
	allComponentsIsSame = 1;

	archetype = world->Archetypes[a];
	archetypeComponents = archetype->Components;

	allComponentsIsSame = components_ids_equals(components, archetypeComponents, size, archetype->ComponentsSize);

	if (allComponentsIsSame == 1)
	{
	    return archetype;
	}
    }

    return NULL;
}

void
_ecs_register_component(World* world, const char* componentName, i32 componentSize)
{
    ComponentStorage* storage = &world->Storage;

    i32 isComponentAlreadyRegistered = shash_geti(world->Storage.NameToId, componentName);
    if (isComponentAlreadyRegistered != -1)
    {
	GERROR("Trying to add component that already registered! [%d]\n", isComponentAlreadyRegistered);
	vassert(0);
    }

    ComponentID componentId = world->Storage.LastId;
    shash_put(world->Storage.NameToId, componentName, componentId);
    hash_put(world->Storage.IdToName, componentId, componentName);
    hash_put(world->Storage.IdToSize, componentId, componentSize);

    ++world->Storage.LastId;
}

//NOTE(bies): register on top of old Archetype
void
_ecs_entity_register_archetype(World* world, i32 isPrevArchetypeExist, EntityID entityId, Archetype* archetype)
{

}

ArchetypeRecord
_ecs_entity_get_archetype_record(World* world, EntityID entityId)
{
    ArchetypeRecord archetypeRecord = hash_get(world->ArchetypesStorage, entityId);
    vassert(archetypeRecord.Archetype != NULL);
    return archetypeRecord;
}

/*
  ENTITY RELATED
*/
i8
_ecs_entity_has_component(World* world, EntityID entityId, const char* componentName)
{
    if (world->ArchetypesStorage == NULL || hash_geti(world->ArchetypesStorage, entityId) == -1)
    {
	return 0;
    }

    i32 i;
    const ComponentID* components = _ecs_entity_get_components_id(world, entityId);
    ComponentID componentId = shash_get(world->Storage.NameToId, componentName);
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

const ComponentID*
_ecs_entity_get_components_id(World* world, EntityID entityId)
{
    if (hash_geti(world->ArchetypesStorage, entityId) == -1)
    {
	//GWARNING("Entity don't have a components and don't belong to any archetype!\n");
	return NULL;
    }

    ArchetypeRecord archetypeRecord = hash_get(world->ArchetypesStorage, entityId);
    return archetypeRecord.Archetype->Components;
}

void
_archetype_copy_from_prev(Archetype* archetype, ArchetypeRecord prevArchRecord)
{
    Archetype* prevArchetype = prevArchRecord.Archetype;
    void* beginDataAddress = prevArchetype->Data + prevArchRecord.Offset;

    archetype_set_component(archetype, archetype->Size, prevArchetype->ComponentsSize, beginDataAddress);
    archetype->Size += archetype->ComponentsSize;

    i32 shouldPrevArchBeCopied = prevArchRecord.Archetype->Size != 0;
    if (shouldPrevArchBeCopied)
    {
	u64 bytesAfterData = prevArchetype->Size - prevArchRecord.Offset;
	archetype_cut_bytes(prevArchetype, beginDataAddress, bytesAfterData);
    }
}

void
_ecs_entity_add_component(World* world, EntityID entityId, const char* componentName)
{
    i32 isPrevArchetypeExist = (hash_geti(world->ArchetypesStorage, entityId) != -1);
    ArchetypeRecord prevArchRecord = hash_get(world->ArchetypesStorage, entityId);
    Archetype* prevArchetype = prevArchRecord.Archetype;

    i32 componentsSize;
    ComponentID* newIds;
    ComponentID componentId = _ecs_get_component_id_by_name(world, componentName);
    if (isPrevArchetypeExist)
    {
	componentsSize = prevArchRecord.Archetype->ComponentsSize;
	newIds = array_copy(prevArchRecord.Archetype->Components);
    }
    else
    {
	componentsSize = 0;
	newIds = NULL;
    }
    componentsSize += hash_get(world->Storage.IdToSize, componentId);
    array_push(newIds, componentId);

    Archetype* archetype = _ecs_get_archetype(world, newIds, componentsSize);
    if (archetype == NULL)
    {
	archetype = _ecs_register_archetype(world, newIds, componentsSize);
    }

    i32 offset = archetype->Size;
    if (isPrevArchetypeExist)
    {
	_archetype_copy_from_prev(archetype, prevArchRecord);
    }
    {
	// NOTE(bies): проблема в том, что у нас не хватает archetype->Capacity в set_component(), очевидно аллокацию оставить в add_component
	// archetype_push_bytes(archetype, beginDataAddress, archetype->ComponentsSize);
    }

    hash_put(world->ArchetypesStorage, entityId, ((ArchetypeRecord) { .Offset = offset, .Archetype = archetype }));
}

void
_ecs_entity_add_entity(World* world, EntityID entityId, EntityID otherId)
{

}

void*
_ecs_entity_get_component(World* world, EntityID entityId, const char* componentName)
{
    vassert(world->ArchetypesStorage != NULL);
    vassert(hash_geti(world->ArchetypesStorage, entityId) != -1);

    ComponentID componentId = shash_get(world->Storage.NameToId, componentName);
    void* result = _ecs_entity_get_component_by_id(world, entityId, componentId);
    return result;
}

void*
_ecs_entity_get_component_by_id(World* world, EntityID entityId, ComponentID componentID)
{
    ArchetypeRecord record = hash_get(world->ArchetypesStorage, entityId);
    void* data = (void*) (record.Archetype->Data + record.Offset);
    i32 offset = _ecs_archetype_get_component_offset(world->Storage, record.Archetype->Components, componentID);

    return (void*) (data + offset);
}

void
_ecs_entity_set_component(World* world, EntityID entityId, const char* componentName, i32 componentSize, void* value)
{
    ArchetypeRecord record = hash_get(world->ArchetypesStorage, entityId);
    Archetype* archetype = record.Archetype;
    ComponentID componentId = shash_get(world->Storage.NameToId, componentName);

    vassert(archetype != NULL);

    i32 componentOffset = _ecs_archetype_get_component_offset(world->Storage, archetype->Components, componentId);

    archetype_set_component(archetype, (record.Offset + componentOffset), componentSize, value);
}

/*
  ECS PUBLIC
*/

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

ComponentID*
components_name_to_id(World* world, const char* components)
{
    ComponentID* ids = NULL;
    char** componentsNames = vstring_split((char*)components, ',');
    i32 count = array_count(componentsNames);
    for (i32 i = 0; i < count; i++)
    {
	ComponentID id = shash_get(world->Storage.NameToId, componentsNames[i]);

	i32 result = array_find(ids, id);
	if (result != -1 && count > 0)
	{
	    vassert(0);
	}

	array_push(ids, id);
    }

    return ids;
}

/*
  FIX: archetype->Size == 0
*/
i32
_ecs_archetype_get_component_offset(ComponentStorage storage, const ComponentID* componentsId, ComponentID componentId)
{
    i32 i;
    i32 offset = 0;
    i32 count = array_count(componentsId);

    for (i = 0; i < count; i++)
    {
	if (componentId == componentsId[i])
	{
	    break;
	}

	offset += hash_get(storage.IdToSize, componentsId[i]);
    }

    return offset;
}


force_inline char*
vstring_remove_spaces(char* str, i32* lastIndex)
{
    i32 length = vstring_length(str);
    for (i32 i = *lastIndex; i < length; i++)
    {
	if (str[i] == ' ')
	{
	    str[i] = '\0';

	    if ((i + 1) == length)
		*lastIndex = i;
	    else
		*lastIndex = i + 1;

	    return str;
	}
    }

    return NULL;
}

static struct { char* Key; ECSQueryResult Value; }* g_CachedQueryResults = NULL;

ECSQueryResult
_ecs_archetype_get(World* world, ComponentID* ids)
{
#if 0
    char* components = (char*) istring(#__VA_ARGS__);

    i32 isExist = shash_geti(g_CachedQueryResults, components);
    if (isExist == -1)
    {
	i32* ids = NULL;
	i32 lastIndex = 0;
	i32 length = istring_length(components);
	i32 count = vstring_count_of_fast(components, length, ',') + 1;
	for (i32 i = 0; i < count; i++)
	{
	    char* name = (char*)vstring_remove_spaces(components + lastIndex * sizeof(char), &lastIndex);
	    ComponentID componentId = shash_get(world->Storage.NameToId, name);
	    array_push(ids, componentId);
	}
	ComponentID* ids = components_name_to_id(world, const char* components);
	ECSQueryResult result = _ecs_archetype_get(world, ids);
	shash_put(g_CachedQueryResults, components, result);

	i32 archetypesCount = array_count(world->Archetypes);

	for (i32 a = 0; a < archetypesCount; a++)
	{
	    Archetype* archetype = world->Archetypes[a];
	    i32 isArrayEquals = array_is_equals(ids, archetype->Components);

	    if (isArrayEquals)
	    {
		ECSQueryResult queryResult;
		queryResult.Data = archetype->Data;
		queryResult.Components = ids;
		queryResult.World = world;
		queryResult.Offset = 0;
		queryResult.Count =  (archetype->Size / archetype->ComponentsSize);
		queryResult.Current = -1;
		queryResult.ComponentsSize =  archetype->ComponentsSize;

		shash_put(g_CachedQueryResults, components, queryResult);
		return queryResult;
	    }
	}

	vassert(0);
    }
    else
    {
	ECSQueryResult queryResult = shash_get(g_CachedQueryResults, components);
	return queryResult;
    }
#endif

    vassert(0);
    return DEFAULT(ECSQueryResult);
}

ECSQueryResult
_ecs_archetype_get_old(World* world, const char* comps)
{
    ComponentID* ids = components_name_to_id(world, comps);
    i32 archetypesCount = array_count(world->Archetypes);

    for (i32 a = 0; a < archetypesCount; a++)
    {
	Archetype* archetype = world->Archetypes[a];
	i32 isArrayEquals = array_is_equals(ids, archetype->Components);

	if (isArrayEquals)
	{
	    ECSQueryResult queryResult;
	    queryResult.Data = archetype->Data;
	    queryResult.Components = ids;
	    queryResult.World = world;
	    queryResult.Offset = 0;
	    queryResult.Count =  (archetype->Size / archetype->ComponentsSize);
	    queryResult.Current = -1;
	    queryResult.ComponentsSize =  archetype->ComponentsSize;

	    return queryResult;
	}
    }

    vassert(0);
    return DEFAULT(ECSQueryResult);
}

void*
_ecs_query_result_get(ECSQueryResult queryResult, const char* componentName)
{
    World* world = queryResult.World;
    ComponentID componentId = shash_get(world->Storage.NameToId, componentName);
    i32 index = array_find(queryResult.Components, componentId);
    vassert(index != -1);

    i32 offset = queryResult.Current * queryResult.ComponentsSize;
    offset += _ecs_archetype_get_component_offset(world->Storage, queryResult.Components, componentId);

    return (void*) (queryResult.Data + offset);
}

i32
_ecs_query_result_next(ECSQueryResult* queryResult)
{
    ++queryResult->Current;

    if (queryResult->Current >= queryResult->Count)
    {
	return 0;
    }

    return 1;
}
