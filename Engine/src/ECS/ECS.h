#ifndef ECS_H
#define ECS_H

#include <stddef.h>
#include <stdarg.h>

#include "ECSTypes.h"
#include "Archetype.h"

/*
World
{
   "A,B"   -> Archetype*
   "A,B,C" -> Archetype*
}
*/

typedef struct ArchetypeRecord
{
    Archetype* Archetype;
    i32 Offset;
} ArchetypeRecord;

typedef struct ArchetypeStorage
{
    EntityID Key;
    ArchetypeRecord Value;
} ArchetypeStorage;

typedef struct ComponentStorage
{
    struct { const char* Key; i32 Value; }* NameToID;
    struct { ComponentID Key; const char* Value; }* IDToName;
    struct { ComponentID Key; i32 Value; }* IDToSize;
    ComponentID LastID;
} ComponentStorage;

typedef struct World
{
    WorldID ID;
    //Entity related stuff
    EntityID LastEntityID;
    Archetype** Archetypes;
    ArchetypeStorage* ArchetypesStorage;
    ComponentStorage Storage;
} World;

typedef struct ECSQueryResult
{
    World* World;
    void* Data;
    ComponentID* Components;
    i32 ComponentsSize;
    i32 Offset;
    i32 Count;
    i32 Current;
} ECSQueryResult;

/*
  ECS INTERNAL
*/

ComponentID _ecs_get_component_id_by_name(World* world, const char* componentName);
const char* _ecs_get_component_name_by_id(World* world, ComponentID componentID);
ComponentID* _ecs_component_names_to_ids(World* world, const char* components);

// delete ??
Archetype* _ecs_get_archetype(World* world, const ComponentID* components, u32 size);
void _ecs_register_component(World* world, const char* componentName, i32 componentSize);
ArchetypeRecord _ecs_entity_get_archetype_record(World* world, EntityID entityID);


/*
  ENTITY RELATED
 */

force_inline EntityID
_ecs_entity_create(World* world)
{
    EntityID entityID = world->LastEntityID;
    ++world->LastEntityID;
    return entityID;
}
force_inline void
_ecs_entity_destroy(World* world, EntityID entityID)
{
    ArchetypeRecord archRecord = _ecs_entity_get_archetype_record(world, entityID);

    Archetype* arch = archRecord.Archetype;
    i32 offset = archRecord.Offset;
    if ((offset + arch->ComponentsSize) != arch->Size)
    {
	memcpy(arch->Data + offset, arch->Data + arch->ComponentsSize, arch->Size - arch->ComponentsSize);
    }
    else
    {
	memset(arch->Data + offset, '\0', arch->ComponentsSize);
    }
}

void _ecs_entity_add_component(World* world, EntityID entityID, const char* componentName);
void _ecs_entity_add_entity(World* world, EntityID entityID, EntityID otherID);
void* _ecs_entity_get_component(World* world, EntityID entityID, const char* componentName);
void* _ecs_entity_get_component_by_id(World* world, EntityID entityID, ComponentID componentID);
void _ecs_entity_set_component(World* world, EntityID entityID, const char* componentName, i32 componentSize, void* value);
i32 _ecs_entity_has_component(World* world, EntityID entityID, const char* componentName);
const ComponentID* _ecs_entity_get_components_id(World* world, EntityID entityID);
Archetype* _ecs_entity_get_archetype(World* world, EntityID entityID);

ECSQueryResult _ecs_archetype_get(World* world, ComponentID* ids);
ECSQueryResult _ecs_query_result_get(World* world, const char* comps);
void* _ecs_query_result_get_data(ECSQueryResult queryResult, const char* componentName);
i32 _ecs_query_result_next(ECSQueryResult* queryResult);


/*
  ECS PUBLIC API
*/

World* world_create();
World* world_copy(World* world);
void world_destroy(World* world);

#define ECS_REGISTER_COMPONENT(world, component) _ecs_register_component((world), #component, sizeof(component))
#define ECS_GET_COMPONENT_ID(world, component) _ecs_get_component_id_by_name((world), #component)
#define ECS_GET_COMPONENT_ID_BY_NAME(world, component) _ecs_get_component_id_by_name((world), component)
#define ECS_GET_COMPONENT_NAME(world, componentID)  _ecs_get_component_name_by_id((world), componentID);

#define WORLD_HAS_ENTITY(world, entityID) (IS_ENTITY_ID_VALID(entityID) && entityID <= (world)->LastEntityID)
#define ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, type)  (shash_geti((world)->Storage.NameToID, #type) != -1)

#define ECS_ENTITY_CREATE(world) _ecs_entity_create((world))
#define ECS_ENTITY_DESTROY(world, entityID) _ecs_entity_destroy((world), entityID)
#define ECS_ENTITY_ADD_COMPONENT(world, entityID, type) _ecs_entity_add_component((world), entityID, #type)
#define ECS_ENTITY_ADD_ENTITY(world, entityID, otherID) _ecs_entity_add_entity((world), entityID, otherID)
#define ECS_ENTITY_GET_COMPONENT(world, entityID, type)	(type*)(_ecs_entity_get_component((world), entityID, #type))
//TODO convert to type*
#define ECS_ENTITY_GET_COMPONENT_BY_ID(world, entityID, id)	\
    (_ecs_entity_get_component_by_id((world), entityID, id))
#define ECS_ENTITY_SET_COMPONENT(world, entityID, type, value)		\
    ({									\
	__typeof__(value) tValue = value;				\
	_ecs_entity_set_component((world), entityID, #type, sizeof(type), ((void*) &(tValue))); \
    })
#define ECS_ENTITY_SET_COMPONENT_PTR_BY_NAME(world, entityID, name, size, valuePtr) \
    ({									\
	_ecs_entity_set_component((world), entityID, name, size, valuePtr); \
    })
#define ECS_ENTITY_HAS_COMPONENT(world, entityID, type) (_ecs_entity_has_component((world), entityID, #type))
#define ECS_ENTITY_GET_COMPONENTS(world, entityID) _ecs_entity_get_components_id((world), entityID)
#define ECS_ENTITY_GET_ARCHETYPE(world, entityID) _ecs_entity_get_archetype(world, entityID)

/*

  USAGE:
  ECSQueryResult queryResult = ECS_ARCHETYPE_GET(world, "A, B");
  while (ECS_QUERY_RESULT_NEXT(queryResult))
  {
      A* a = ECS_QUERY_RESULT_GET(queryResult, A);
      B* a = ECS_QUERY_RESULT_GET(queryResult, B);
  }
*/

#define ECS_QUERY_RESULT_GET(queryResult, type)  _ecs_query_result_get_data(queryResult, #type)
#define ECS_QUERY_RESULT_NEXT(queryResult)  _ecs_query_result_next(&queryResult)

#define ECS_ARCHETYPE_GET(world, ...)					\
    ({									\
	_ecs_query_result_get(world, #__VA_ARGS__);			\
    })

#endif
