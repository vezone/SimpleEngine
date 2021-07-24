#include "Texture2D.h"

#include <glad/glad.h>

#include "OpenGLBase.h"
#include "Utils/Array.h"
#include "Utils/stb_image.h"
#include "Utils/Logger.h"

u32* TexturesCollection = NULL;

Texture2D*
texture2d_create(const char* path)
{
    i32 width, height, channels;
    GLenum dataFormat, internalFormat;
    Texture2D* texture = (Texture2D*) memory_allocate(sizeof(Texture2D));
    texture->Path = path;
    TEXTUREDEBUG("Texture path: %s\n", path);

    //Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);
    if (data == NULL)
    {
	GERROR("Failed to load a texture!");
	texture->Slot = -1;
	return texture;
    }

    i32 stw = 0;
    if (width <= 0 || height <= 0)
    {
	stw = 1;
	GWARNING("width or height of texture == 0!\n");
    }

    texture->Width = width;
    texture->Height = height;
    texture->Channels = channels;

    TEXTUREDEBUG("Channels: %d\n", channels);

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

    glCreateTextures(GL_TEXTURE_2D, 1, &texture->ID);
    glTextureStorage2D(texture->ID, 1, internalFormat, texture->Width, texture->Height);

    glTextureParameteri(texture->ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture->ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(texture->ID, 0, 0, 0, texture->Width, texture->Height, dataFormat, GL_UNSIGNED_BYTE, data);

    if (data)
    {
	stbi_image_free(data);
    }

    array_push(TexturesCollection, texture->ID);

    return texture;
}

Texture2D*
texture2d_create_from_buffer(void* data, u32 width, u32 height, u8 channels)
{
    GLenum dataFormat, internalFormat;
    Texture2D* texture = (Texture2D*) memory_allocate(sizeof(Texture2D));
    texture->Path = "None";

    //Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texture->Width = width;
    texture->Height = height;
    texture->Channels = channels;

    TEXTUREDEBUG("Channels: %d\n", channels);

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

    texture->DataFormat = dataFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &texture->ID);
    glTextureStorage2D(texture->ID, 1, internalFormat, texture->Width, texture->Height);

    glTextureParameteri(texture->ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture->ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture->ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(texture->ID, 0, 0, 0, texture->Width, texture->Height, dataFormat, GL_UNSIGNED_BYTE, data);

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
