#ifndef APPLICATION_H
#define APPLICATION_H

#include "Graphics/Window.h"
#include "Graphics/KeyCodes.h"
#include "Utils/Logger.h"
#include "Event/Event.h"
#include "Platform/Linux/Linux.h"
#include "UI/ui.h"

typedef struct Layer
{
    const char* Name;
    void (*OnAttach)(NativeWindow window);
    void (*OnUpdate)(f32 timestep);
    void (*OnEvent)(Event* event);
    void (*OnDestoy)();
    void (*OnUIRender)();
} Layer;

typedef struct Application
{
    i8 IsMinimized;
    i8 IsRunning;
    char* EventTypeToString[32];
    Layer* Layers;
    NativeWindow Window;
} Application;

Application* application_get();
void application_push_layer(Layer layer);
void application_init(u32 width, u32 height, const char* name);
void application_start();
void application_on_event(Event* event);
void application_end();

#endif
