#include "OrthographicCamera.h"

OrthographicCamera
orthographic_camera_create(f32 left, f32 right, f32 bot, f32 top)
{
    OrthographicCamera camera = {};

    camera.Left = left;
    camera.Right = right;
    camera.Bot = bot;
    camera.Top = top;
    v3_assign(camera.Position, v3_(0, 0, 0));
    orthographic(left, right, bot, top, -1.0f, 1.0f, camera.ProjectionMatrix);
    m4_set_identity(camera.ViewMatrix);
    m4_mul(camera.ProjectionMatrix, camera.ViewMatrix, camera.ViewProjectionMatrix);

    return camera;
}

void
orthographic_camera_set_projection(OrthographicCamera* camera, f32 left, f32 right, f32 bot, f32 top)
{
    camera->Left = left;
    camera->Right = right;
    camera->Bot = bot;
    camera->Top = top;

    orthographic(left, right, bot, top, -1.0f, 1.0f, camera->ProjectionMatrix);
    m4_mul(camera->ProjectionMatrix, camera->ViewMatrix, camera->ViewProjectionMatrix);
}

void
orthographic_camera_recalculate_view_matrix(OrthographicCamera* camera, f32 rotation)
{
    m4 identityTranslate = M4_IDENTITY;
    m4 identityRotate = M4_IDENTITY;
    m4 transform = M4_IDENTITY;
    v3 rotateVec = v3_(0.0f, 0.0f, 1.0f);

    m4_translate(identityTranslate, camera->Position);
    m4_rotate(identityRotate, rotateVec, rotation);
    m4_mul(identityTranslate, identityRotate, transform);
    m4_inverse(transform, camera->ViewMatrix);
    m4_mul(camera->ProjectionMatrix, camera->ViewMatrix, camera->ViewProjectionMatrix);
}
