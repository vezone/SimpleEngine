#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "cglm/cglm.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer2D/OrthographicCamera.h"
#include "Graphics/Texture2D.h"
#include "Utils/Types.h"
#include "Utils/Array.h"
#include "ECS/ECS.h"
#include "Math/M4.h"

static void
renderer_set_viewport(u32 width, u32 height)
{
    //OpenGL stuff
    glViewport(0, 0, width, height);
}

//0.2f, 0.345f, 0.456f, 1.0f
static void
renderer_clear(vec4 color)
{
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

/*
  Batch renderers
*/
#define MaxObjectToDraw (i64)100
#define MaxTextureSlots 32

/*
  xyz: 3
  color: 4
  texturePos: 2
  textureId: 1
  entityId: 1
*/
#define QuadVertexElementCount (3 + 4 + 2 + 1 + 1)
#define QuadVerticesCount (4 * QuadVertexElementCount)
#define SizeofQuadVertex (QuadVertexElementCount * sizeof(f32))

#define VerticesCount (MaxObjectToDraw * QuadVerticesCount)
#define IndicesCount (MaxObjectToDraw * 6)

#define VertexBufferSize (VerticesCount * sizeof(f32))
#define IndicesBufferSize (IndicesCount * sizeof(u32))

typedef struct Renderer2DStatistics
{
    u32 DrawCalls;
    u32 RectanglesCount;
    f32 Frametime;

    u32 MaximumObjectToDraw;
    u32 MaximumTextureSlots;
} Renderer2DStatistics;

force_inline void
renderer_statistics_init(Renderer2DStatistics* statistics)
{
    //TODO(bies): fix this
    statistics->MaximumObjectToDraw = MaxObjectToDraw;
    statistics->MaximumTextureSlots = MaxTextureSlots;
}

force_inline void
renderer_reset_statistics(Renderer2DStatistics* statistics, f32 frametime)
{
    statistics->DrawCalls = 0;
    statistics->RectanglesCount = 0;
    statistics->Frametime = frametime;
}

typedef struct TextureList
{
    u32 MaxTextureSlot;
    u32 NextTextureIndex;
    u32 StartIndex;
    Texture2D Textures[32];
} TextureList;

typedef struct BatchRenderer2DData
{
    u32 DataCount;
    u32 IndexCount;
    VertexArray Vao;
    TextureList List;
    f32 Data[VerticesCount];
    u32 Indices[IndicesCount];
} BatchRenderer2DData;

void renderer_batch_init(Renderer2DStatistics* statistics, Shader* shader, Texture2D* whiteTexture, OrthographicCamera* camera);

/* Submit functions */

void renderer_submit_rectangle(vec3 position, vec2 size, v4 color, vec2* coords, Texture2D* texture, EntityID entityId);
void renderer_submit_rotated_rectangle(v3 position, v2 size, v4 color, Texture2D* texture, f32 angle, EntityID entityId);
void renderer_submit_rectanglet(m4 transform, v4 color, Texture2D* texture, EntityID entityId);
//functions for texture atlas
void renderer_submit_atlas(vec3 position, vec2 size, v4 color, TextureAtlas* atlas, i32 row, i32 col, EntityID entityId);

/*Dots*/
void renderer_submit_dot(vec3 position, vec4 color);

/* Flush functions */
void renderer_flush();
void renderer_destroy();

Renderer2DStatistics renderer_get_statistics();

#endif
