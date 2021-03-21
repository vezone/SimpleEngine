#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <cglm/cglm.h>
#include "Graphics/Texture2D.h"

typedef struct PositionComponent
{
    mat4 Transform;
} PositionComponent;

typedef struct SpriteComponent
{
    vec4 Color;
    Texture2D Texture;
} SpriteComponent;

///
/// ECS INTERNAL
///

#define COMPONENT_INVALID_ID 404

#if 0
typedef struct ComponentNamesToIdsTable
{
    char* key;
    u32 value;
} ComponentNamesToIdsTable;

typedef struct ComponentAdditional
{
    u32 Size;
    const char* Name;
} ComponentAdditional;

typedef struct ComponentTable
{
    u32 key;
    ComponentAdditional value;
} ComponentTable;

typedef struct Type
{
    u32* ComponentIds;
} Type;

///
/// Tables
///
typedef struct ComponentStorage
{
    u64 LastId;
    //NOTE(bies): используем это только для того, чтобы не добавлять уже существующие компоненты
    ComponentTable* Components;
    ComponentNamesToIdsTable* NameToId;

    //TODO(bies): rename to NamesTable
    //TypeTable* Types;
} Storage;


u32 storage_name_to_id(Storage* storage, const char* name)
{
    i32 isNameInStorage = shgeti(storage->NameToId, name);
    if (isNameInStorage == -1)
    {
	return COMPONENT_INVALID_ID;
    }

    return shget(storage->NameToId);
}

void storage_add_component(Storage* storage, const char* componentName, u64 componentId, u64 componentSize)
{
    ComponentAdditional componentAdditional = (ComponentAdditional) { .Name = componentName, .Size = componentSize };

    shput(storage->Components, componentId, componentAdditional);
    shput(storage->NameToId, componentName, componentId);
}
#endif

#endif
