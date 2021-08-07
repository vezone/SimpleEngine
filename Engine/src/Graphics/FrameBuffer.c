#include "FrameBuffer.h"
#include <glad/glad.h>
#include <assert.h>
#include <stdlib.h>
#include "Utils/Logger.h"
#include "Utils/Array.h"

force_inline GLint
framebuffer_format_to_opengl(FrameBufferType type)
{
    switch (type)
    {
    case FRAMEBUFFER_TYPE_RGBA8:
	return GL_RGBA;

    case FRAMEBUFFER_TYPE_RED_INTEGER:
	return GL_RED_INTEGER;

    case FRAMEBUFFER_TYPE_DEPTH24_STENCIL8:
	return GL_DEPTH24_STENCIL8;
    }

    vassert(0 && "Can't get a opengl format from format! Probably unsupported framebuffer type!");
    return 0;
}

force_inline GLenum
opengl_format_to_internal_format(GLenum format)
{
    switch (format)
    {
    case GL_RGBA:
	return GL_RGBA8;
    case GL_RED_INTEGER:
	return GL_R32I;
    case GL_DEPTH24_STENCIL8:
	return GL_DEPTH_STENCIL;
    }

    vassert(0 && "Can't get a enum from format! Probably unsupported format!");
    return 0;
}

force_inline GLenum
opengl_format_to_type(GLint format)
{
    switch (format)
    {
    case GL_RGBA:
	return GL_UNSIGNED_BYTE;
    case GL_RED_INTEGER:
	return GL_UNSIGNED_BYTE;
    case GL_DEPTH24_STENCIL8:
	return GL_UNSIGNED_INT_24_8;
    }

    vassert(0 && "Can't get a type from format! Probably unsupported format!");
    return 0;
}

#include "Utils/HashTable.h"
#include "Utils/Logger.h"
struct { const char* Key; i32 Value; }* g_DebugTable;
#define REGISTER_FORMAT(f) shash_put((g_DebugTable), #f, (f))
#define GET_FORMAT(f)

force_inline void
framebuffer_attachments_validate(FrameBuffer* framebuffer, FrameBufferType* attachmentTypes)
{
    i32 depthAttachmentsCount = 0;
    i32 colorAttachmentsCount = 0;
    i32 count = array_count(attachmentTypes);
    GERROR("count: %d\n", count);

    vassert(count >= 2 && "At least 2 attachment types needed!");
    framebuffer->ColorAttachmentTypes = NULL;
    framebuffer->ColorAttachments = NULL;
    array_reserve(framebuffer->ColorAttachments, count);

    g_DebugTable = NULL;
    shash_put(g_DebugTable, TO_STRING(GL_RGBA8), GL_RGBA8);
    shash_put(g_DebugTable, TO_STRING(GL_RED_INTEGER), GL_RED_INTEGER);
    shash_put(g_DebugTable, TO_STRING(GL_DEPTH24_STENCIL8), GL_DEPTH24_STENCIL8);

    GERROR("%s: %d\n", TO_STRING(GL_RGBA8), shash_get((g_DebugTable), TO_STRING(GL_RGBA8)));
    GERROR("%s: %d\n", TO_STRING(GL_RED_INTEGER), shash_get((g_DebugTable), TO_STRING(GL_RED_INTEGER)));
    GERROR("%s: %d\n", TO_STRING(GL_DEPTH24_STENCIL8), shash_get((g_DebugTable), TO_STRING(GL_DEPTH24_STENCIL8)));

    for (i32 i = 0; i < count; i++)
    {
	FrameBufferType attachmentType = attachmentTypes[i];
	switch (attachmentType)
	{
	case FRAMEBUFFER_TYPE_DEPTH24_STENCIL8:
	    ++depthAttachmentsCount;
	    framebuffer->DepthAttachmentType = attachmentType;
	    GERROR("DepthAttachmentType: %d\n", attachmentType);
	    break;
	default:
	    ++colorAttachmentsCount;
	    array_push(framebuffer->ColorAttachmentTypes, attachmentType);
	    array_push(framebuffer->ColorAttachments, 0);
	    GERROR("ColorAttachmentsTypes: %d\n", attachmentType);
	    break;
	}
    }

    vassert(depthAttachmentsCount == 1 && "Only 1 depth buffer attachment needed for FrameBuffer!");
    vassert(colorAttachmentsCount >= 1 && "At least 1 color attachment needed for FrameBuffer");
}

