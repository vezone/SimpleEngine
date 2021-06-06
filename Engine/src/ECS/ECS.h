#ifndef ECS_H
#define ECS_H

#include <stddef.h>

#include <Utils/String.h>
#include <Utils/Array.h>
#include <Utils/Types.h>
#include <Utils/Logger.h>
#include <Utils/HashTable.h>

#define ECS_GET_COMPONENT_ID(world, component)

/*
  FROM 0 .. 255 internal id's
  FROM 256 .. 511 component's id's
  FROM 512 .. MAX_U64() Entities id's
*/
#define COMPONENTS_FIRST_ID 0
#define COMPONENTS_LAST_ID 255
#define INTERNAL_FIRST_ID 256
#define INTERNAL_LAST_ID 511
#define IS_COMPONENT_ID_VALID(id) ((id) >= COMPONENTS_FIRST_ID && (id) <= COMPONENTS_LAST_ID)
#define ENTITIES_FIRST_ID 512
#define ENTITIES_LAST_ID I32_MAX
#define IS_ENTITY_ID_VALID(id) ((id) >= ENTITIES_FIRST_ID && (id) <= ENTITIES_LAST_ID)

typedef i32 EntityID;
typedef i32 ComponentID;
typedef i32 WorldID;

/*
World
{
   "A,B"   -> Archetype*
   "A,B,C" -> Archetype*
}
*/

typedef struct ComponentData
{
    u32 Size;
} ComponentData;

typedef struct Archetype
{
    void* Data;
    const ComponentID* Components;
    u32 Size;
    u32 Capacity;
    u32 ComponentsSize;
} Archetype;

typedef struct ArchetypeRecord
{
    Archetype* Archetype;
    u32 Offset;
} ArchetypeRecord;

typedef struct ArchetypeStorage
{
    EntityID Key;
    ArchetypeRecord Value;
} ArchetypeStorage;

typedef struct ComponentStorage
{
    struct { const char* Key; u32 Value; }* NameToId;
    struct { ComponentID Key; const char* Value; }* IdToName;
    struct { ComponentID Key; u32 Value; }* IdToSize;
    ComponentID LastId;
} ComponentStorage;

typedef struct World
{
    WorldID Id;
    //Entity related stuff
    EntityID LastEntityId;
    Archetype** Archetypes;
    ArchetypeStorage* ArchetypesStorage;
    ComponentStorage Storage;
} World;

/*
  ECS INTERNAL
*/
ComponentID _ecs_get_component_id_by_name(World* world, const char* componentName);
const char* _ecs_get_component_name_by_id(World* world, ComponentID componentId);
// delete ??
i32 _ecs_is_archetype_registered(World* world, const ComponentID* componentsId);
Archetype* _ecs_get_archetype(World* world, const ComponentID* components, u32 size);
void _ecs_register_component(World* world, const char* componentName, u32 componentSize);
Archetype* _ecs_register_archetype(World* world, const ComponentID* componentsId, u32 componentsSize);
ArchetypeRecord _ecs_get_archetype_record(World* world, EntityID entityId);

/*
  ENTITY RELATED
 */

force_inline EntityID
_ecs_entity_create(World* world)
{
    EntityID entityId = world->LastEntityId;
    ++world->LastEntityId;
    return entityId;
}

i8 _ecs_entity_has_component(World* world, EntityID entityId, const char* componentName);
const ComponentID* _ecs_entity_get_components_id(World* world, EntityID entityId);
void _ecs_entity_add_component(World* world, EntityID entityId, const char* componentName);
void* _ecs_entity_get_component(World* world, EntityID entityId, const char* componentName);
void _ecs_entity_set_component(World* world, EntityID entityId, const char* componentName, i32 componentSize, void* value);
void _ecs_entity_register_archetype(World* world, EntityID entityId, Archetype* archetype);

/*
  ECS PUBLIC API
*/

World* world_create();

#define ECS_REGISTER_COMPONENT(world, component) _ecs_register_component((world), #component, sizeof(component))
#define ECS_GET_COMPONENT_ID_BY_TYPE(world, component) _ecs_get_component_id_by_name((world), #component)
#define ECS_GET_COMPONENT_NAME_BY_ID(world, componentId)  _ecs_get_component_name_by_id((world), componentId);

#define WORLD_HAS_ENTITY(world, entityId) (IS_ENTITY_ID_VALID(entityId) && entityId <= (world)->LastEntityId)
#define ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, type)  (shash_geti((world)->Storage.NameToId, #type) != -1)

#define ECS_ENTITY_CREATE(world) _ecs_entity_create((world))
#define ECS_ENTITY_ADD_COMPONENT(world, entityId, type) _ecs_entity_add_component((world), entityId, #type)
#define ECS_ENTITY_GET_COMPONENT(world, entityId, type)	(type*)(_ecs_entity_get_component((world), entityId, #type))
#define ECS_ENTITY_SET_COMPONENT(world, entityId, type, value)		\
    ({									\
	__typeof__(value) tValue = value;				\
	_ecs_entity_set_component((world), entityId, #type, sizeof(type), ((void*) &((tValue)))); \
    })
#define ECS_ENTITY_HAS_COMPONENT(world, entityId, type) (_ecs_entity_has_component((world), entityId, #type))

typedef struct ECSQueryResult
{
    void* Data;
    World* World;
    ComponentID* Components;
    i32 ComponentsSize;
    i32 Offset;
    i32 Count;
    i32 Current;
} ECSQueryResult;

ComponentID* components_name_to_id(World* world, const char* components);
i32 array_is_equals(const i32* ids1, const i32* ids2);
i32 array_find(i32* array, i32 value);
ECSQueryResult _ecs_archetype_get(World* world, const char* components);
// write function for this
void _ecs_archetype_put_data(Archetype* archetype, i32 size, void* data);

void* _ecs_query_result_get(ECSQueryResult queryResult, const char* componentName);
i32 _ecs_query_result_next(ECSQueryResult* queryResult);

/*

  USAGE:
  ECSQueryResult queryResult = ECS_ARCHETYPE_GET(world, "A, B");
  while (ECS_QUERY_RESULT_NEXT(queryResult))
  {
      A* a = ECS_QUERY_RESULT_GET(queryResult, A);
      B* a = ECS_QUERY_RESULT_GET(queryResult, B);
  }

*/

#define ECS_QUERY_RESULT_GET(queryResult, type)  _ecs_query_result_get(queryResult, #type)
#define ECS_QUERY_RESULT_NEXT(queryResult)  _ecs_query_result_next(&queryResult)
#define ECS_ARCHETYPE_GET(world, components) _ecs_archetype_get(world, components)

#endif
