#include "EditorCamera.h"

//EditorCamera editor_camera_create(f32 zoomLevel, f32 aspectRatio, f32 speed, f32 left, f32 right, f32 bot, f32 top)
EditorCamera
editor_camera_create(f32 left, f32 right, f32 bot, f32 top, v3 position, EditorCameraSettings settings)
{
    EditorCamera editorCamera = {};
    editorCamera.ZoomLevel = settings.ZoomLevel;
    editorCamera.AspectRatio = settings.AspectRatio;
    editorCamera.Speed = settings.Speed;
    editorCamera.Orthographic = orthographic_camera_create(left, right, bot, top);
    v3_assign(editorCamera.Orthographic.Position, position);

    return editorCamera;
}

void
editor_camera_on_update(EditorCamera* editorCamera)
{
    OrthographicCamera* camera = &editorCamera->Orthographic;
    orthographic_camera_set_projection(camera, -editorCamera->ZoomLevel, editorCamera->ZoomLevel, -editorCamera->AspectRatio * editorCamera->ZoomLevel, editorCamera->AspectRatio * editorCamera->ZoomLevel);
    orthographic_camera_recalculate_view_matrix(camera, 0.0f);
}
