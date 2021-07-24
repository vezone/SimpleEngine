#ifndef SIMPLE_EDITOR_LAYER_H
#define SIMPLE_EDITOR_LAYER_H

#include <Engine.h>

void simple_editor_on_attach(NativeWindow window);
void simple_editor_on_update(f32 timestep);
void simple_editor_on_ui_render();
void simple_editor_on_event(Event* event);

static void
simple_editor_on_destroy()
{
    shader_delete_collection();
}

#endif
