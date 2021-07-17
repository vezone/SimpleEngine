#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "OrthographicCamera.h"

typedef enum EditorMatrixType
{
    EditorMatrixType_Orthographic = 0,
    EditorMatrixType_Perspective = 1
} EditorMatrixType;

typedef struct EditorCameraSettings
{
    f32 ZoomLevel;
    f32 AspectRatio;
    f32 Speed;
    f32 Left;
    f32 Right;
    f32 Bot;
    f32 Top;
} EditorCameraSettings;

typedef struct EditorCamera
{
    f32 ZoomLevel;
    f32 AspectRatio;
    f32 Speed;
    f32 Rotation;
    OrthographicCamera Orthographic;
} EditorCamera;

EditorCamera editor_camera_create(EditorCameraSettings settings);
void editor_camera_on_update(EditorCamera* editorCamera, NativeWindow* window, f32 timestep);
void editor_camera_on_event(EditorCamera* editorCamera, Event* event);

force_inline void
editor_camera_resize(EditorCamera* camera, f32 width, f32 height)
{
    camera->AspectRatio = width / height;
    orthographic_camera_set_projection(&camera->Orthographic, -camera->AspectRatio * camera->ZoomLevel, camera->AspectRatio * camera->ZoomLevel, -camera->ZoomLevel, camera->ZoomLevel);
}

#endif
