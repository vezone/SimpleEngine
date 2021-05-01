#ifndef ASSET_H
#define ASSET_H

#define BASE_ASSET_PATH "assets"
#define asset(path) BASE_ASSET_PATH path
#define asset_shader(shader) asset("/shaders/") shader
#define asset_texture(texture) asset("/textures/") texture
#define asset_font(font) asset("/fonts/") font

#endif
