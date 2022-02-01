#include "Renderer2D.h"

#include <glad/glad.h>
#include <Math/MathTypes.h>
#include <Math/V3.h>
#include <Math/M4.h>

/*
  Batch renderer
*/

static Shader* BatchedShader;
static Camera* CurrentCamera;
static BatchRenderer2DData RendererData =
{
    .Rectangles.DataCount = 0,
    .Rectangles.IndexCount = 0
};
static Renderer2DStatistics* Statistics;
static v2 DefaultCoords[4] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f }
};
static i32 TextureIndices[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
static v4 BaseVectorPositions[4] = {
    { -0.5f, -0.5f, 0.0f, 1.0f }, /* 0 0 */
    { -0.5f,  0.5f, 0.0f, 1.0f }, /* 0 1 */
    {  0.5f,  0.5f, 0.0f, 1.0f }, /* 1 1 */
    {  0.5f, -0.5f, 0.0f, 1.0f }  /* 1 0 */
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

    ++Statistics->RectanglesCount;
}

force_inline void
fill_rotated_data_array(f32* destination, u32 startIndex, v3 positionsArray[4], v4 color, i32 textureID, EntityID entityID)
{
    i32 i = startIndex;

    RenderableRectangle rRect;

    rRect.Position[0][0] = positionsArray[0][0];
    rRect.Position[0][1] = positionsArray[0][1];
    rRect.Position[0][2] = positionsArray[0][2];

    rRect.Position[1][0] = positionsArray[1][0];
    rRect.Position[1][1] = positionsArray[1][1];
    rRect.Position[1][2] = positionsArray[1][2];

    rRect.Position[2][0] = positionsArray[2][0];
    rRect.Position[2][1] = positionsArray[2][1];
    rRect.Position[2][2] = positionsArray[2][2];

    rRect.Position[3][0] = positionsArray[3][0];
    rRect.Position[3][1] = positionsArray[3][1];
    rRect.Position[3][2] = positionsArray[3][2];

    rRect.Color[0] = color[0];
    rRect.Color[1] = color[1];
    rRect.Color[2] = color[2];
    rRect.Color[3] = color[3];

    rRect.TextureID = textureID;
    rRect.EntityID = entityID;

    array_push(RendererData.Rectangles.RRectangles, rRect);

    ++Statistics->RectanglesCount;
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
    i32 isTextureListNotFull = !(texture_list_is_full(&rendererData->Rectangles.List));
    i32 isVerticesBufferNotFull = ((rendererData->Rectangles.DataCount + QuadVerticesCount) < VerticesCount);

    if (isTextureListNotFull && isVerticesBufferNotFull)
    {
	textureId = texture_list_contains(&rendererData->Rectangles.List, texture);
	if (textureId == -1)
	{
	    textureId = rendererData->Rectangles.List.NextTextureIndex;
	    texture_list_add(&rendererData->Rectangles.List, texture, textureId);
	}
    }
    else
    {
	renderer_flush();
	textureId = rendererData->Rectangles.List.StartIndex;
	texture_list_add(&rendererData->Rectangles.List, texture, textureId);
    }

    return textureId;
}

void
renderer_batch_init(Renderer2DStatistics* statistics, Shader* shader, Texture2D* whiteTexture, Camera* camera)
{
    VertexBuffer vbo = {};
    IndexBuffer ibo = {};

    Statistics = statistics;
    BatchedShader = shader;
    CurrentCamera = camera;

    RendererData.Rectangles.RRectangles = NULL;
    array_reserve(RendererData.Rectangles.RRectangles, QuadVertexElementCount);
    RendererData.Rectangles.Data = (f32*) memory_allocate(VertexBufferSize);
    RendererData.Rectangles.Indices = (u32*) memory_allocate(IndicesBufferSize);

    RendererData.LinesData = (f32*) memory_allocate(VertexBufferSize);
    RendererData.LinesIndices = (u32*) memory_allocate(IndicesBufferSize);

    GSUCCESS("Allocate Vertices Buffer (size: %lu bytes)\n", VertexBufferSize);
    GSUCCESS("Allocate Indices Buffer (size: %lu bytes)\n", IndicesBufferSize);

    vertex_buffer_allocate(&vbo, VertexBufferSize);
    vertex_buffer_bind(&vbo);
    vertex_buffer_add_layout(&vbo, 0, Float3);
    vertex_buffer_add_layout(&vbo, 0, Float4);
    vertex_buffer_add_layout(&vbo, 0, Float2);
    vertex_buffer_add_layout(&vbo, 0, Float1);
    vertex_buffer_add_layout(&vbo, 0, Float1);

    fill_indices_array(RendererData.Rectangles.Indices, IndicesCount);
    index_buffer_create(&ibo, RendererData.Rectangles.Indices, IndicesCount);
    index_buffer_bind(&ibo);

    vertex_array_create(&RendererData.Rectangles.Vao);
    vertex_array_add_vbo(&RendererData.Rectangles.Vao, vbo);
    vertex_array_add_ibo(&RendererData.Rectangles.Vao, ibo);
    vertex_array_bind(&RendererData.Rectangles.Vao);
    RendererData.Rectangles.List = (TextureList) {
	.StartIndex = 0,
	.MaxTextureSlot = 32,
	.NextTextureIndex = 1
    };
    texture_list_set_immutable(&RendererData.Rectangles.List, whiteTexture);

    renderer_statistics_init(Statistics);
}

force_inline void
renderer_submit_rotated_base(m4 transform, v4 color, Texture2D* texture, EntityID entityId)
{
    v3 positions[4];
    v4 positionsFourDimensional[4];

    m4_mul_v4(transform, BaseVectorPositions[0], positionsFourDimensional[0]);
    m4_mul_v4(transform, BaseVectorPositions[1], positionsFourDimensional[1]);
    m4_mul_v4(transform, BaseVectorPositions[2], positionsFourDimensional[2]);
    m4_mul_v4(transform, BaseVectorPositions[3], positionsFourDimensional[3]);

    v3_v4(positions[0], positionsFourDimensional[0]);
    v3_v4(positions[1], positionsFourDimensional[1]);
    v3_v4(positions[2], positionsFourDimensional[2]);
    v3_v4(positions[3], positionsFourDimensional[3]);

    i32 textureId = texture_list_submit_texture_or_flush(&RendererData, texture);
    fill_rotated_data_array(RendererData.Rectangles.Data, RendererData.Rectangles.DataCount, positions, color, textureId, entityId);

    RendererData.Rectangles.DataCount  += QuadVerticesCount;
    RendererData.Rectangles.IndexCount += 6;
}

void
renderer_submit_rectangle(v3 position, v2 size, v4 color, v2* coords, Texture2D* texture, EntityID entityId)
{
    vassert(0 && "renderer_submit_rectangle");
    i32 textureId = texture_list_submit_texture_or_flush(&RendererData, texture);
    v2 textureAdds;

    v2_assign_xy(textureAdds, textureId, 1);

    fill_data_array(RendererData.Rectangles.Data, RendererData.Rectangles.DataCount, position, size, color, coords != NULL ? coords : DefaultCoords, textureId, entityId);

    RendererData.Rectangles.DataCount  += QuadVerticesCount;
    RendererData.Rectangles.IndexCount += 6;
}

void
renderer_submit_rotated_rectangle(v3 position, v2 size, v4 color, Texture2D* texture, f32 angle, EntityID entityId)
{
    vassert(0 && "renderer_submit_rectangle");
    v3 scaleVec;
    v3 rotation;
    m4 transform;

    v3_assign_xyz(scaleVec, size[0], size[1], 1.0f);
    v3_assign_xyz(rotation, 0.0f, 0.0f, angle);

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
	{ startX, startY },
	{ startX, endY },
	{ endX, endY },
	{ endX, startY }
    };

    renderer_submit_rectangle(position, size, color, coords, atlas->Texture, entityId);
}

