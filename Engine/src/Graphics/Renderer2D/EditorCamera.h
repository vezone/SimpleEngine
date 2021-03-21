#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "OrthographicCamera.h"

typedef struct EditorCamera
{
    OrthographicCamera OrthoCamera;
} EditorCamera;

EditorCamera editor_camera_create(f32 left, f32 right, f32 bot, f32 top);
void editor_camera_on_update(EditorCamera* editorCamera, NativeWindow* window, f32 timestep);
void editor_camera_on_event(EditorCamera* editorCamera, Event* event);

force_inline void
editor_camera_resize(EditorCamera* camera, f32 width, f32 height)
{
    orthographic_camera_resize(&camera->OrthoCamera, width, height);
}

#endif
