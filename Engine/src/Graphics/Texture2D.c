#include "Texture2D.h"

#include <glad/glad.h>

#include "OpenGLBase.h"
#include "Utils/stb_image.h"

u32* TexturesCollection = NULL;

Texture2D*
texture2d_create_from_buffer(void* data, u32 width, u32 height, u8 channels)
{
    GLenum dataFormat, internalFormat;
    Texture2D* texture = (Texture2D*) memory_allocate(sizeof(Texture2D));

    //Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (channels == 3)
    {
	dataFormat = GL_RGB;
	internalFormat = GL_RGB8;
    }
    else if (channels == 4)
    {
	dataFormat = GL_RGBA;
	internalFormat = GL_RGBA8;
    }

    texture->Path = "None";
    texture->Width = width;
    texture->Height = height;
    texture->Channels = channels;
    texture->DataFormat = dataFormat;
    texture->Slot = -1;
    GDEBUG("Channels: %d\n", channels);

    glCreateTextures(GL_TEXTURE_2D, 1, &texture->ID);
    glTextureStorage2D(texture->ID, 1, internalFormat, texture->Width, texture->Height);

    glTextureParameteri(texture->ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture->ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE/*GL_REPEAT*/);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE/*GL_REPEAT*/);

    glTextureSubImage2D(texture->ID, 0, 0, 0, texture->Width, texture->Height, dataFormat, GL_UNSIGNED_BYTE, data);

    return texture;
}

Texture2D*
texture2d_create(const char* path)
{
    i32 width, height, channels;
    GLenum dataFormat, internalFormat;
    GDEBUG("Texture path: %s\n", path);

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);
    if (data == NULL)
    {
	assert(0 && "Failed to load a texture!");
    }
    Texture2D* texture = texture2d_create_from_buffer(data, width, height, channels);
    texture->Path = path;

    array_push(TexturesCollection, texture->ID);

    return texture;
}

void
texture2d_set_data(Texture2D* texture, void* data)
{
    glBindTextureUnit(texture->Slot, texture->ID);
    glTextureSubImage2D(texture->ID, 0, 0, 0, texture->Width, texture->Height, texture->DataFormat, GL_UNSIGNED_BYTE, data);
}

void
texture2d_bind(Texture2D* texture, u32 slot)
{
    texture->Slot = slot;
    glBindTextureUnit(slot, texture->ID);
}

void
texture2d_unbind(Texture2D* texture)
{
    glBindTextureUnit(texture->Slot, 0);
}

void
texture2d_delete(Texture2D* texture)
{
    glDeleteTextures(1, &texture->ID);
}

void
texture_atlas_create(TextureAtlas* atlas, const char* path, v2 atlasSize, v2 textureSize)
{
    atlas->AtlasWidth = atlasSize[0];
    atlas->AtlasHeight = atlasSize[1];
    atlas->TextureWidth = textureSize[0];
    atlas->TextureHeight = textureSize[1];
    atlas->Texture = texture2d_create(path);
}