void
renderer_submit_dot(v3 position, v4 color)
{
}

i32
_comp_func(const void* a, const void* b)
{
    RenderableRectangle arrect = *((RenderableRectangle*)a);
    RenderableRectangle brrect = *((RenderableRectangle*)b);

    f32 result = arrect.Position[2] - brrect.Position[2];
    return result;
}

void
renderable_rectangle_to_array(RenderableRectangle* rrects, f32* destination)
{
    i32 ind = RendererData.Rectangles.DataCount - 1;

    i32 i, count = array_count(rrects);
    for (i = 0; i < count; ++i)
    {
	RenderableRectangle rrect = rrects[i];

	destination[++ind] = rrect.Position[0][0];
	destination[++ind] = rrect.Position[0][1];
	destination[++ind] = rrect.Position[0][2];
	destination[++ind] = rrect.Color[0];
	destination[++ind] = rrect.Color[1];
	destination[++ind] = rrect.Color[2];
	destination[++ind] = rrect.Color[3];
	destination[++ind] = 0;
	destination[++ind] = 0;
	destination[++ind] = (f32) rrect.TextureID;
	destination[++ind] = rrect.EntityID;

	destination[++ind] = rrect.Position[1][0];
	destination[++ind] = rrect.Position[1][1];
	destination[++ind] = rrect.Position[1][2];
	destination[++ind] = rrect.Color[0];
	destination[++ind] = rrect.Color[1];
	destination[++ind] = rrect.Color[2];
	destination[++ind] = rrect.Color[3];
	destination[++ind] = 0;
	destination[++ind] = 1;
	destination[++ind] = (f32) rrect.TextureID;
	destination[++ind] = rrect.EntityID;

	destination[++ind] = rrect.Position[2][0];
	destination[++ind] = rrect.Position[2][1];
	destination[++ind] = rrect.Position[2][2];
	destination[++ind] = rrect.Color[0];
	destination[++ind] = rrect.Color[1];
	destination[++ind] = rrect.Color[2];
	destination[++ind] = rrect.Color[3];
	destination[++ind] = 1;
	destination[++ind] = 1;
	destination[++ind] = (f32) rrect.TextureID;
	destination[++ind] = rrect.EntityID;

	destination[++ind] = rrect.Position[3][0];
	destination[++ind] = rrect.Position[3][1];
	destination[++ind] = rrect.Position[3][2];
	destination[++ind] = rrect.Color[0];
	destination[++ind] = rrect.Color[1];
	destination[++ind] = rrect.Color[2];
	destination[++ind] = rrect.Color[3];
	destination[++ind] = 1;
	destination[++ind] = 0;
	destination[++ind] = (f32) rrect.TextureID;
	destination[++ind] = rrect.EntityID;

	RendererData.Rectangles.DataCount  += QuadVerticesCount;
	RendererData.Rectangles.IndexCount += 6;
	++Statistics->RectanglesCount;
    }
}

