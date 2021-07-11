#ifndef ECS_TYPES_H
#define ECS_TYPES_H

#include <Utils/Types.h>

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


#endif
