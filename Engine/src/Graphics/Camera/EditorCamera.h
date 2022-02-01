#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <Math/Orthographic.h>
#include <Math/M4.h>
#include <Math/BaseMath.h>
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
    i32 IsOrthographic;
    v3 Position;
    v3 Axis;
} EditorCameraSettings;

typedef struct EditorCamera
{
    f32 ZoomLevel;
    f32 AspectRatio;
    f32 Speed;
    f32 Rotation;
    f32 Far;
    f32 Near;
    f32 FOV;
    i32 IsOrthographic;
    v3 Position;
    v3 Axis;
    Camera InternalCamera;
} EditorCamera;

force_inline void
_editor_camera_recalc(EditorCamera* camera)
{
    camera_recalculate_view(&camera->InternalCamera, camera->Position, camera->Axis, camera->Rotation);
    camera_recalculate_view_projection(&camera->InternalCamera, camera->Position, camera->Axis, camera->Rotation);

    //camera->InternalCamera = camera_create(camera->InternalCamera.Projection, camera->InternalCamera.View, camera->InternalCamera.ViewProjection);
}

static void
editor_camera_set_orthograhic(EditorCamera* camera)
{
    camera->IsOrthographic = 1;
    f32 aspectZoom = camera->AspectRatio * camera->ZoomLevel;
    orthographic(-camera->ZoomLevel, camera->ZoomLevel, -aspectZoom, aspectZoom, camera->Near, camera->Far, camera->InternalCamera.Projection);

    _editor_camera_recalc(camera);
}

static void
editor_camera_set_perspective(EditorCamera* camera)
{
    camera->IsOrthographic = 0;
    perspective(camera->Near, camera->Far, camera->AspectRatio, camera->FOV, camera->InternalCamera.Projection);

    _editor_camera_recalc(camera);
}

force_inline void
editor_camera_settings_(EditorCamera* this, EditorCameraSettings settings)
{
    this->ZoomLevel = settings.ZoomLevel;
    this->AspectRatio = settings.AspectRatio;
    this->Speed = settings.Speed;
    this->Rotation = settings.Rotation;
    this->IsOrthographic = settings.IsOrthographic;
    v3_copy(this->Position, settings.Position);
    v3_copy(this->Axis, settings.Axis);
}

force_inline EditorCamera
editor_camera_create(f32 near, f32 far, EditorCameraSettings settings)
{
    EditorCamera editorCamera = {};
    editor_camera_settings_(&editorCamera, settings);

    f32 aspectZoom = editorCamera.AspectRatio * editorCamera.ZoomLevel;
    if (editorCamera.IsOrthographic)
    {
	editor_camera_set_orthograhic(&editorCamera);
    }
    else
    {
	editor_camera_set_perspective(&editorCamera);
    }

    editorCamera.Near = near;
    editorCamera.Far = far;
    editorCamera.FOV = rad(90);

    _editor_camera_recalc(&editorCamera);

    return editorCamera;
}

force_inline void
editor_camera_on_update(EditorCamera* editorCamera)
{
    f32 aspectZoom = editorCamera->AspectRatio * editorCamera->ZoomLevel;

    if (editorCamera->IsOrthographic)
    {
	orthographic(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, -aspectZoom, aspectZoom, -1.0f, 1.0f, editorCamera->InternalCamera.Projection);
    }
    else
    {
	perspective(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, editorCamera->AspectRatio, 45, editorCamera->InternalCamera.Projection);
    }

    _editor_camera_recalc(editorCamera);
}

force_inline void
editor_camera_resize(EditorCamera* editorCamera, f32 width, f32 height)
{
    f32 aspectRatio = width / height;
    f32 aspectZoom = aspectRatio * editorCamera->ZoomLevel;

    editorCamera->AspectRatio = aspectRatio;
    if (editorCamera->IsOrthographic)
    {
	orthographic(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, -aspectZoom, aspectZoom, -1.0f, 1.0f, editorCamera->InternalCamera.Projection);
    }
    else
    {
	perspective(-editorCamera->ZoomLevel, editorCamera->ZoomLevel, editorCamera->AspectRatio, 45, editorCamera->InternalCamera.Projection);
    }

    m4_mul(editorCamera->InternalCamera.Projection, editorCamera->InternalCamera.View, editorCamera->InternalCamera.ViewProjection);
}

#endif
