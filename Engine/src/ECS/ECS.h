#ifndef ECS_H
#define ECS_H

#include <stddef.h>

#include <Utils/Types.h>
#include <Utils/stb_ds.h>
#include <Utils/Logger.h>

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

typedef struct ComponentStorageData
{
    ComponentID key;
    ComponentData value;
} ComponentStorageData;

typedef struct ComponentStorage
{
    struct { char* key; u32 value; }* NameToId;
    struct { ComponentID key; char* value; }* IdToName;
    struct { ComponentID key; u32 value; }* IdToSize;
    //TODO(bies): remove this in the future
    ComponentStorageData* Components;
    ComponentID LastId;
} ComponentStorage;

typedef struct Archetype
{
    void* Data;
    ComponentID* Components;
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
    EntityID key;
    ArchetypeRecord value;
} ArchetypeStorage;

typedef struct World
{
    WorldID Id;
    //Entity related stuff
    EntityID LastEntityId;
    Archetype** Archetypes;
    ArchetypeStorage* ArchetypesStorage;
    ComponentStorage Storage;
} World;

ComponentID _ecs_get_component_id_by_type(World* world, const char* componentName);
char* _ecs_get_component_name_by_id(World* world, ComponentID componentId);
i32 _ecs_get_archetype_index(World* world, ComponentID* componentsId);
i8 _ecs_is_type_registered_as_component(World* world, const char* componentName);
i8 _entity_has_component(World* world, EntityID entityId, const char* componentName);

void _ecs_register_component(World* world, const char* componentName, u32 componentSize);
Archetype* _ecs_register_archetype(World* world, ComponentID* componentsId);

ComponentID* _ecs_entity_get_components(World* world, EntityID entityId);
void _ecs_entity_add_component(World* world, EntityID entityId, const char* componentName);
void _ecs_entity_register_archetype(World* world, EntityID entityId, Archetype* archetype);

#define ECS_GET_COMPONENT_ID_BY_TYPE(world, component) _ecs_get_component_id_by_type((world), #component)
#define ECS_GET_COMPONENT_NAME_BY_ID(world, componentId)  _ecs_get_component_name_by_id((world), componentId);


#define ECS_IS_ENTITY_REGISTERED(world, entityId) (IS_ENTITY_ID_VALID(entityId) && entityId <= (world)->LastEntityId)
#define ENTITY_HAS_COMPONENT(world, entityId, type) (_entity_has_component((world), entityId, #type))
#define ECS_IS_TYPE_REGISTERED_AS_COMPONENT(world, type)  (shgeti((world)->Storage.NameToId, #type) != -1)


#define ECS_REGISTER_COMPONENT(world, component) \
    { \
	const char* componentName = #component; \
	_ecs_register_component((world), componentName, sizeof(component)); \
    }

#define ECS_ENTITY_CREATE(world, entityIdPtr)	\
    {						\
	World* worldPtr = (world);		\
	*(entityIdPtr) = worldPtr->LastEntityId;\
	++worldPtr->LastEntityId;		\
    }

#define ECS_ENTITY_ADD_COMPONENT(world, entityId, component) \
    { \
	 _ecs_entity_add_component((world), entityId, #component); \
    }

void world_init(World* world);

#endif
