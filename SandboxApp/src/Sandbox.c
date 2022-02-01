#include "Sandbox.h"

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdarg.h>

#if ENGINE_DEBUG == 1
 #define BASE_PATH "SandboxApp/assets"
#else
 #define BASE_PATH "assets"
#endif

#define RENDER_TO_FRAMEBUFFER 0

static NativeWindow Window;
f32 Width = 1280.0f;
f32 Height = 720.0f;

Renderer2DStatistics RendererStatistics;
EditorCamera CurrentCamera;
Scene CurrentScene;
FrameBuffer CurrentFramebuffer;
Shader BatchedTextureShader;

Camera* ActiveCamera = NULL;

//Textures
const char* ShaderPath = BASE_PATH"/shaders/BatchedShader.glsl";
const char* TexturePath = BASE_PATH"/textures/atlas/RPGpack_sheet.png";
const char* WhiteTexturePath = asset_texture("default/white_texture.png");

Texture2D* WhiteTexture = NULL;

v4 RedColor    = { 0.8f, 0.1f, 0.1f, 1.0f };
v4 YellowColor = { 1.0f, 1.0f, 0.0f, 1.0f };
v4 YellowColorAlpha = { 1.0f, 1.0f, 0.0f, 0.3f };
v4 BlueColor   = { 0.1f, 0.1f, 0.8f, 1.0f };
v4 GreenColor  = { 0.2f, 0.7f, 0.2f, 0.3f };

void
sandbox_on_attach(NativeWindow window)
{
    f32 aspectRatio = ((f32)window.Width) / window.Height;

    Window = window;
    window_set_vsync(3);
    window_set_icon(&Window, "resources/Logo1.png");

    char* currentDirectory = path_get_current_directory();
    if (currentDirectory != NULL)
    {
	GLOG(GREEN("Current working dir: %s")"\n", currentDirectory);
    }

#if 0
    memory_helper_print_allocations_in_terminal(
	PRINT_ALLOCATION_SOURCE_TYPE_TERMINAL);
#endif

    FrameBufferType* types = NULL;
    array_push(types, FRAMEBUFFER_TYPE_RGBA8);
    // array_push(types, FRAMEBUFFER_TYPE_RGBA8);
    array_push(types, FRAMEBUFFER_TYPE_DEPTH24_STENCIL8);
    framebuffer_create(&CurrentFramebuffer, window.Width, window.Height, types);

    EditorCameraSettings settings = (EditorCameraSettings)
	{
	    .ZoomLevel = 1.0f,
	    .AspectRatio = aspectRatio,
	    .Speed = 5.0f,
	    .Rotation = 0,
	    .IsOrthographic = 1,
	    .Position = { 0, 0, -5.0f },
	    .Axis = { 0, 0, 0 }
	};
    CurrentCamera = editor_camera_create(1000.0f, -10.0f, settings);

    ActiveCamera = &CurrentCamera.InternalCamera;
    char* json = file_read_string("/home/bies/Data/programming/C/SimpleEngine/Def.json");
    deserialize_scene_from_json(&CurrentScene, json);
    free(json);

    Texture2D* whiteTexture = texture2d_create(asset_texture("default/white_texture.png"));
    {
	const char* shaderPath = asset_shader("BatchedShader.glsl");
	ShaderSource shaderSource = shader_load(shaderPath);
	vassert(shaderSource.name);
	BatchedTextureShader = shader_compile(shaderSource);
	if (BatchedTextureShader.ShaderID == -1)
	{
	    GERROR("We fucked up with shader sources!!!\n");
	    return;
	}

	renderer_batch_init(&RendererStatistics, &BatchedTextureShader, whiteTexture, ActiveCamera);
    }

    //Entity camera = entity_create(&ActiveScene, "Camera");
    //m4 ortho;
    //orthographic(-16, 16, -9, 9, -1.f, 1.f, ortho);
    //ECS_ENTITY_ADD_COMPONENT(ActiveScene.World, camera.ID, CameraComponent);
    //ECS_ENTITY_SET_COMPONENT(ActiveScene.World, camera.ID, CameraComponent, CameraComponent_(1, ortho));
}

void sandbox_on_update(f32 timestep)
{
    framebuffer_bind(&CurrentFramebuffer);
    {
	renderer_reset_statistics(&RendererStatistics, timestep);
	renderer_clear(v4_(0.1f, 0.1f, 0.1f, 1.0f));

	{
	    scene_on_update(&CurrentScene, ActiveCamera);
	}

	renderer_flush();
    }
    framebuffer_unbind(&CurrentFramebuffer);
}

void sandbox_on_ui_render()
{
}

void
sandbox_on_event(Event* event)
{
    if (event->Category == KeyCategory)
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	if (window_is_key_pressed(&Window, KEY_ESCAPE))
	{
	    window_set_should_close(&Window, 1);
	}

	event->IsHandled = 1;
    }
}
