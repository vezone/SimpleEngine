#ifndef BUFFER_H
#define BUFFER_H

#include "Utils/SimpleStandardLibrary.h"
#include "OpenGLBase.h"

typedef enum DataType
{
    Float1 = 0, Float2, Float3, Float4,
    Int1, Int2, Int3, Int4
} DataType;

//typedef struct BufferLayout {
//    BufferElement* Elements;
//    i32 Stride;
//} BufferLayout;

typedef struct BufferElement
{
    i8 IsNormalized;
    DataType Type;
    i32 Size;
    i32 Count;
    i32 Offset;
} BufferElement;

typedef struct VertexBuffer
{
    u32 RendererID;
    i32 Stride;
    f32* Vertices;
    BufferElement* Elements;
} VertexBuffer;

typedef struct IndexBuffer
{
    u32 RendererID;
    u32 Count;
    u32* Indices;
} IndexBuffer;

typedef struct VertexArray
{
    u32 RendererID;
    VertexBuffer* Vertex;
    IndexBuffer Index;
} VertexArray;

force_inline u32
data_type_get_size(DataType type)
{
    switch (type)
    {
    case Float1: return 4;
    case Float2: return 4 * 2;
    case Float3: return 4 * 3;
    case Float4: return 4 * 4;
    case Int1: return 4;
    case Int2: return 4 * 2;
    case Int3: return 4 * 3;
    case Int4: return 4 * 4;
    }
    return 0;
}

force_inline u32
data_type_get_count(DataType type)
{
    switch (type)
    {
    case Float1: return 1;
    case Float2: return 2;
    case Float3: return 3;
    case Float4: return 4;
    case Int1: return 1;
    case Int2: return 2;
    case Int3: return 3;
    case Int4: return 4;
    }
}

force_inline void
vertex_buffer_set_data(VertexBuffer* buffer, f32* data, u32 size)
{
    buffer->Vertices = data;
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void vertex_buffer_create(VertexBuffer* buffer, f32* vertices, u32 size);
void vertex_buffer_allocate(VertexBuffer* buffer, u32 size);
void vertex_buffer_add_layout(VertexBuffer* buffer, i8 isNormalized, DataType type);
void vertex_buffer_bind(VertexBuffer* buffer);
void vertex_buffer_unbind();

void index_buffer_create(IndexBuffer* buffer, u32* indices, u32 size);
void index_buffer_bind(IndexBuffer* buffer);
void index_buffer_unbind();

void vertex_array_create(VertexArray* va);
void vertex_array_add_vbo(VertexArray* va, VertexBuffer vbo);
void vertex_array_add_ibo(VertexArray* va, IndexBuffer ibo);
void vertex_array_bind(VertexArray* va);
void vertex_array_unbind();

force_inline void
vertex_array_destroy(VertexArray* va)
{
    glDeleteVertexArrays(1, &va->RendererID);
}

#endif
