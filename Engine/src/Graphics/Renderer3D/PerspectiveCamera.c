#include "PerspectiveCamera.h"

#include "Math/Math.h"

PerspectiveCamera
perspective_camera_create(f32 near, f32 far, f32 aspect, f32 fov, v3 position)
{
    PerspectiveCamera camera = {};

    camera.Near = near;
    camera.Far = far;
    camera.Aspect = aspect;
    camera.FOV = fov;

    v3_copy(camera.Position, position);
    perspective(near, far, aspect, fov, camera.ProjectionMatrix);
    m4_set_identity(camera.ViewMatrix);
    m4_mul(camera.ProjectionMatrix, camera.ViewMatrix, camera.ViewProjectionMatrix);

    return camera;
}
