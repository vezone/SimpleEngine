#include "Buffer.h"

#include <stdlib.h>
#include "Utils/Array.h"
#include "Utils/Logger.h"

static void
buffer_element_print(BufferElement element)
{
    BUFFERDEBUG("Size: %d, Count: %d, Offset: %d\n", element.Size, element.Count, element.Offset);
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

static void
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

void vertex_array_create(VertexArray* va)
{
    glCreateVertexArrays(1, &(va->RendererID));
}

void vertex_array_add_vbo(VertexArray* va, VertexBuffer vbo)
{
    va->Vertex = &vbo;

    glBindVertexArray(va->RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, va->Vertex->RendererID);
    BUFFERDEBUG("Stride: %d\n", vbo.Stride);

    BufferElement* layout = vbo.Elements;
    for (i32 i = 0; i < array_len(layout); i++)
    {
	BufferElement element = layout[i];

	buffer_element_print(element);
	glEnableVertexAttribArray(i);
	glVertexAttribPointer(i, element.Count, GL_FLOAT, element.IsNormilized, vbo.Stride, (const void*)element.Offset);
    }
}

void vertex_array_add_ibo(VertexArray* va, IndexBuffer ibo)
{
    va->Index = ibo;
    glBindVertexArray(va->RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.RendererID);
}

void vertex_array_bind(VertexArray* va)
{
    glBindVertexArray(va->RendererID);
}

void vertex_array_unbind()
{
    glBindVertexArray(0);
}
