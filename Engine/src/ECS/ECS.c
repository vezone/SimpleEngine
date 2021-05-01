#include "ECS.h"
#include "Components.h"
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

    //GWARNING("FirstId: %d, SecondId: %d\n", firstId, secondId);

    if (firstId != secondId)
    {
        return -1;
    }
    }

    return 1;
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
_ecs_register_archetype(World* world, const ComponentID* componentsId, u32 componentsSize)
{
    i32 componentsIdCount = array_count(componentsId);
    Archetype* archetype = (Archetype*) memory_allocate(sizeof(Archetype));
    archetype->Data = NULL;
    archetype->Components = componentsId;
    archetype->Size = 0;
    archetype->Capacity = 0;
    archetype->ComponentsSize = componentsSize;

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

    i32 i;
    const ComponentID* components = _ecs_entity_get_components_id(world, entityId);
    ComponentID componentId = shget(world->Storage.NameToId, componentName);
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
    if (hmgeti(world->ArchetypesStorage, entityId) == -1)
    {
    GWARNING("Entity don't have a components and don't belong to any archetype!\n");
    return NULL;
    }

    ArchetypeRecord archetypeRecord = hmget(world->ArchetypesStorage, entityId);
    return archetypeRecord.Archetype->Components;
}

void
_ecs_entity_add_component(World* world, EntityID entityId, const char* componentName)
{
    i32 i;
    u32 componentSize;
    ComponentID* newIds;
    const ComponentID* componentsId;

    ComponentID componentId = _ecs_get_component_id_by_name(world, componentName);
    ComponentStorage* storage = &world->Storage;

    if (hmgeti(world->ArchetypesStorage, entityId) == -1)
    {
    GWARNING("Entity don't have a components and don't belong to any archetype!\n");
    componentSize = 0;
    componentsId = NULL;
    newIds = NULL;
    }
    else
    {
    // ADDITINAL: add const to ComponentID* where is necessary
    ArchetypeRecord record = hmget(world->ArchetypesStorage, entityId);
    componentsId = record.Archetype->Components;
    componentSize = record.Archetype->ComponentsSize;
    newIds = array_copy(componentsId);
    }

    componentSize += hmget(storage->IdToSize, componentId);
    array_push(newIds, componentId);

    Archetype* archetype = _ecs_get_archetype(world, newIds, componentSize);
    if (archetype == NULL)
    {
    archetype = _ecs_register_archetype(world, newIds, componentSize);
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
    const ComponentID* components = record.Archetype->Components;

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

    vassert(archetype != NULL);

    i32 componentOffset = 0;
    const ComponentID* componentsId = archetype->Components;
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
    archetype->Size += archetype->ComponentsSize;
    }

    void* componentDataAddress = archetype->Data + record.Offset + componentOffset;

    memcpy(componentDataAddress, value, componentSize);
}

/*
  NOTE: скорее всего проблема здесь
 */

void
_ecs_archetype_replace_record(ArchetypeStorage* archetypesStorage, EntityID entityId, Archetype* archetype, ArchetypeRecord prevArchetypeRecord)
{
    Archetype* prevArchetype = prevArchetypeRecord.Archetype;
    u64 beginDataAddress = u64(prevArchetype->Data) + prevArchetypeRecord.Offset;
    u64 endDataAddress = beginDataAddress + prevArchetype->ComponentsSize;
    u32 cutDataSize = u64(prevArchetype->Data) + prevArchetype->Size - endDataAddress;

    i32 isReallocNeeded = ((archetype->Size + archetype->ComponentsSize) >= archetype->Capacity);
    if (isReallocNeeded)
    {
    archetype->Capacity = 2 * archetype->Capacity + 1;
    archetype->Data = memory_reallocate(archetype->Data, archetype->Capacity);
    }

    // paste prev data into new arch
    memcpy(archetype->Data + archetype->Size, (void*)beginDataAddress, prevArchetype->ComponentsSize);

    // вырезаем старые данные из Data, потом берем все что после и вставляем в data address
    // ****|****|****|****|
    // ****|    |****|****|
    // ****|****|****|
    memcpy(((void*)beginDataAddress), ((void*)endDataAddress), cutDataSize);

    hmput(archetypesStorage, entityId, ((ArchetypeRecord) { .Offset = archetype->Size, .Archetype = archetype }));

    prevArchetype->Size -= prevArchetype->ComponentsSize;
    archetype->Size += archetype->ComponentsSize;
}

