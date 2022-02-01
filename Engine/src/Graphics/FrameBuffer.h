#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <Math/MathTypes.h>

typedef enum FrameBufferType
{
    FRAMEBUFFER_TYPE_NONE,

    FRAMEBUFFER_TYPE_RGBA8,
    FRAMEBUFFER_TYPE_RED_INTEGER,
    FRAMEBUFFER_TYPE_DEPTH24_STENCIL8
} FrameBufferType;

typedef struct FrameBuffer
{
    i8 IsSwapChainTarget;
    u32 Width;
    u32 Height;
    u32 Samples;
    u32 RendererId;
    u32* ColorAttachments;
    u32 DepthAttachment;

    FrameBufferType* ColorAttachmentTypes;
    FrameBufferType DepthAttachmentType;
} FrameBuffer;

void framebuffer_create(FrameBuffer* framebuffer, u32 width, u32 height, FrameBufferType* attachments);
void framebuffer_invalidate(FrameBuffer* framebuffer, u32 width, u32 height);
i32 framebuffer_read_pixel(FrameBuffer* framebuffer, i32 attachment, i32 x, i32 y);
void framebuffer_read_pixel_color(FrameBuffer* framebuffer, i32 attachment, i32 x, i32 y, v4 result);
void framebuffer_bind(FrameBuffer* framebuffer);
void framebuffer_unbind();
void framebuffer_destroy(FrameBuffer* framebuffer);

#endif
