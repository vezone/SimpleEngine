#include "Renderer2D.h"

#include <glad/glad.h>
#include <Math/MathTypes.h>
#include <Utils/Logger.h>
#include <Utils/Array.h>
#include <Math/V3.h>
#include <Math/M4.h>

/*
  Batch renderer
*/

static Shader* g_Shader;
static OrthographicCamera* g_Camera;
static BatchRenderer2DData g_RendererData =
{
    .DataCount = 0,
    .IndexCount = 0
};
static Renderer2DStatistics* g_Statistics;
static v2 g_DefaultCoords[4] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f }
};
static i32 g_TextureIndices[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
static v4 g_BaseVectorPositions[4] = {
    { -0.5f, -0.5f, 0.0f, 1.0f }, /* 0 0 */
    { -0.5f,  0.5f, 0.0f, 1.0f }, /* 0 1 */
    {  0.5f,  0.5f, 0.0f, 1.0f }, /* 1 1 */
    {  0.5f, -0.5f, 0.0f, 1.0f }  /* 1 0 */
};

//static v4 g_BaseVectorPositions[4] = {
//    { -0.5f, -0.5f, 0.5f,  0.5f }, /* 0 0 */
//    { -0.5f,  0.5f, 0.5f, -0.5f }, /* 0 1 */
//    {  0.0f,  0.0f, 0.0f,  0.0f }, /* 1 1 */
//    {  1.0f,  1.0f, 1.0f,  1.0f }  /* 1 0 */
//};


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
fill_data_array(f32* destination, u32 startIndex, v3 position, v2 size, v4 color, v2* coords, f32 textureId, EntityID entityId)
{
    i32 i = startIndex;

    destination[i++] = position[0];
    destination[i++] = position[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = coords[0][0]; // [0, 0]
    destination[i++] = coords[0][1];
    destination[i++] = textureId; //(f32) textureId;
    destination[i++] = entityId; //(f32) isTextured;

    destination[i++] = position[0];
    destination[i++] = position[1] + size[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = coords[1][0]; // [0, 1]
    destination[i++] = coords[1][1];
    destination[i++] = textureId; //(f32) textureId;
    destination[i++] = entityId; //(f32) isTextured;

    destination[i++] = position[0] + size[0];
    destination[i++] = position[1] + size[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = coords[2][0]; // [1, 1]
    destination[i++] = coords[2][1];
    destination[i++] = textureId; //(f32) textureId;
    destination[i++] = entityId; //(f32) isTextured;

    destination[i++] = position[0] + size[0];
    destination[i++] = position[1];
    destination[i++] = position[2];
    destination[i++] = color[0];
    destination[i++] = color[1];
    destination[i++] = color[2];
    destination[i++] = color[3];
    destination[i++] = coords[3][0]; // [1, 0]
    destination[i++] = coords[3][1];
    destination[i++] = textureId; //(f32) textureId;
    destination[i++] = entityId; //(f32) isTextured;

    ++g_Statistics->RectanglesCount;
}


// we don't need isTextured here, because we have textureId that is 0 when we don't use a texture
force_inline void
fill_rotated_data_array(f32* destination, u32 startIndex, v3 positionsArray[4], v4 color, i32 textureId, EntityID entityId)
{
    i32 i = startIndex;

    // 10 per vertex
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
    destination[i++] = entityId;

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
    destination[i++] = entityId;

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
    destination[i++] = entityId;

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
    destination[i++] = entityId;

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
	id = list->Textures[i].ID;
	if (id == texture->ID)
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
    if (texture == NULL)
    {
	return 0;
    }

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

    // g_RendererData.Data = (f32*) memory_allocate(VertexBufferSize);
    // g_RendererData.Indices = (u32*) memory_allocate(IndicesBufferSize);

    GSUCCESS("Allocate Vertices Buffer (size: %lu bytes)\n", VertexBufferSize);
    GSUCCESS("Allocate Indices Buffer (size: %lu bytes)\n", IndicesBufferSize);

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

force_inline void
renderer_submit_rotated_base(m4 transform, v4 color, Texture2D* texture, EntityID entityId)
{
    v3 positions[4];
    v4 positionsFourDimensional[4];

    m4_mul_v4(transform, g_BaseVectorPositions[0], positionsFourDimensional[0]);
    m4_mul_v4(transform, g_BaseVectorPositions[1], positionsFourDimensional[1]);
    m4_mul_v4(transform, g_BaseVectorPositions[2], positionsFourDimensional[2]);
    m4_mul_v4(transform, g_BaseVectorPositions[3], positionsFourDimensional[3]);

    v3_v4(positionsFourDimensional[0], positions[0]);
    v3_v4(positionsFourDimensional[1], positions[1]);
    v3_v4(positionsFourDimensional[2], positions[2]);
    v3_v4(positionsFourDimensional[3], positions[3]);

    i32 textureId = texture_list_submit_texture_or_flush(&g_RendererData, texture);
    fill_rotated_data_array(g_RendererData.Data, g_RendererData.DataCount, positions, color, textureId, entityId);
    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void
renderer_submit_rectangle(v3 position, v2 size, v4 color, v2* coords, Texture2D* texture, EntityID entityId)
{
    i32 textureId = texture_list_submit_texture_or_flush(&g_RendererData, texture);
    v2 textureAdds = v2_(textureId, 1);

    fill_data_array(g_RendererData.Data, g_RendererData.DataCount, position, size, color, coords != NULL ? coords : g_DefaultCoords, textureId, entityId);

    g_RendererData.DataCount  += QuadVerticesCount;
    g_RendererData.IndexCount += 6;
}

void
renderer_submit_rotated_rectangle(v3 position, v2 size, v4 color, Texture2D* texture, f32 angle, EntityID entityId)
{
    v3 scaleVec = v3_(size[0], size[1], 1.0f);
    v3 rotation = v3_(0.0f, 0.0f, angle);
    m4 transform;
    m4_transform(position, scaleVec, rotation, transform);
    renderer_submit_rotated_base(transform, color, texture, entityId);
}

void
renderer_submit_rectanglet(m4 transform, v4 color, Texture2D* texture, EntityID entityId)
{
    renderer_submit_rotated_base(transform, color, texture, entityId);
}

void
renderer_submit_atlas(v3 position, v2 size, v4 color, TextureAtlas* atlas, i32 row, i32 col, EntityID entityId)
{
    f32 startX = (col * atlas->TextureWidth) / atlas->AtlasWidth;
    f32 endX = ((col + 1) * atlas->TextureWidth) / atlas->AtlasWidth;
    f32 startY = (row * atlas->TextureHeight) / atlas->AtlasHeight;
    f32 endY = ((row + 1) * atlas->TextureHeight) / atlas->AtlasHeight;

    v2 coords[4] = {
	v2_wo_convert(startX, startY),
	v2_wo_convert(startX, endY),
	v2_wo_convert(endX, endY),
	v2_wo_convert(endX, startY)
    };

    renderer_submit_rectangle(position, size, color, coords, atlas->Texture, entityId);
}

void
renderer_submit_dot(v3 position, v4 color)
{
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

    shader_set_m4(g_Shader, "u_ViewProjection", 1, 0, g_Camera->ViewProjectionMatrix[0]);
    shader_set_int1(g_Shader, "u_Textures", g_RendererData.List.NextTextureIndex, g_TextureIndices);

    glDrawElements(GL_TRIANGLES, g_RendererData.IndexCount, GL_UNSIGNED_INT, NULL);

    g_RendererData.DataCount  = 0;
    g_RendererData.IndexCount = 0;
    g_RendererData.List.NextTextureIndex = 1;

    ++g_Statistics->DrawCalls;

    texture_list_unbind(&g_RendererData.List);
    glDisable(GL_BLEND);
}

void
renderer_destroy()
{
    memory_free(g_RendererData.Data);
    memory_free(g_RendererData.Indices);
}

Renderer2DStatistics
renderer_get_statistics()
{
    return *g_Statistics;
}
