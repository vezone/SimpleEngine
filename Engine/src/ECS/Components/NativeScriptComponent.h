#ifndef NATIVE_SCRIPT_COMPONENT_H
#define NATIVE_SCRIPT_COMPONENT_H

typedef struct NativeScriptComponent
{
    void (*OnInit)();
} NativeScriptComponent;

#endif
