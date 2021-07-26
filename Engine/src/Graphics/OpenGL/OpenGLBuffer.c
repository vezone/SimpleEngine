#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include "OpenGLBuffer.h"

void
vertex_buffer_set_data(VertexBuffer* buffer, f32* data, u32 size)
{
    buffer->Vertices = data;
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void
vertex_buffer_create(VertexBuffer* buffer, f32* vertices, u32 size)
{
    glGenBuffers(1, &(buffer->RendererID));
    glBindBuffer(GL_ARRAY_BUFFER, buffer->RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    buffer->Elements = NULL;
    buffer->Vertices = vertices;
}

void
vertex_buffer_allocate(VertexBuffer* buffer, u32 size)
{
    glGenBuffers(1, &(buffer->RendererID));
    glBindBuffer(GL_ARRAY_BUFFER, buffer->RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);

    buffer->Elements = NULL;
    buffer->Vertices = NULL;
}

force_inline void
stride_update(VertexBuffer* buffer)
{
    //0 12 24
    i32 i;
    i32 offset;
    i32 buffer_elements_length;

    offset = 0;
    buffer->Stride = 0;

    // o: 0, stride: 12
    // o: 12, stride: 24
    buffer_elements_length = array_len(buffer->Elements);
    for (i = 0; i < buffer_elements_length; i++)
    {
	buffer->Elements[i].Offset = offset;
	offset += buffer->Elements[i].Size;
	buffer->Stride += buffer->Elements[i].Size;
    }
}

void
vertex_buffer_add_layout(VertexBuffer* buffer, i8 isNormalized, DataType type)
{
    BufferElement element = {
	.IsNormilized = isNormalized,
	.Type = type,
	.Size = data_type_get_size(type),
	.Count = data_type_get_count(type)
    };

    array_push(buffer->Elements, element);
    stride_update(buffer);
}

void
vertex_buffer_bind(VertexBuffer* vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo->RendererID);
}

void
vertex_buffer_unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
index_buffer_create(IndexBuffer* buffer, u32* indices, u32 count)
{
    buffer->Count = count;
    buffer->Indices = indices;
    glGenBuffers(1, &(buffer->RendererID));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
}

void
index_buffer_bind(IndexBuffer* buffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->RendererID);
}

void
index_buffer_unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


#endif
