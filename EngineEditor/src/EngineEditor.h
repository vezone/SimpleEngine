#ifndef ENGINE_EDITOR_LAYER_H
#define ENGINE_EDITOR_LAYER_H

#include <Engine.h>

void engine_editor_on_attach(NativeWindow window);
void engine_editor_on_update(f32 timestep);
void engine_editor_on_ui_render();
void engine_editor_on_event(Event* event);

static void
engine_editor_on_destroy()
{
    shader_delete_collection();
}

#endif
