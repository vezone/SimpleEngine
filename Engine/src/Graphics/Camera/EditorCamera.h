#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <Math/Orthographic.h>
#include <Math/M4.h>
#include <Graphics/Camera/Camera.h>

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
    f32 Rotation;
    v3 Position;
    v3 Axis;
} EditorCameraSettings;

typedef struct EditorCamera
{
    f32 ZoomLevel;
    f32 AspectRatio;
    f32 Speed;
    f32 Rotation;
    v3 Position;
    v3 Axis;
    Camera InternalCamera;
} EditorCamera;

force_inline void
editor_camera_settings_(EditorCamera* this, EditorCameraSettings settings)
{
    this->ZoomLevel = settings.ZoomLevel;
    this->AspectRatio = settings.AspectRatio;
    this->Speed = settings.Speed;
    this->Rotation = settings.Rotation;
    v3_assign(this->Position, settings.Position);
    v3_assign(this->Axis, settings.Axis);
}

force_inline EditorCamera
editor_camera_create(f32 left, f32 right, f32 bot, f32 top, EditorCameraSettings settings)
{
    EditorCamera editorCamera = {};
    editor_camera_settings_(&editorCamera, settings);

    f32 aspectZoom = editorCamera.AspectRatio * editorCamera.ZoomLevel;
    orthographic(-editorCamera.ZoomLevel, editorCamera.ZoomLevel, -aspectZoom, aspectZoom, -1.0f, 1.0f, editorCamera.InternalCamera.Projection);

    camera_recalculate_view(&editorCamera.InternalCamera, editorCamera.Position, editorCamera.Axis, editorCamera.Rotation);
    camera_recalculate_view_projection(&editorCamera.InternalCamera, editorCamera.Position, editorCamera.Axis, editorCamera.Rotation);

    editorCamera.InternalCamera = camera_create(editorCamera.InternalCamera.Projection, editorCamera.InternalCamera.View, editorCamera.InternalCamera.ViewProjection);

    return editorCamera;
}

force_inline void
editor_camera_on_update(EditorCamera* editorCamera)
{
    f32 aspectZoom = editorCamera->AspectRatio * editorCamera->ZoomLevel;

    orthographic(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, -aspectZoom, aspectZoom, -1.0f, 1.0f, editorCamera->InternalCamera.Projection);
    camera_recalculate_view(&editorCamera->InternalCamera, editorCamera->Position, editorCamera->Axis, editorCamera->Rotation);

    camera_recalculate_view_projection(&editorCamera->InternalCamera, editorCamera->Position, v3_(0, 0, 1), 0);
}

force_inline void
editor_camera_resize(EditorCamera* editorCamera, f32 width, f32 height)
{
    f32 aspectRatio = width / height;
    f32 aspectZoom = aspectRatio * editorCamera->ZoomLevel;

    editorCamera->AspectRatio = aspectRatio;
    orthographic(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, -aspectZoom, aspectZoom, -1.0f, 1.0f, editorCamera->InternalCamera.Projection);
    m4_mul(editorCamera->InternalCamera.Projection, editorCamera->InternalCamera.View, editorCamera->InternalCamera.ViewProjection);
}

#endif
