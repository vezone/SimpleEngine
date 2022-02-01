#include "UTest_Perspective.h"

#include "UTests/Test.h"
#include <Math/Math.h>
#include <cglm/cglm.h>

void
perspective_creation_test()
{
    m4 pers;
    f32 near = -1.0f,
	far = 1.0f,
	aspect = 1920.0f / 1080.0f,
	fov = 45.0f;

    perspective(near, far, aspect, fov, pers);

    //glm_perspective(float fovy, float aspect, float nearVal, float farVal, mat4  dest)
    mat4 glmPers;
    glm_perspective(fov, aspect, near, far, glmPers);

    M4_Equal(pers, glmPers);
    M4_Value(pers);
    M4_Value(glmPers);
}

void
perspective_test()
{
    TEST(perspective_creation_test());
}
