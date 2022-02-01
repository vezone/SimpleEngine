#include "UTest_M4.h"
#include "UTests/Test.h"
#include <Math/Math.h>
#include <ECS/Components/TransformComponent.h>
#include <cglm/cglm.h>

static v4 g_BaseVectorPositions[4] = {
    { -0.5f, -0.5f, 0.0f, 1.0f }, /* 0 0 */
    { -0.5f,  0.5f, 0.0f, 1.0f }, /* 0 1 */
    {  0.5f,  0.5f, 0.0f, 1.0f }, /* 1 1 */
    {  0.5f, -0.5f, 0.0f, 1.0f }  /* 1 0 */
};

static v4 g_BaseVectorPositions3[4] = {
    { -0.5f, -0.5f, 0.0f, 1.0f }, /* 0 0 */
    { -0.5f,  0.5f, 0.0f, 1.0f }, /* 0 1 */
    {  0.5f,  0.5f, 0.0f, 1.0f }, /* 1 1 */
    {  0.5f, -0.5f, 0.0f, 1.0f }  /* 1 0 */
};

void
cglm_get_transform(vec3 position, vec2 size, f32 angle, mat4 transform)
{
    vec3 scaleVec = { size[0], size[1], 1.0f };
    mat4 translation = GLM_MAT4_IDENTITY_INIT;
    mat4 rotation = GLM_MAT4_IDENTITY_INIT;
    mat4 scale = GLM_MAT4_IDENTITY_INIT;

    glm_translate(translation, position);
    glm_rotate_z(rotation, glm_rad(angle), rotation);
    glm_scale(scale, scaleVec);

    glm_mat4_mulN((mat4*[]) {&translation, &rotation, &scale}, 3, transform);
}

void
cglm_get_transformed_v3(vec3 basePosition[4], mat4 transform, vec3 result[4])
{
    glm_mat4_mulv3(transform, basePosition[0], 1.0f, result[0]);
    glm_mat4_mulv3(transform, basePosition[1], 1.0f, result[1]);
    glm_mat4_mulv3(transform, basePosition[2], 1.0f, result[2]);
    glm_mat4_mulv3(transform, basePosition[3], 1.0f, result[3]);
}

void
m4_layout_test()
{
    m4 m = M4_IDENTITY;
    mat4 cm = GLM_MAT4_IDENTITY_INIT;

    M4_Equal(m, cm);
}

void
m4_mul_v4_wo_converttest()
{
    m4 matrix = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
    };
    v4 position = { 1.0f, 1.0f, 1.0f, 1.0f };

    v4 result;
    m4_mul_v4(matrix, position, result);

    V4_Value(result);
}

void
m4_get_transform_test()
{
    f32 angle = 0.0f;
    v2 size = { 1.0f, 1.0f };
    v3 position = { 1.0f, 1.0f, 1.0f };
    v3 rotation;
    mat4 cglmTransform = GLM_MAT4_IDENTITY_INIT;
    m4 transform = M4_IDENTITY;

    v3_assign_xyz(rotation, 0.0f, 0.0f, angle);
    cglm_get_transform(position, size, angle, cglmTransform);
    m4_transform(position, size, rotation, transform);

    M4_Equal(transform, cglmTransform);
    M4_Value(transform);
    M4_Value(cglmTransform);

    v3 rotation2;
    f32 angle2 = 45.0f;

    v3_assign_xyz(rotation2, 0.0f, 0.0f, angle2);
    m4_set_identity(cglmTransform);
    m4_set_identity(transform);
    cglm_get_transform(position, size, angle2, cglmTransform);
    m4_transform(position, size, rotation2, transform);

    M4_Equal(transform, cglmTransform);
    M4_Value(transform);
    M4_Value(cglmTransform);
}

void
m4_get_transform_profiling()
{
    f32 angle = 0.0f;
    v3 position = { 1.0f, 1.0f, 1.0f };
    v2 size = { 1.0f, 1.0f };
    v3 rotation;
    m4 transform = M4_IDENTITY;

    v3_assign_xyz(rotation, 0.0f, 0.0f, angle);
    m4_transform(position, size, rotation, transform);
    M4_Value(transform);
}

void
cglm_get_transform_profiling()
{
    f32 angle = 0.0f;
    v3 position = { 1.0f, 1.0f, 1.0f };
    v2 size = { 1.0f, 1.0f };
    v3 rotation;
    mat4 cglmTransform = GLM_MAT4_IDENTITY_INIT;

    v3_assign_xyz(rotation, 0.0f, 0.0f, angle);
    cglm_get_transform(position, size, angle, cglmTransform);
    M4_Value(cglmTransform);
}

void
m4_apply_transform_test()
{
    f32 angle = 0.0f;
    v3 xyz = { 1.0f, 1.0f, 1.0f };
    v2 size = { 1.5f, 1.0f };
    v3 rotation;
    m4 transform = M4_IDENTITY;
    mat4 cglmTransform = GLM_MAT4_IDENTITY_INIT;

    v3_assign_xyz(rotation, 0.0f, 0.0f, angle);
    cglm_get_transform(xyz, size, angle, cglmTransform);
    m4_transform(xyz, size, rotation, transform);

    vec3 cglmTransformedVec3;
    v3 transformedV3;

    glm_mat4_mulv3(cglmTransform, xyz, 1.0f, cglmTransformedVec3);
    m4_mul_v3(transform, xyz, transformedV3);

    V3_Is_Equal(cglmTransformedVec3, transformedV3);

    V3_Value(cglmTransformedVec3);
    V3_Value(transformedV3);
}

void
m4_check_transform_component()
{
    f32 angle = 0.0f;
    v3 xyz = { 1.0f, 1.0f, 1.0f };
    v2 size = { 1.5f, 1.0f };
    v3 rotation;
    m4 transform = M4_IDENTITY;
    mat4 cglmTransform = GLM_MAT4_IDENTITY_INIT;

    m4_transform(xyz, size, rotation, transform);

    TransformComponent transformDefault  = TransformComponent_(xyz, size, rotation);
    TransformComponent transformPosition = TransformComponent_PositionV3(xyz);

    M4_Equal(transformDefault.Transform, transform);
    M4_Equal(transformPosition.Transform, transform);
}

void
m4_test()
{
    TEST(m4_layout_test());
    TEST(m4_mul_v4_wo_converttest());
    TEST(m4_get_transform_test());
    TEST(m4_apply_transform_test());
    TEST(m4_check_transform_component());

    TEST(m4_get_transform_profiling());
    TEST(cglm_get_transform_profiling());
}
