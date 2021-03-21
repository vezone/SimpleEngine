#pragma once

#include <Utils/Types.h>

typedef struct FrameBuffer
{
    i8 IsSwapChainTarget;
    u32 Width;
    u32 Height;
    u32 Samples;
    u32 RendererId;
    u32 ColorAttachment;
    u32 DepthAttachment;
} FrameBuffer;

void framebuffer_invalidate(FrameBuffer* framebuffer, u32 width, u32 height);
void framebuffer_bind(FrameBuffer* framebuffer);
void framebuffer_unbind();
void framebuffer_destroy(FrameBuffer* framebuffer);
