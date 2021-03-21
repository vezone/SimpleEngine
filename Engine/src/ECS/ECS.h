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
#define IS_COMPONENT_ID_VALID(id) (id >= COMPONENTS_FIRST_ID && id <= COMPONENTS_LAST_ID)
#define ENTITIES_FIRST_ID 512
#define ENTITIES_LAST_ID U64_MAX

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
    u32 key;
    ComponentData value;
} ComponentStorageData;

typedef struct ComponentStorage
{
    struct { char* key; u32 value; }* NameToId;
    ComponentStorageData* Components;
    u32 LastId;
} ComponentStorage;

typedef struct Archetype
{
    u32* Components;
    void* Data;
} Archetype;

typedef struct World
{
    u64 Id;
    //Entity related stuff
    u64 LastEntityId;
    Archetype* Archetypes;
    ComponentStorage Storage;
} World;

u32 _ecs_component_get_id_by_type(World* world, const char* componentName, u32* componentId);

void _ecs_register_component(World* world, const char* componentName, u32 componentSize);
void _ecs_register_archetype(World* world, u32* componentsId);
i32 _ecs_get_archetype_index(World* world, u32* componentsId);

u32* _ecs_entity_get_components(World* world, u32 entityId);

#define ECS_GET_COMPONENT_ID_BY_TYPE(world, component, componentId) \
    { \
	_ecs_component_get_id_by_type((world), #component, componentId); \
    }

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
	 World* worldPtr = (world); \
	 u32* componentsId = _ecs_entity_get_components(worldPtr, entityId); \
	 u32 componentId; \
	 _ecs_component_get_id_by_type(worldPtr, #component, &componentId); \
	 array_push(componentsId, componentId); \
	 i32 index = _ecs_get_archetype_index(worldPtr, componentsId); \
	 if (index != -1) \
	 {\
	     Archetype archetype = worldPtr->Archetypes[index]; \
	 }\
	 else\
	 {\
	     _ecs_register_archetype(worldPtr, componentsId);\
	 }\
    }

void world_init(World* world);

#endif
