#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "Graphics/Shader.h"
#include "Graphics/Renderer3D/PerspectiveCamera.h"
#include "Graphics/Texture2D.h"
#include "Utils/SimpleStandardLibrary.h"
#include "Math/Math.h"

#if 0
    {
	static PerspectiveCamera g_Camera3D;
	static Shader shader3D;

	g_Camera3D = perspective_camera_create(-10.0f, 10.0f, aspectRatio, 45, v3_(0, 0, 1));
	ShaderSource shaderSource = shader_load(asset_shader("Shader3D.glsl"));
	vassert(shaderSource.name);
	shader3D = shader_compile(shaderSource);
	renderer3d_batch_init(&shader3D, &g_Camera3D);
    }
#endif


void renderer3d_batch_init(Shader* shader, PerspectiveCamera* camera);

/* Flush functions */
void renderer3d_submit();
void renderer3d_flush();

#endif
