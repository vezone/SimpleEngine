#pragma once

#include <cglm/cglm.h>
#include "Graphics/Window.h"
#include "Graphics/KeyCodes.h"
#include "Utils/Types.h"

typedef struct OrthographicCamera
{
    f32 Left;
    f32 Right;
    f32 Bot;
    f32 Top;
    vec3 Position;
    mat4 ViewMatrix;
    mat4 ViewProjectionMatrix;
    mat4 ProjectionMatrix;
} OrthographicCamera;

OrthographicCamera orthographic_camera_create(f32 left, f32 right, f32 bot, f32 top);
void orthographic_camera_set_projection(OrthographicCamera* camera, f32 left, f32 right, f32 bot, f32 top);
void orthographic_camera_recalculate_view_matrix(OrthographicCamera* camera, f32 rotation);
