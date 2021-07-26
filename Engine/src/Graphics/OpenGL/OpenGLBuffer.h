#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include "Utils/Types.h"

#if 0
typedef struct OpenGLBuffer
{
    VertexBuffer* VBO;
} OpenGLBuffer;

void vertex_buffer_set_data(VertexBuffer* buffer, f32* data, u32 size);
void vertex_buffer_create(VertexBuffer* buffer, f32* vertices, u32 size);
void vertex_buffer_allocate(VertexBuffer* buffer, u32 size);
void vertex_buffer_add_layout(VertexBuffer* buffer, i8 isNormalized, DataType type);
void vertex_buffer_bind(VertexBuffer* buffer);
void vertex_buffer_unbind();
#endif

#endif
