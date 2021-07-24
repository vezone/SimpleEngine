#ifndef VIEWPORT_CONTROL_H
#define VIEWPORT_CONTROL_H

#include <stdbool.h>
#include <Utils/Types.h>
#include <Graphics/Renderer2D/EditorCamera.h>
#include <Graphics/FrameBuffer.h>

typedef struct ViewportSettings
{
    EditorMatrixType MatrixType;
} ViewportSettings;

void viewport();
void viewport_on_update(EditorCamera* camera, NativeWindow* window, f32 timestep);
void viewport_on_event(Event* event);
void viewport_menu_item();
ViewportSettings viewport_get_settings();

#endif
