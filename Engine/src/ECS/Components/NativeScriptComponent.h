#ifndef NATIVE_SCRIPT_COMPONENT_H
#define NATIVE_SCRIPT_COMPONENT_H

#include "ECS/ECSTypes.h"
#include "ECS/ECS.h"

typedef struct NativeScriptComponent
{
    EntityID ID;
    World* World;
    //void (*OnInit)(NativeScriptComponent nativeScriptComponent);
} NativeScriptComponent;

//void native_script_component();
#define THIS_GET_COMPONENT(nativeScriptComponent, componentType) ECS_ENTITY_GET_COMPONENT(nativeScriptComponent.World, nativeScriptComponent.ID, componentType)

#if BASIC_IDEA_HOW_IT_SHOULD_WORK
void
native_script_component_on_init(NativeScriptComponent nativeScriptComponent)
{
    // how we can get this
    TransformComponent* transformComponent = THIS_GET_COMPONENT(nativeScriptComponent, TransformComponent);
}
#endif

#endif
