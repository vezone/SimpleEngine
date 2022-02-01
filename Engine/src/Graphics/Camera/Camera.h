#ifndef CAMERA_H
#define CAMERA_H

#include <Math/MathTypes.h>
#include <Math/M4.h>

typedef struct Camera
{
    m4 View;
    m4 Projection;
    m4 ViewProjection;
} Camera;

force_inline void
camera_set_view(Camera* camera, m4 view)
{
    m4_copy(camera->View, view);
}

force_inline void
camera_set_projection(Camera* camera, m4 projection)
{
    m4_copy(camera->Projection, projection);
}

force_inline void
camera_set_view_projection(Camera* camera, m4 viewProjection)
{
    m4_copy(camera->ViewProjection, viewProjection);
}

force_inline Camera
camera_create(m4 projection, m4 view, m4 viewProjection)
{
    Camera camera;
    camera_set_view(&camera, view);
    camera_set_projection(&camera, projection);
    camera_set_view_projection(&camera, viewProjection);

    return camera;
}

force_inline void
camera_recalculate_view(Camera* camera, v3 position, v3 rotationAxis, f32 rotationValue)
{
    m4 identityTranslate = M4_IDENTITY;
    m4 identityRotate = M4_IDENTITY;
    m4 transform = M4_IDENTITY;

    m4_translate(identityTranslate, position);
    m4_rotate(identityRotate, rotationAxis, rotationValue);
    m4_mul(identityTranslate, identityRotate, transform);
    m4_inverse(transform, camera->View);
}

force_inline void
camera_recalculate_view_projection(Camera* camera, v3 position, v3 rotationAxis, f32 rotationValue)
{
    m4_mul(camera->Projection, camera->View, camera->ViewProjection);
}

#endif
