#pragma once

#include <cglm/cglm.h>
#include "Graphics/Window.h"
#include "Graphics/KeyCodes.h"
#include "Utils/Types.h"

typedef struct OrthographicCamera
{
    f32 Rotation;
    f32 Speed;
    f32 AspectRatio;
    f32 ZoomLevel;
    f32 Left;
    f32 Right;
    f32 Bot;
    f32 Top;
    vec3 Position;
    mat4 ViewMatrix;
    mat4 ViewProjectionMatrix;
    mat4 ProjectionMatrix;
} OrthographicCamera;

OrthographicCamera
orthographic_camera_create(f32 left, f32 right, f32 bot, f32 top);

void
orthographic_camera_set_projection(OrthographicCamera* camera, f32 left, f32 right, f32 bot, f32 top);

void
orthographic_camera_recalculate_view_matrix(OrthographicCamera* camera);

force_inline void
orthographic_camera_resize(OrthographicCamera* camera, f32 width, f32 height)
{
    camera->AspectRatio = width / height;
    orthographic_camera_set_projection(camera, -camera->AspectRatio * camera->ZoomLevel, camera->AspectRatio * camera->ZoomLevel, -camera->ZoomLevel, camera->ZoomLevel);
}
