#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <cglm/cglm.h>
#include "Utils/Types.h"

typedef struct Texture2D
{
    u32 RendererID;
    u32 Width;
    u32 Height;
    u32 Channels;
    u32 Slot;
    u32 DataFormat;
    const char* Path;
} Texture2D;

Texture2D* texture2d_create(const char* path);
Texture2D* texture2d_create_from_buffer(void* data, u32 width, u32 height, u8 channels);
void texture2d_set_data(Texture2D* texture, void* data);
// dont do this
// void texture2d_set_from_file(Texture2D* texture, const char* path);
void texture2d_bind(Texture2D* texture, u32 slot);
void texture2d_unbind(Texture2D* texture);
void texture2d_delete(Texture2D* texture);

typedef struct TextureAtlas
{
    f32 AtlasWidth;
    f32 AtlasHeight;
    f32 TextureWidth;
    f32 TextureHeight;
    Texture2D* Texture;
} TextureAtlas;

void texture_atlas_create(TextureAtlas* atlas, const char* path, vec2 atlasSize, vec2 textureSize);

#endif
