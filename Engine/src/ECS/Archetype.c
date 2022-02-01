#include "Archetype.h"

Archetype*
archetype_copy(Archetype* copyFrom)
{
    Archetype* arch = (Archetype*) memory_allocate(sizeof(Archetype));
    arch->Data = memory_allocate(copyFrom->Capacity);
    memcpy(arch->Data, copyFrom->Data, copyFrom->Capacity);
    arch->Components = array_copy(copyFrom->Components);
    arch->Size = copyFrom->Size;
    arch->Capacity = copyFrom->Capacity;
    arch->ComponentsSize = copyFrom->ComponentsSize;

    return arch;
}

void
archetype_realloc(Archetype* arch, i32 newSize)
{
    if (newSize >= arch->Capacity)
    {
	i32 newCapacity = MAX(newSize, 2 * arch->Capacity + 1);
	arch->Capacity = newCapacity;
	arch->Data = memory_reallocate(arch->Data, newCapacity);
    }
}

void
archetype_set_component(Archetype* arch, i32 offset, i32 componentSize, void* value)
{
    vassert(arch != NULL);
    vassert(arch->Data != NULL);

    i32 newSize = (offset + componentSize);
    archetype_realloc(arch, newSize);
    vassert(newSize <= arch->Capacity);
    memcpy(arch->Data + offset, value, componentSize);
}

// вырезаем старые данные из Data, потом берем все что после и вставляем в data address
// ****|****|****|****|
// ****|    |****|****|
// ****|****|****|
void
archetype_cut_bytes(Archetype* arch, void* data, i32 bytesAfterData)
{
    void* end = (data + arch->ComponentsSize);
    memset(data, '\0', arch->ComponentsSize);
    if ((arch->Data + arch->Size) < end)
    {
	memcpy(data, (data + arch->ComponentsSize), bytesAfterData);
    }
    arch->Size -= arch->ComponentsSize;
}

void
archetype_free(Archetype* arch)
{
    vassert(arch != NULL && arch->Data != NULL && arch->Components && "NULL");

    memory_free(arch->Data);
    array_free(arch->Components);
    memory_free(arch);
}

Archetype*
_ecs_register_archetype(const ComponentID* componentsID, i32 componentsSize)
{
    i32 newCapacity = 2 * componentsSize + 1;
    Archetype* archetype = (Archetype*) memory_allocate(sizeof(Archetype));
    archetype->Data = memory_allocate(newCapacity);
    archetype->Components = componentsID;
    archetype->Size = 0;
    archetype->Capacity = newCapacity;
    archetype->ComponentsSize = componentsSize;

    return archetype;
}

typedef struct IDToSizeType
{
    ComponentID Key;
    i32 Value;
} IDToSizeType;

i32
_ecs_archetype_get_component_offset(void* idToSize, const ComponentID* componentsId, ComponentID componentId)
{
    i32 i;
    i32 offset = 0;
    i32 count = array_count(componentsId);

    IDToSizeType* idToSizeType = (IDToSizeType*) idToSize;

    for (i = 0; i < count; i++)
    {
	if (componentId == componentsId[i])
	{
	    break;
	}

	offset += hash_get(idToSizeType, componentsId[i]);
    }

    return offset;
}
