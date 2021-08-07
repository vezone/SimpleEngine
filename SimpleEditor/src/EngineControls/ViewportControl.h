#ifndef VIEWPORT_CONTROL_H
#define VIEWPORT_CONTROL_H

#include <stdbool.h>
#include <Utils/Types.h>
#include <Graphics/Camera/EditorCamera.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Window.h>
#include <ECS/Scene.h>
#include <UI/ui.h>

void viewport_create(NativeWindow window, EditorCamera* camera, FrameBuffer* framebuffer);
void viewport();
v2* viewport_get_bounds();
ImVec2 viewport_get_pos();
void viewport_on_update(f32 timestep);
void viewport_on_event(Event* event);
void viewport_menu_item();

#endif
