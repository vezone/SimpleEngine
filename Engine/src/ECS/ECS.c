#include "ECS.h"
#include <stdlib.h>

static u64 g_WorldsCount = 0;

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
	ComponentID firstID = first[c];
	ComponentID secondID = second[c];

	if (firstID != secondID)
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
    i32 i, count = array_count(array);
    for (i = 0; i < count; i++)
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
    i32 isComponentAlreadyRegistered = shash_geti(storage->NameToID, componentName);
    if (isComponentAlreadyRegistered == -1)
    {
	GERROR("Trying to get component %s that is not registered in the system!\n", componentName);
	vassert(isComponentAlreadyRegistered != -1);
    }

    ComponentID componentID = shash_get(storage->NameToID, componentName);
    return componentID;
}

const char*
_ecs_get_component_name_by_id(World* world, ComponentID componentID)
{
    ComponentStorage* storage = &world->Storage;
    if (hash_geti(storage->IDToName, componentID) != -1)
    {
	const char* componentName = hash_get(storage->IDToName, componentID);
	return componentName;
    }

    assert(0);
    return NULL;
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

    i32 isComponentAlreadyRegistered = shash_geti(world->Storage.NameToID, componentName);
    if (isComponentAlreadyRegistered != -1)
    {
	GERROR("Trying to add component that already registered! [%d]\n", isComponentAlreadyRegistered);
	vassert(0);
    }

    ComponentID componentID = world->Storage.LastID;
    shash_put(world->Storage.NameToID, componentName, componentID);
    hash_put(world->Storage.IDToName, componentID, componentName);
    hash_put(world->Storage.IDToSize, componentID, componentSize);

    ++world->Storage.LastID;
}

ArchetypeRecord
_ecs_entity_get_archetype_record(World* world, EntityID entityID)
{
    ArchetypeRecord archetypeRecord = hash_get(world->ArchetypesStorage, entityID);
    vassert(archetypeRecord.Archetype != NULL);
    return archetypeRecord;
}

/*
  ENTITY RELATED
*/
i32
_ecs_entity_has_component(World* world, EntityID entityID, const char* componentName)
{
    if (world->ArchetypesStorage == NULL || hash_geti(world->ArchetypesStorage, entityID) == -1)
    {
	return 0;
    }

    i32 i;
    const ComponentID* components = _ecs_entity_get_components_id(world, entityID);
    ComponentID componentID = shash_get(world->Storage.NameToID, componentName);
    i32 count = array_count(components);

    for (i = 0; i < count; i++)
    {
	if (components[i] == componentID)
	{
	    return 1;
	}
    }

    return 0;
}

const ComponentID*
_ecs_entity_get_components_id(World* world, EntityID entityID)
{
    if (hash_geti(world->ArchetypesStorage, entityID) == -1)
    {
	GWARNING("Entity don't have a components and don't belong to any archetype!\n");
	return NULL;
    }

    ArchetypeRecord archetypeRecord = hash_get(world->ArchetypesStorage, entityID);
    return archetypeRecord.Archetype->Components;
}

