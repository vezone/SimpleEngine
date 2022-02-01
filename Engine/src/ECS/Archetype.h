#ifndef ARCHETYPE_H
#define ARCHETYPE_H

#include "ECSTypes.h"

typedef struct Archetype
{
    void* Data;
    const ComponentID* Components;
    i32 Size;
    i32 Capacity;
    i32 ComponentsSize;
} Archetype;

Archetype* archetype_copy(Archetype* copyFrom);
void archetype_realloc(Archetype* arch, i32 newSize);
void archetype_set_component(Archetype* arch, i32 offset, i32 componentSize, void* value);
void archetype_cut_bytes(Archetype* arch, void* data, i32 bytesAfterData);
void archetype_free(Archetype* arch);

Archetype* _ecs_register_archetype(const ComponentID* componentsID, i32 componentsSize);
i32 _ecs_archetype_get_component_offset(void* idToSize, const ComponentID* componentsID, ComponentID componentID);

#endif
