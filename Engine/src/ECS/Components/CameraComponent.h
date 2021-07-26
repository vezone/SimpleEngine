#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Utils/Types.h"
#include "Graphics/Camera.h"

typedef struct CameraComponent
{
    i32 IsPrimary;
    Camera Camera;
} CameraComponent;

#define CameraComponent_(isPrimary, projection) _camera_component(isPrimary, projection)

force_inline CameraComponent
_camera_component(i32 isPrimary, m4 projection)
{
    CameraComponent result;

    Camera camera = {};
    m4_copy(projection, camera.Projection);

    result.Camera = camera;
    result.IsPrimary = isPrimary;

    return result;
}

#endif
