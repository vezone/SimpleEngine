#ifndef TAG_COMPONENT_H
#define TAG_COMPONENT_H

#include "ECS/ECSTypes.h"

typedef struct TagComponent
{
    const char* Name;
    EntityID ID;
} TagComponent;

#define TagComponent_(name, id) ((TagComponent) { .Name = name, .ID = id })

#endif