Archetype*
_ecs_entity_get_archetype(World* world, EntityID entityID)
{
    vassert(world != NULL && "World shouldn't be NULL!");

    i32 a, archetypesCount = array_count(world->Archetypes);
    ComponentID* ids = _ecs_entity_get_components_id(world, entityID);
    for (a = 0; a < archetypesCount; a++)
    {
	Archetype* archetype = world->Archetypes[a];
	i32 isArrayEquals = array_is_equals(ids, archetype->Components);
	if (isArrayEquals)
	{
	    return archetype;
	}
    }

    GWARNING("Archetype doesn't exist!");
    return NULL;
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
_ecs_entity_add_component(World* world, EntityID entityID, const char* componentName)
{
    i32 hasComponent = _ecs_entity_has_component(world, entityID, componentName);
    if (hasComponent)
	return;

    i32 isPrevArchetypeExist = (hash_geti(world->ArchetypesStorage, entityID) != -1);
    ArchetypeRecord prevArchRecord = hash_get(world->ArchetypesStorage, entityID);
    Archetype* prevArchetype = prevArchRecord.Archetype;

    i32 componentsSize;
    ComponentID* newIDs;
    ComponentID componentID = _ecs_get_component_id_by_name(world, componentName);
    if (isPrevArchetypeExist)
    {
	componentsSize = prevArchRecord.Archetype->ComponentsSize;
	newIDs = array_copy(prevArchRecord.Archetype->Components);
    }
    else
    {
	componentsSize = 0;
	newIDs = NULL;
    }
    componentsSize += hash_get(world->Storage.IDToSize, componentID);
    array_push(newIDs, componentID);

    Archetype* archetype = _ecs_get_archetype(world, newIDs, componentsSize);
    if (archetype == NULL)
    {
	archetype = _ecs_register_archetype(newIDs, componentsSize);
	array_push(world->Archetypes, archetype);
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

    hash_put(world->ArchetypesStorage, entityID, ((ArchetypeRecord) { .Offset = offset, .Archetype = archetype }));
}

void
_ecs_entity_add_entity(World* world, EntityID entityID, EntityID otherID)
{

}

void*
_ecs_entity_get_component(World* world, EntityID entityID, const char* componentName)
{
    vassert(world->ArchetypesStorage != NULL);
    vassert(hash_geti(world->ArchetypesStorage, entityID) != -1 && "Entity does not contain requested component");

    ComponentID componentID = shash_get(world->Storage.NameToID, componentName);
    void* result = _ecs_entity_get_component_by_id(world, entityID, componentID);
    return result;
}

void*
_ecs_entity_get_component_by_id(World* world, EntityID entityID, ComponentID componentID)
{
    ArchetypeRecord record = hash_get(world->ArchetypesStorage, entityID);
    void* data = (void*) (record.Archetype->Data + record.Offset);
    i32 offset = _ecs_archetype_get_component_offset(world->Storage.IDToSize, record.Archetype->Components, componentID);

    return (void*) (data + offset);
}

void
_ecs_entity_set_component(World* world, EntityID entityID, const char* componentName, i32 componentSize, void* value)
{
    ArchetypeRecord record = hash_get(world->ArchetypesStorage, entityID);
    Archetype* archetype = record.Archetype;
    ComponentID componentID = shash_get(world->Storage.NameToID, componentName);

    vassert(archetype != NULL);

    i32 componentOffset = _ecs_archetype_get_component_offset(world->Storage.IDToSize, archetype->Components, componentID);

    archetype_set_component(archetype, (record.Offset + componentOffset), componentSize, value);
}

/*
  ECS PUBLIC
*/

World*
world_create()
{
    World* world = (World*) memory_allocate(sizeof(World));
    world->ID = g_WorldsCount;
    world->LastEntityID = ENTITIES_FIRST_ID;

    world->Archetypes = NULL;
    world->ArchetypesStorage = NULL;

    world->Storage.NameToID = NULL;
    world->Storage.IDToName = NULL;
    world->Storage.IDToSize = NULL;
    world->Storage.LastID = COMPONENTS_FIRST_ID;

    ++g_WorldsCount;

    return world;
}

//WIP
World*
world_copy(World* world)
{
#if 0
    typedef struct World
    {
	WorldID ID;
	//Entity related stuff
	EntityID LastEntityID;
	Archetype** Archetypes;
	ArchetypeStorage* ArchetypesStorage;
	ComponentStorage Storage;
    } World;

    typedef struct ArchetypeRecord
    {
	Archetype* Archetype;
	i32 Offset;
    } ArchetypeRecord;

    World* dest = (World*) memory_allocate(sizeof(World));
    dest->ID = world->ID;
    dest->LastEntityID = world->LastEntityID;
    dest->Archetypes = NULL;
    //dest->ArchetypesStorage = ;

    for (i32 i = ENTITIES_FIRST_ID; i < world->LastEntityID; ++i)
    {
	Archetype* arch = _ecs_entity_get_archetype(world, i);

	ArchetypeRecord record = hash_get(world->ArchetypesStorage, i);
	record.Offset;
	record.Archetype;
	Archetype* arch = record->ArchetypesStorage[i];
	Archetype* newArch = archetype_copy(arch);

	array_push(dest->Archetypes, newArch);
    }

    world->ArchetypesStorage = NULL;

    dest->Storage.NameToID = array_copy(world->Storage.NameToID);
    dest->Storage.IDToName = array_copy(world->Storage.IDToName);
    dest->Storage.IDToSize = array_copy(world->Storage.IDToSize);

    world->Storage.LastID = COMPONENTS_FIRST_ID;

    ++g_WorldsCount;

    return dest;
#endif
    return NULL;
}

void
world_destroy(World* world)
{
    world->ID = -1;
    world->LastEntityID = -1;

    array_free(world->Archetypes);
    hash_free(world->ArchetypesStorage);

    shash_free(world->Storage.NameToID);
    hash_free(world->Storage.IDToName);
    hash_free(world->Storage.IDToSize);

    --g_WorldsCount;
}

/*
"TagComponent,TransformComponent,SpriteComponent"



*/
typedef struct ComponentsNameToIDCache
{
    const char* FullName;
    char** SplittedNames;
    ComponentID* ID;
} ComponentsNameToIDCache;

static ComponentsNameToIDCache* g_ComponentsNameToIDCache = NULL;

ComponentID*
components_name_to_id(World* world, const char* components)
{
    ComponentID* ids = NULL;

    i32 i, count = array_count(g_ComponentsNameToIDCache);
    for (i = 0; i < count; ++i)
    {
	ComponentsNameToIDCache compCache = g_ComponentsNameToIDCache[i];
	if (string_compare(compCache.FullName, components))
	{
	    return compCache.ID;
	}
    }

    char** splittedNames = string_split((char*)components, ',');
    count = array_count(splittedNames);
    for (i = 0; i < count; i++)
    {
	ComponentID id = shash_get(world->Storage.NameToID, splittedNames[i]);

	i32 result = array_find(ids, id);
	if (result != -1 && count > 0)
	{
	    GERROR("");
	    vassert(0);
	}

	array_push(ids, id);
    }

    ComponentsNameToIDCache cache;
    cache.FullName = components;
    cache.SplittedNames = splittedNames;
    cache.ID = ids;

    array_push(g_ComponentsNameToIDCache, cache);

    return ids;
}

/*
  FIX: archetype->Size == 0
*/

force_inline char*
string_remove_spaces(char* str, i32* lastIndex)
{
    i32 length = string_length(str);
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
    vassert(0);
    return DEFAULT(ECSQueryResult);
}

ECSQueryResult
_ecs_query_result_get(World* world, const char* comps)
{
    if (world == NULL)
	return (ECSQueryResult) { .Current = 0 };

    i32 archetypesCount = array_count(world->Archetypes);

    if (archetypesCount == 0)
	return (ECSQueryResult) { .Current = 0 };

    ComponentID* ids = components_name_to_id(world, comps);
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
	    queryResult.ComponentsSize = archetype->ComponentsSize;
	    return queryResult;
	}
    }

    vassert(0);
    return DEFAULT(ECSQueryResult);
}

void*
_ecs_query_result_get_data(ECSQueryResult queryResult, const char* componentName)
{
    World* world = queryResult.World;
    ComponentID componentID = shash_get(world->Storage.NameToID, componentName);
    i32 index = array_find(queryResult.Components, componentID);
    vassert(index != -1);

    i32 offset = queryResult.Current * queryResult.ComponentsSize;
    offset += _ecs_archetype_get_component_offset(world->Storage.IDToSize, queryResult.Components, componentID);

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
