#include "Renderer2D.h"

#include <glad/glad.h>
#include <Graphics/Shader.h>
#include <Utils/Logger.h>
#include <Utils/Array.h>
#include <Math/MathTypes.h>

/*
  Batch renderer
*/
#define vec2_ctr(vec, x, y) vec[0]=x;vec[1]=y

static Shader* g_Shader;
static OrthographicCamera* g_Camera;
static BatchRenderer2DData g_RendererData =
{
    .DataCount = 0,
    .IndexCount = 0
};
static Renderer2DStatistics* g_Statistics;
static vec2 g_DefaultCoords[4] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f }
};
static i32 g_TextureIndices[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
static vec3 g_BaseVectorPositions[4] = {
    { -0.5f, -0.5f, 0.0f }, /* 0 0 */
    { -0.5f,  0.5f, 0.0f }, /* 0 1 */
    {  0.5f,  0.5f, 0.0f }, /* 1 1 */
    {  0.5f, -0.5f, 0.0f }  /* 1 0 */
};

force_inline void
fill_indices_array(u32* indices, u32 length)
{
    i32 i;
    u32 temp;
    for (i = 0, temp = 0; i < length; i += 6, temp += 4)
    {
	indices[i]     = 0 + temp;
	indices[i + 1] = 1 + temp;
	indices[i + 2] = 2 + temp;
	indices[i + 3] = 2 + temp;
	indices[i + 4] = 3 + temp;
	indices[i + 5] = 0 + temp;
    }
}

force_inline void
fill_data_array(f32* destination, vec3 position, vec2 size, vec4 color, vec2 textureAdds, vec2* coords, u32 startIndex)
{
    i32 i = startIndex;

    destination[i++] = position[0];
    destination[i++] = position[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    // [0, 0]
    destination[i++] = coords[0][0];
    destination[i++] = coords[0][1];
    destination[i++] = textureAdds[0]; //(f32) textureId;
    destination[i++] = textureAdds[1]; //(f32) isTextured;

    destination[i++] = position[0];
    destination[i++] = position[1] + size[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    // [0, 1]
    destination[i++] = coords[1][0];
    destination[i++] = coords[1][1];
    destination[i++] = textureAdds[0]; //(f32) textureId;
    destination[i++] = textureAdds[1]; //(f32) isTextured;

    destination[i++] = position[0] + size[0];
    destination[i++] = position[1] + size[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    // [1, 1]
    destination[i++] = coords[2][0];
    destination[i++] = coords[2][1];
    destination[i++] = textureAdds[0]; //(f32) textureId;
    destination[i++] = textureAdds[1]; //(f32) isTextured;

    destination[i++] = position[0] + size[0];
    destination[i++] = position[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    // [1, 0]
    destination[i++] = coords[3][0];
    destination[i++] = coords[3][1];
    destination[i++] = textureAdds[0]; //(f32) textureId;
    destination[i++] = textureAdds[1]; //(f32) isTextured;

    ++g_Statistics->RectanglesCount;
}

force_inline void
fill_rotated_data_array(f32* destination, vec3 positionsArray[4], vec4 color, i32 textureId, u32 isTextured, u32 startIndex)
{
    i32 i = startIndex;

    destination[i++] = positionsArray[0][0];
    destination[i++] = positionsArray[0][1];
    destination[i++] = positionsArray[0][2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = 0;
    destination[i++] = 0;
    destination[i++] = (f32) textureId;
    destination[i++] = (f32) isTextured;

    destination[i++] = positionsArray[1][0];
    destination[i++] = positionsArray[1][1];
    destination[i++] = positionsArray[1][2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = 0;
    destination[i++] = 1;
    destination[i++] = (f32) textureId;
    destination[i++] = (f32) isTextured;

    destination[i++] = positionsArray[2][0];
    destination[i++] = positionsArray[2][1];
    destination[i++] = positionsArray[2][2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = 1;
    destination[i++] = 1;
    destination[i++] = (f32) textureId;
    destination[i++] = (f32) isTextured;

    destination[i++] = positionsArray[3][0];
    destination[i++] = positionsArray[3][1];
    destination[i++] = positionsArray[3][2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = 1;
    destination[i++] = 0;
    destination[i++] = (f32) textureId;
    destination[i++] = (f32) isTextured;

    ++g_Statistics->RectanglesCount;
}

// TODO (bies): mb add list->MaxTextureSlot check
force_inline void
texture_list_set_immutable(TextureList* list, Texture2D* texture)
{
    list->Textures[list->StartIndex++] = *texture;
}

force_inline i8
texture_list_is_full(TextureList* list)
{
    return list->NextTextureIndex >= list->MaxTextureSlot;
}

force_inline i32
texture_list_contains(TextureList* list, Texture2D* texture)
{
    i32 i, isAlreadyInList = -1;
    u32 id;

    for (i = 1; i < list->NextTextureIndex; i++)
    {
	id = list->Textures[i].RendererID;
	if (id == texture->RendererID)
	{
	    return i;
	}
    }

    return isAlreadyInList;
}

force_inline void
texture_list_add(TextureList* list, Texture2D* texture, i32 textureId)
{
    list->Textures[textureId] = *texture;
    ++list->NextTextureIndex;
}

force_inline void
texture_list_bind(TextureList* list)
{
    i32 i;
    for (i = 0; i < list->NextTextureIndex; i++)
    {
	texture2d_bind(&(list->Textures[i]), i);
    }
}

force_inline void
texture_list_unbind(TextureList* list)
{
    i32 i;
    for (i = 0; i < list->NextTextureIndex; i++)
    {
	texture2d_unbind(&(list->Textures[i]));
    }
}

force_inline i32
texture_list_submit_texture_or_flush(BatchRenderer2DData* rendererData, Texture2D* texture)
{
    i32 textureId;
    i32 isTextureListNotFull = !(texture_list_is_full(&rendererData->List));
    i32 isVerticesBufferNotFull = ((rendererData->DataCount + QuadVerticesCount) < VerticesCount);

    if (isTextureListNotFull && isVerticesBufferNotFull)
    {
	textureId = texture_list_contains(&rendererData->List, texture);
	if (textureId == -1)
	{
	    textureId = rendererData->List.NextTextureIndex;
	    texture_list_add(&rendererData->List, texture, textureId);
	}
    }
    else
    {
	renderer_flush();
	textureId = rendererData->List.StartIndex;
	texture_list_add(&rendererData->List, texture, textureId);
    }

    return textureId;
}

void
renderer_batch_init(Renderer2DStatistics* statistics, Shader* shader, Texture2D* whiteTexture, OrthographicCamera* camera)
{
    VertexBuffer vbo = {};
    IndexBuffer ibo = {};

    g_Statistics = statistics;
    g_Shader = shader;
    g_Camera = camera;

    vertex_buffer_allocate(&vbo, VertexBufferSize);
    vertex_buffer_bind(&vbo);
    vertex_buffer_add_layout(&vbo, 0, Float3);
    vertex_buffer_add_layout(&vbo, 0, Float4);
    vertex_buffer_add_layout(&vbo, 0, Float2);
    vertex_buffer_add_layout(&vbo, 0, Float1);
    vertex_buffer_add_layout(&vbo, 0, Float1);

    fill_indices_array(g_RendererData.Indices, IndicesCount);
    index_buffer_create(&ibo, g_RendererData.Indices, IndicesCount);
    index_buffer_bind(&ibo);

    vertex_array_create(&g_RendererData.Vao);
    vertex_array_add_vbo(&g_RendererData.Vao, vbo);
    vertex_array_add_ibo(&g_RendererData.Vao, ibo);
    vertex_array_bind(&g_RendererData.Vao);
    g_RendererData.List = (TextureList) {
	.StartIndex = 0,
	.MaxTextureSlot = 32,
	.NextTextureIndex = 1
    };
    texture_list_set_immutable(&g_RendererData.List, whiteTexture);

    renderer_statistics_init(g_Statistics);
}

void
renderer_submit_rectangle(vec3 position, vec2 size, vec2* coords, Texture2D* texture)
{
    i32 textureId = texture_list_submit_texture_or_flush(&g_RendererData, texture);
    v4 color = v4_(1.0f, 1.0f, 1.0f, 1.0f);
    v2 textureAdds = v2_(textureId, 1);

    if (coords == NULL)
    {
	fill_data_array(g_RendererData.Data, position, size, color, textureAdds, g_DefaultCoords, g_RendererData.DataCount);
    }
    else
    {
	fill_data_array(g_RendererData.Data, position, size, color, textureAdds, coords, g_RendererData.DataCount);
    }

    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void
renderer_submit_rotated_rectangle(vec3 position, vec2 size, f32 angle, Texture2D* texture)
{
    v3 scaleVec = v3_(0.0f, 0.0f, 1.0f);
    v2 nullSizeVec =  v2_(0.0f, 0.0f);
    v4 color = v4_(1.0f, 1.0f, 1.0f, 1.0f);
    m4 transform = GLM_MAT4_IDENTITY_INIT;
    m4 translation = GLM_MAT4_IDENTITY_INIT;
    m4 rotation = GLM_MAT4_IDENTITY_INIT;
    m4 scale = GLM_MAT4_IDENTITY_INIT;

    glm_translate(translation, position);
    glm_rotate_z(rotation, glm_rad(angle), rotation);

    scaleVec[0] = size[0];
    scaleVec[1] = size[1];
    glm_scale(scale, scaleVec);

    glm_mat4_mulN((mat4*[]) {&translation, &rotation, &scale}, 3, transform);

    vec3 positionsArray[4];
    glm_mat4_mulv3(transform, g_BaseVectorPositions[0], 1.0f, positionsArray[0]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[1], 1.0f, positionsArray[1]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[2], 1.0f, positionsArray[2]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[3], 1.0f, positionsArray[3]);

    i32 textureId = texture_list_submit_texture_or_flush(&g_RendererData, texture);
    fill_rotated_data_array(g_RendererData.Data, positionsArray, color, textureId, 1, g_RendererData.DataCount);
    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void
renderer_submit_colored_rectangle(vec3 position, vec2 size, vec4 color)
{
    v2 textureAdds = v2_(0, 0);

    fill_data_array(g_RendererData.Data, position, size, color, textureAdds, g_DefaultCoords, g_RendererData.DataCount);
    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void
renderer_submit_colored_rotated_rectangle(vec3 position, vec2 size, vec4 color, f32 angle)
{
    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    mat4 translationMat = GLM_MAT4_IDENTITY_INIT;
    mat4 rotationMat = GLM_MAT4_IDENTITY_INIT;
    mat4 scaleMat = GLM_MAT4_IDENTITY_INIT;
    vec3 scaleVec = (vec3) { size[0], size[1], 1.0f };
    vec2 nullSizeVec = (vec2) {0.0f, 0.0f};

    glm_translate(translationMat, position);
    glm_rotate_z(rotationMat, glm_rad(angle), rotationMat);
    glm_scale(scaleMat, scaleVec);

    glm_mat4_mulN((mat4*[]) {&translationMat, &rotationMat, &scaleMat}, 3, transform);

    vec3 positionsArray[4];
    glm_mat4_mulv3(transform, g_BaseVectorPositions[0], 1.0f, positionsArray[0]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[1], 1.0f, positionsArray[1]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[2], 1.0f, positionsArray[2]);
    glm_mat4_mulv3(transform, g_BaseVectorPositions[3], 1.0f, positionsArray[3]);

    fill_rotated_data_array(g_RendererData.Data, positionsArray, color, 0, 0, g_RendererData.DataCount);
    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

//transform
void renderer_submit_rectanglet(mat4 transform, Texture2D* texture)
{
    i32 textureId = texture_list_submit_texture_or_flush(&g_RendererData, texture);
    v4 color = v4_(1.0f, 1.0f, 1.0f, 1.0f);
    v2 textureAdds = v2_(textureId, 1);

    //fill_data_array(g_RendererData.Data, position, size, color, textureAdds, g_DefaultCoords, g_RendererData.DataCount);

    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void renderer_submit_colored_rectanglet(mat4 transform, vec4 color)
{
    v2 size = v2_(1.0f, 1.0f);
    v3 position = v3_(1.5f, 1.5f, 0.0f);
    renderer_submit_colored_rectangle(position, size, color);
}

//note(bm): use only 1 atlas at time
void
renderer_submit_atlas(vec3 position, vec2 size, TextureAtlas* atlas, i32 row, i32 col)
{
    f32 startX = (col * atlas->TextureWidth) / atlas->AtlasWidth;
    f32 endX = ((col + 1) * atlas->TextureWidth) / atlas->AtlasWidth;
    f32 startY = (row * atlas->TextureHeight) / atlas->AtlasHeight;
    f32 endY = ((row + 1) * atlas->TextureHeight) / atlas->AtlasHeight;

    vec2 coords[4];
    vec2_ctr(coords[0], startX, startY);
    vec2_ctr(coords[1], startX, endY);
    vec2_ctr(coords[2], endX, endY);
    vec2_ctr(coords[3], endX, startY);

    renderer_submit_rectangle(position, size, coords, &atlas->Texture);
}

void
renderer_submit_dot(vec3 position, vec4 color)
{
    // we need another buffer for dots
    vec2 coords[4];
    coords[0][0] = 0.0f;
    coords[0][1] = 0.0f;
    coords[1][0] = 0.0f;
    coords[1][1] = 0.0f;
    coords[2][0] = 0.0f;
    coords[2][1] = 0.0f;
    coords[3][0] = 0.0f;
    coords[3][1] = 0.0f;

    fill_data_array(g_RendererData.Data, position, (vec2) { 0.0f, 0.0f }, color, (vec2) { 0.0f, 0.0f }, coords, g_RendererData.DataCount);
}

void
renderer_flush()
{
    u32 size;
    glEnable(GL_BLEND);

    shader_bind(g_Shader);
    vertex_array_bind(&g_RendererData.Vao);

    texture_list_bind(&g_RendererData.List);

    PRINT_ONESF(GDEBUG, "DataCount: %d\n", g_RendererData.DataCount);

    size = g_RendererData.DataCount * sizeof(f32);
    vertex_buffer_set_data(g_RendererData.Vao.Vertex, g_RendererData.Data, size);

    shader_set_mat4(g_Shader, "u_ViewProjection", 1, 0, g_Camera->ViewProjectionMatrix[0]);
    shader_set_int1(g_Shader, "u_Textures", g_RendererData.List.NextTextureIndex, g_TextureIndices);

    glDrawElements(GL_TRIANGLES, g_RendererData.IndexCount, GL_UNSIGNED_INT, NULL);

    g_RendererData.DataCount  = 0;
    g_RendererData.IndexCount = 0;
    g_RendererData.List.NextTextureIndex = 1;

    ++g_Statistics->DrawCalls;

    texture_list_unbind(&g_RendererData.List);
    glDisable(GL_BLEND);
}

Renderer2DStatistics
renderer_get_statistics()
{
    return *g_Statistics;
}