void
framebuffer_create(FrameBuffer* framebuffer, u32 width, u32 height, FrameBufferType* attachments)
{
    framebuffer_attachments_validate(framebuffer, attachments);

    framebuffer_invalidate(framebuffer, width, height);
}

void
framebuffer_invalidate(FrameBuffer* framebuffer, u32 width, u32 height)
{
    vassert(framebuffer->ColorAttachmentTypes && "You need framebuffer attachments if you want framebuffer to work!");

    if (framebuffer->RendererId)
    {
	framebuffer_destroy(framebuffer);
    }

    glCreateFramebuffers(1, &framebuffer->RendererId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->RendererId);

    i32 count = array_count(framebuffer->ColorAttachmentTypes);
    glCreateTextures(GL_TEXTURE_2D, count, framebuffer->ColorAttachments);

    for (i32 i = 0; i < count; i++)
    {
	FrameBufferType attachmentType = framebuffer->ColorAttachmentTypes[i];
	glBindTexture(GL_TEXTURE_2D, framebuffer->ColorAttachments[i]);
	I32P(framebuffer->ColorAttachments[i]);

	switch (attachmentType)
	{
	case FRAMEBUFFER_TYPE_RGBA8:
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	    break;
	case FRAMEBUFFER_TYPE_RED_INTEGER:
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	    break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->ColorAttachments[i], 0);

#if 0
	GERROR("Color attachment: %d\n", attachmentType);
	i32 openglFormat = framebuffer_format_to_opengl(attachmentType);
	i32 internalFormat = opengl_format_to_internal_format(openglFormat);
	GLenum type = opengl_format_to_type(openglFormat);
	GERROR("OpenGL Type: %d %d\n", openglFormat, GL_RED_INTEGER);

	glBindTexture(GL_TEXTURE_2D, framebuffer->ColorAttachments[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->ColorAttachments[i], 0);
#endif
    }

    if (framebuffer->DepthAttachmentType != FRAMEBUFFER_TYPE_NONE)
    {
	FrameBufferType attachmentType = framebuffer->DepthAttachmentType;
	GLenum openglFormat = framebuffer_format_to_opengl(attachmentType);
	i32 internalFormat = opengl_format_to_internal_format(openglFormat);
	GLenum type = opengl_format_to_type(openglFormat);

	switch (framebuffer->DepthAttachmentType)
	{
	case FRAMEBUFFER_TYPE_DEPTH24_STENCIL8:
	    glBindTexture(GL_TEXTURE_2D, framebuffer->DepthAttachment);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0, GL_DEPTH24_STENCIL8, type, NULL);

	    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer->DepthAttachment, 0);
	    break;
	}
    }

    if (count > 1)
    {
	vassert(count <= 4 && "We are not supporting more then 4 color attachments at the same time!");

	GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(count, buffers);
    }
    else if (count == 0)
    {
	glDrawBuffer(GL_NONE);
    }

    vassert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Failed in framebuffer creation !!! :(");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer->Width = width;
    framebuffer->Height = height;
}

i32
framebuffer_read_pixel(FrameBuffer* framebuffer, i32 attachment, i32 x, i32 y)
{
    i32 pixelData;
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void
framebuffer_read_pixel_color(FrameBuffer* framebuffer, i32 attachment, i32 x, i32 y, v4 result)
{
    u8 data[4];
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    // argb
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    result[0] = data[0] / 255.0f;
    result[1] = data[1] / 255.0f;
    result[2] = data[2] / 255.0f;
    result[3] = data[3] / 255.0f;
}

void
framebuffer_bind(FrameBuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->RendererId);
    glViewport(0, 0, framebuffer->Width, framebuffer->Height);
}

void
framebuffer_unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
framebuffer_destroy(FrameBuffer* framebuffer)
{
    GWARNING("Destroy framebuffer!\n");

    glDeleteFramebuffers(1, &framebuffer->RendererId);
    glDeleteTextures(array_count(framebuffer->ColorAttachments), framebuffer->ColorAttachments);
    glDeleteTextures(1, &framebuffer->DepthAttachment);
    array_clear(framebuffer->ColorAttachments);
    framebuffer->DepthAttachment = 0;
}