//NOTE(bies): register on top of old Archetype
void
_ecs_entity_register_archetype(World* world, EntityID entityId, Archetype* archetype)
{
    ArchetypeStorage* archetypesStorage = world->ArchetypesStorage;
    i32 isPrevArchetypeExist = (hmgeti(archetypesStorage, entityId) != -1);
    ArchetypeRecord prevArchetypeRecord = hmget(archetypesStorage, entityId);
    Archetype* prevArchetype = prevArchetypeRecord.Archetype;

    if (archetype == prevArchetype)
    {
    GWARNING("Trying register archetype to entity that alread registered!\n");
    return;
    }

    i32 isPrevArchetypeNeedsToBeCopied = isPrevArchetypeExist && prevArchetype && prevArchetype != NULL && prevArchetype->Data != NULL;

    if (world->ArchetypesStorage != NULL && isPrevArchetypeNeedsToBeCopied)
    {
    _ecs_archetype_replace_record(archetypesStorage, entityId, archetype, prevArchetypeRecord);

    // hmput(archetypesStorage, entityId, ((ArchetypeRecord) { .Offset = archetype->Size, .Archetype = archetype }));
    }
    else
    {
    hmput(archetypesStorage, entityId, ((ArchetypeRecord) { .Offset = archetype->Size, .Archetype = archetype }));
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

ComponentID*
components_name_to_id(World* world, const char* components)
{
    ComponentID* ids = NULL;
    char** componentsNames = vstring_split((char*)components, ',');
    i32 count = array_count(componentsNames);
    for (i32 i = 0; i < count; i++)
    {
    ComponentID id = shget(world->Storage.NameToId, componentsNames[i]);
    array_push(ids, id);
    }

    return ids;
}

i32
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

i32
array_find(i32* array, i32 value)
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
FIX: archetype->Size == 0
 */
ECSQueryResult
_ecs_archetype_get(World* world, const char* components)
{
    ComponentID* ids = components_name_to_id(world, components);
    i32 archetypesCount = array_count(world->Archetypes);

    for (i32 a = 0; a < archetypesCount; a++)
    {
    Archetype* archetype = world->Archetypes[a];
    i32 isArrayEquals = array_is_equals(ids, archetype->Components);

    if (isArrayEquals)
    {
        // GERROR("ARCHETYPE SIZE: %d\n", archetype->Size);
        ECSQueryResult queryResult = (ECSQueryResult)
        {
            .Data = archetype->Data,
            .Components = ids,
            .World = world,
            .Offset = 0,
            .Count =  (archetype->Size / archetype->ComponentsSize),
            .Current = -1,
            .ComponentsSize =  archetype->ComponentsSize
        };
        return queryResult;
    }
    }

    return DEFAULT(ECSQueryResult);
}

void*
_ecs_query_result_get(ECSQueryResult queryResult, const char* componentName)
{
    World* world = queryResult.World;
    ComponentID componentId = shget(world->Storage.NameToId, componentName);
    i32 index = array_find(queryResult.Components, componentId);
    vassert(index != -1);

    u32 offset = queryResult.Current * queryResult.ComponentsSize;
    i32 count = array_count(queryResult.Components);
    for (i32 c = 0; c < count; c++)
    {
    ComponentID queryComponentID = queryResult.Components[c];
    if (componentId == queryComponentID)
    {
        return (void*) (queryResult.Data + offset);
    }

    offset += hmget(world->Storage.IdToSize, queryComponentID);
    }

    return (void*) (queryResult.Data + offset);
}

i32
_ecs_query_result_next(ECSQueryResult* queryResult)
{
    if (queryResult->Current >= queryResult->Count)
    {
    return 0;
    }

    ++queryResult->Current;

    return 1;
}
