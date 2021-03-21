#ifndef SANDBOX
#define SANDBOX

#include <Engine.h>

i32 sandbox_start();
void sandbox_on_attach(NativeWindow window);
void sandbox_on_update(f32 timestep);
void sandbox_on_ui_render();
void sandbox_on_event(Event* event);

static void
sandbox_on_destroy()
{
    shader_delete_collection();
}

#endif
