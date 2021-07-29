#ifndef VIEWPORT_CONTROL_H
#define VIEWPORT_CONTROL_H

#include <stdbool.h>
#include <Utils/Types.h>
#include <Graphics/Renderer2D/EditorCamera.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Window.h>
#include <ECS/Scene.h>

void viewport_create(NativeWindow window, EditorCamera* camera, FrameBuffer* framebuffer);
void viewport();
void viewport_on_update(f32 timestep);
void viewport_on_event(Event* event);
void viewport_menu_item();

#endif