void
renderer_flush()
{
    u32 size;
    glEnable(GL_BLEND);

    shader_bind(BatchedShader);
    vertex_array_bind(&RendererData.Rectangles.Vao);

    texture_list_bind(&RendererData.Rectangles.List);

    DO_ONESF(GDEBUG, "DataCount: %d\n", RendererData.Rectangles.DataCount);

    i32 count = array_count(RendererData.Rectangles.RRectangles);
    //qsort(RendererData.Rectangles.RRectangles, count, sizeof(RenderableRectangle), _comp_func);
    renderable_rectangle_to_array(RendererData.Rectangles.RRectangles, RendererData.Rectangles.Data);

    size = RendererData.Rectangles.DataCount * sizeof(f32);
    vertex_buffer_set_data(RendererData.Rectangles.Vao.Vertex, RendererData.Rectangles.Data, size);

    shader_set_m4(BatchedShader, "u_ViewProjection", 1, 0, (f32*) &CurrentCamera->ViewProjection[0]);
    shader_set_int1(BatchedShader, "u_Textures", RendererData.Rectangles.List.NextTextureIndex, TextureIndices);

    glDrawElements(GL_TRIANGLES, RendererData.Rectangles.IndexCount, GL_UNSIGNED_INT, NULL);

    array_clearv(RendererData.Rectangles.RRectangles, (RenderableRectangle) {});

    RendererData.Rectangles.DataCount  = 0;
    RendererData.Rectangles.IndexCount = 0;
    RendererData.Rectangles.List.NextTextureIndex = 1;

    ++Statistics->DrawCalls;

    texture_list_unbind(&RendererData.Rectangles.List);
    glDisable(GL_BLEND);
}

void
renderer_destroy()
{
    memory_free(RendererData.Rectangles.Data);
    memory_free(RendererData.Rectangles.Indices);

    memory_free(RendererData.LinesData);
    memory_free(RendererData.LinesIndices);
}

Renderer2DStatistics
renderer_get_statistics()
{
    return *Statistics;
}
