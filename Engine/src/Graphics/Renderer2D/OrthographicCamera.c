#include "OrthographicCamera.h"

OrthographicCamera
orthographic_camera_create(f32 left, f32 right, f32 bot, f32 top)
{
    OrthographicCamera camera = {};

    camera.Left = left;
    camera.Right = right;
    camera.Bot = bot;
    camera.Top = top;
    glm_vec3_zero(camera.Position);
    glm_ortho(left, right, bot, top, -1.0f, 1.0f, camera.ProjectionMatrix);
    glm_mat4_identity(camera.ViewMatrix);
    glm_mat4_mul(camera.ProjectionMatrix, camera.ViewMatrix, camera.ViewProjectionMatrix);

    return camera;
}

void
orthographic_camera_set_projection(OrthographicCamera* camera, f32 left, f32 right, f32 bot, f32 top)
{
    camera->Left = left;
    camera->Right = right;
    camera->Bot = bot;
    camera->Top = top;

    glm_ortho(left, right, bot, top, -1.0f, 1.0f, camera->ProjectionMatrix);
    glm_mat4_mul(camera->ProjectionMatrix, camera->ViewMatrix, camera->ViewProjectionMatrix);
}

void
orthographic_camera_recalculate_view_matrix(OrthographicCamera* camera, f32 rotation)
{
    mat4 identityTranslate = GLM_MAT4_IDENTITY_INIT;
    mat4 identityRotate    = GLM_MAT4_IDENTITY_INIT;
    mat4 transform = {};
    vec3 rotate_vec = { 0.0f, 0.0f, 1.0f };

    glm_translate(identityTranslate, camera->Position);
    glm_rotate(identityRotate, rotation, rotate_vec);
    glm_mat4_mul(identityTranslate, identityRotate, transform);
    glm_mat4_inv(transform, camera->ViewMatrix);
    glm_mat4_mul(camera->ProjectionMatrix, camera->ViewMatrix, camera->ViewProjectionMatrix);
}
