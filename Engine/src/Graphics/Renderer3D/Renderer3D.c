#include "Renderer3D.h"

#if 0
#include <glad/glad.h>
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"

#include <Math/MathTypes.h>
#include <Math/V3.h>
#include <Math/M4.h>


VertexBuffer vbo;
IndexBuffer ibo;
VertexArray vao;
Shader* shaderPtr;
PerspectiveCamera* cameraPtr;
const i32 cubeVerticesCount = 3 * 8;
const i32 cubeVerticesSize = cubeVerticesCount * sizeof(f32);
const i32 indicesCount = 3 * 12;

f32 vertices[cubeVerticesCount] = {
    v3_wo_convert( 0.5f,  0.5f,  0.5f),
    v3_wo_convert(-0.5f,  0.5f, -0.5f),
    v3_wo_convert(-0.5f,  0.5f,  0.5f),
    v3_wo_convert( 0.5f, -0.5f, -0.5f),
    v3_wo_convert(-0.5f, -0.5f, -0.5f),
    v3_wo_convert( 0.5f,  0.5f, -0.5f),
    v3_wo_convert( 0.5f, -0.5f,  0.5f),
    v3_wo_convert(-0.5f, -0.5f,  0.5f)
};

u32 indices[indicesCount] = {
    0, 1, 2,
    1, 3, 4,
    5, 6, 3,
    7, 3, 6,
    2, 4, 7,
    0, 7, 6,
    0, 5, 1,
    1, 5, 3,
    5, 0, 6,
    7, 4, 3,
    2, 1, 4,
    0, 2, 7
};


void
renderer3d_batch_init(Shader* shader, PerspectiveCamera* camera)
{
    shaderPtr = shader;
    cameraPtr = camera;

    vertex_buffer_create(&vbo, vertices, sizeof(vertices));
    index_buffer_create(&ibo, indices, indicesCount);

    vertex_buffer_allocate(&vbo, sizeof(vertices));
    vertex_buffer_bind(&vbo);
    vertex_buffer_add_layout(&vbo, 0, Float3);
    //vertex_buffer_add_layout(&vbo, 0, Float4);
    //vertex_buffer_add_layout(&vbo, 0, Float2);
    //vertex_buffer_add_layout(&vbo, 0, Float1);
    //vertex_buffer_add_layout(&vbo, 0, Float1);

    index_buffer_create(&ibo, indices, indicesCount);
    index_buffer_bind(&ibo);

    vertex_array_create(&vao);
    vertex_array_add_vbo(&vao, vbo);
    vertex_array_add_ibo(&vao, ibo);
    vertex_array_bind(&vao);
}

void
renderer3d_submit()
{

}

/* Flush functions */
void
renderer3d_flush()
{
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    shader_bind(shaderPtr);
    vertex_array_bind(&vao);

    shader_set_m4(shaderPtr, "u_ViewProjection", 1, 0, cameraPtr->ViewProjectionMatrix[0]);

    vertex_buffer_set_data(vao.Vertex, vertices, cubeVerticesSize);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);

    glDisable(GL_CULL_FACE);
}
#endif
