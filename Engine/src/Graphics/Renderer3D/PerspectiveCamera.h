#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include "Math/MathTypes.h"

typedef struct PerspectiveCamera
{
    f32 Near;
    f32 Far;
    f32 Aspect;
    f32 FOV;
    v3 Position;
    m4 ViewMatrix;
    m4 ViewProjectionMatrix;
    m4 ProjectionMatrix;
} PerspectiveCamera;

PerspectiveCamera perspective_camera_create(f32 near, f32 far, f32 aspect, f32 fov, v3 position);

#endif
