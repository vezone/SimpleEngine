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
} EditorCameraSettings;

typedef struct EditorCamera
{
    f32 ZoomLevel;
    f32 AspectRatio;
    f32 Speed;
    OrthographicCamera Orthographic;
} EditorCamera;

EditorCamera editor_camera_create(f32 left, f32 right, f32 bot, f32 top, v3 position, EditorCameraSettings settings);
void editor_camera_on_update(EditorCamera* editorCamera);

force_inline void
editor_camera_resize(EditorCamera* camera, f32 width, f32 height)
{
    camera->AspectRatio = width / height;
    orthographic_camera_set_projection(&camera->Orthographic, -camera->AspectRatio * camera->ZoomLevel, camera->AspectRatio * camera->ZoomLevel, -camera->ZoomLevel, camera->ZoomLevel);
}

#endif
