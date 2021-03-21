#include "Components.h"


#if 0
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

typedef struct Archetype
{
    // an array of Component Ids
    ComponentType Type;
    u64* EntitiIds;
    //AB|AB|AB...
    void* Data;
} Archetype;

/*
Archetype* arch = hmget(entityId)

World
{
   "A,B"   -> Archetype*
   "A,B,C" -> Archetype*
}

*/

typedef struct EntityData
{
    Archetype* Archetype;
    u32 Offset;
} EntityData;

typedef struct EntityTable
{
    //Entity Id
    u64 key;
    EntityData value;
} EntityTable;

typedef struct ArchetypeTable
{
    // "A,B,C"
    char* key;
    Archetype* value;
} ArchetypeTable;

typedef struct World
{
    u64 Id;
    //Entity related stuff
    u64 LastEntityId;
    EntityTable* EntityIds;//ConnectionTable;
    //Archetypes
    ArchetypeTable* Archetypes;
    //Components related stuff
    ComponentStorage Components;
} World;

Archetype* _ecs_archetype_create(World* world, const char* components, u32* sizes);

void _ecs_add_component(World* world, const char* componentName, u64 componentId, u64 componentSize);


/*
struct TransformComponent {f32 Position[3];}
ECS_REGISTER_COMPONENT(g_World, TransformComponent);
*/

#define ECS_ARCHETYPE_CREATE(world)

#define ECS_REGISTER_COMPONENT(world, component) \
    {								\
    World* tWorld = (world);					\
    ComponentTable* table = tWorld->Components.IdTable;		\
    const char* componentName = #component;			\
    ComponentType type = shget(table, componentName);\
    GERROR("Component: %s\n", componentName); \
    if (IS_COMPONENT_ID_VALID(type.Id))	\
    {\
	GERROR("Trying to add component (%s) that already exist in components table!\n", componentName);\
	assert(0);\
    }\
    u64 componentId = tWorld->Components.LastId++; \
    _ecs_add_component(tWorld, #component, componentId, sizeof(component)); \
    }

#define ECS_ENTITY_CREATE(world, entityIdPtr)			\
    {								\
	World* worldPtr = (world);				\
	*(entityIdPtr) = worldPtr->LastEntityId;		\
	++worldPtr->LastEntityId;				\
    }
/*
  hmput(worldPtr->EntityIds, entityIdPtr, NewArchetype);
*/
#define ECS_ENTITY_ADD_COMPONENT(world, entityId, component) \
    {							     \
	World* worldPtr = (world);			     \
	const char* componentName = #component;		     \
	i32 isArchetypeExist = shgeti(worldPtr->Archetypes, componentName); \
	GERROR("isArchetypeExist: %d\n", isArchetypeExist);  \
	if (isArchetypeExist == -1) \
	{					\
	}					\
	else					\
	{					\
	    Archetype* archetype;		\
	    hmput(worldPtr->EntityIds, entityIdPtr, archetype);\
	}				     \
    }

void world_init(World* world);

#endif
#endif
