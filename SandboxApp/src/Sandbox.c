#include "Sandbox.h"

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdarg.h>

// GLOBAL
#define HD 1
#define FULLHD 0
#define DRAW_BASE_SCENE 1
#define UI_TEST 1

#if CG_DEBUG == 1
 #define BASE_PATH "SandboxApp/assets"
#else
 #define BASE_PATH "assets"
#endif

#define RENDER_TO_FRAMEBUFFER 0

#define STOP() static i32 numberfordebugpurposes; scanf("%d", &numberfordebugpurposes)

static NativeWindow g_Window;
char WindowTitle[32];
#if HD == 1
f32 Width = 1280.0f;
f32 Height = 720.0f;
#elif FULLHD == 1
f32 Width = 1920.0f;
f32 Height = 1080.0f;
#else
f32 Width = 960.0f;
f32 Height = 640.0f;
#endif

Renderer2DStatistics Statistics;
OrthographicCamera Camera;
Shader batchedTextureShader;
FrameBuffer Framebuffer;

//Textures
const char* shader_batched_texture_path = BASE_PATH"/shaders/BatchedShader.glsl";
const char* texture_anime_chibi = BASE_PATH"/textures/other/anime_chibi.png";
const char* texture_cherno_hazel_logo = BASE_PATH"/textures/other/hazel.png";
const char* texture_hotline_miami = BASE_PATH"/textures/other/hotline_miami.png";
const char* white_texture_path = BASE_PATH"/textures/default/white_texture.png";
const char* atlas_texture_path = BASE_PATH"/textures/atlas/RPGpack_sheet.png";

Texture2D hazelLogoTexture;
Texture2D chibiTexture;
Texture2D hotlineMiamiTexture;
Texture2D atlasTexture;
TextureAtlas atlas;

vec4 RedColor    = { 0.8f, 0.1f, 0.1f, 1.0f };
vec4 YellowColor = { 1.0f, 1.0f, 0.0f, 1.0f };
vec4 YellowColorAlpha = { 1.0f, 1.0f, 0.0f, 0.3f };
vec4 BlueColor   = { 0.1f, 0.1f, 0.8f, 1.0f };
vec4 GreenColor  = { 0.2f, 0.7f, 0.2f, 0.3f };

void
sandbox_on_attach(NativeWindow window)
{
    f32 aspectRatio;
    f32 zoomLevel;
    char path[512];
    char buffer[1024];
    ShaderSource shaderSource;
    Texture2D whiteTexture;

    g_Window = window;
    aspectRatio = Width / Height;
    zoomLevel = 5.0f;
    //f32 left, f32 right, f32 bot, f32 top
    Camera = orthographic_camera_create(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
    Camera.AspectRatio = aspectRatio;
    Camera.ZoomLevel = zoomLevel;
    Camera.Speed = 5.0f;
    //Camera.Position[1] += 2.0f;

    window_set_vsync(0);
    GFORMAT(WindowTitle, "Demo %f", Camera.ZoomLevel);
    window_set_title(&g_Window, WindowTitle);

    getcwd(buffer, sizeof(buffer));
    GLOG(GREEN("directory: %s\n"), buffer);

    shaderSource = shader_load(shader_batched_texture_path);
    batchedTextureShader = shader_compile(shaderSource);
    if (batchedTextureShader.ShaderID == -1)
    {
	GERROR("We fucked up with shader sources!!!\n");
	return;
    }

    hazelLogoTexture = texture2d_create(texture_cherno_hazel_logo);
    chibiTexture = texture2d_create(texture_anime_chibi);
    hotlineMiamiTexture = texture2d_create(texture_hotline_miami);

    texture_atlas_create(&atlas, atlas_texture_path, (vec2) { 1280.0f, 832.0f }, (vec2) { 64.0f, 64.0f });

    whiteTexture = texture2d_create(white_texture_path);
    renderer_batch_init(&Statistics, &batchedTextureShader, &whiteTexture, &Camera);

    if (atlasTexture.Slot == -2)
    {
	atlasTexture = whiteTexture;
    }

#if RENDER_TO_FRAMEBUFFER == 1
    framebuffer_invalidate(&Framebuffer, Width, Height, 1);
#endif

    GDEBUG("sandbox_on_attach:"GREEN(" succeed")"\n");

}

#if UI_TEST == 1
void sandbox_on_update(f32 timestep)
{
    renderer_reset_statistics(&Statistics, timestep);

    //orthographic_camera_on_update(&Camera, &g_Window, timestep);
    renderer_clear((vec4) { 0.2f, 0.245f, 0.356f, 1.0f });

    renderer_submit_rectangle((vec3) { -3.0f, -1.5f, 0.0f }, (vec2) {1.5f,1.5f}, NULL, &chibiTexture);
    renderer_submit_colored_rectangle((vec3) {-2.0f, 1.0f, 0.0f}, (vec2) { 1.0f, 1.0f }, BlueColor);
    renderer_submit_colored_rotated_rectangle((vec3) {-7.0f, 1.0f, 0.0f}, (vec2) { 1.f, 1.f }, GreenColor, 45);
    renderer_submit_rectangle((vec3) {-3.0f, -2.5f, 0.0f}, (vec2) {1.5f, 1.5f}, NULL, &hotlineMiamiTexture);

    renderer_flush();
}

void sandbox_on_ui_render()
{
#if 0
    if (nk_begin(ctx, "Nucklear window (Movable)", nk_rect(10, 50, 350, 350), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
	nk_layout_row_dynamic(ctx, 120, 1);
	nk_label(ctx, "Label name", NK_TEXT_CENTERED);

	nk_layout_row_dynamic(ctx, 50, 1);
	nk_label(ctx, "", NK_TEXT_LEFT);

	nk_layout_row_static(ctx, 30, 80, 1);
	if (nk_button_label(ctx, "AnyButton"))
	    PRINT("button pressed\n");
    }
    nk_end(ctx);

    if (nk_begin(ctx, "Nucklear window (Static)", nk_rect(650, 50, 350, 350), NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
	nk_layout_row_dynamic(ctx, 50, 1);
	nk_label(ctx, "Label name", NK_TEXT_LEFT);

	nk_layout_row_static(ctx, 30, 80, 1);
	if (nk_button_label(ctx, "AnyButton"))
	    PRINT("button pressed\n");
    }
    nk_end(ctx);
#endif
}

#else
void sandbox_on_update(f32 timestep)
{
#if RENDER_TO_FRAMEBUFFER == 1
    framebuffer_bind(&Framebuffer);
#endif
    renderer_reset_statistics(&Statistics);

    application_camera_on_update(&Camera, &g_Window, timestep);

    renderer_clear((vec4) { 0.2f, 0.245f, 0.356f, 1.0f });

    renderer_submit_rectangle((vec3) {-1.5f, -0.5f, 0.0f}, (vec2) {1.0f, 1.0f}, NULL, &hazelLogoTexture);
    renderer_submit_rectangle((vec3) {-1.5f, -2.5f, 0.0f}, (vec2) {2.0f, 2.0f}, NULL, &chibiTexture);
    renderer_submit_rectangle((vec3) {-3.0f, -2.5f, 0.0f}, (vec2) {1.5f, 1.5f}, NULL, &chibiTexture);
    renderer_submit_rectangle((vec3) {3.0f, -2.5f, 0.0f}, (vec2) {1.5f, 1.5f}, NULL, &hotlineMiamiTexture);
    renderer_submit_rectangle((vec3) {3.0f, -2.5f, 0.0f}, (vec2) {1.5f, 1.5f}, NULL, &hotlineMiamiTexture);

    renderer_submit_colored_rectangle((vec3) {1.0f, 1.0f, 0.0f}, (vec2) { 1.5f, 1.5f }, YellowColorAlpha);
    renderer_submit_colored_rectangle((vec3) {-2.0f, 1.0f, 0.0f}, (vec2) { 1.0f, 1.0f }, BlueColor);

    renderer_submit_colored_rotated_rectangle((vec3) {-6.0f, 1.0f, 0.0f}, (vec2) { 1.5f, 1.5f }, RedColor, 45);
    renderer_submit_colored_rotated_rectangle((vec3) {-7.0f, 1.0f, 0.0f}, (vec2) { 1.f, 1.f }, GreenColor, 45);
    renderer_submit_colored_rotated_rectangle((vec3) {-8.0f, 1.0f, 0.0f}, (vec2) { .5f, .5f }, YellowColor, 45);

    renderer_submit_colored_rotated_rectangle((vec3) {-2.0f, 1.0f, 0.0f}, (vec2) { 2.5f, 2.5f }, YellowColorAlpha, 45);

    {
	static f32 angle = 0.0f;
	angle += timestep * 115.0f;
	angle = FMOD(angle, 360);
	renderer_submit_rotated_rectangle((vec3) { -6.0f, 2.0f, 0.0f }, (vec2) { 0.5f, 0.5f }, angle, &chibiTexture);
    }

    {
	static i32 counter = 0;
	f64 tileSize = 0.3f;
	f64 tileShift = tileSize + tileSize * 0.1f;
	f64 startX = 5.5;
	f64 startY = 5.5;
	f64 endX = 19.5;
	f64 endY = 19.5;
	for (f64 y = startY; y < endY; y += tileShift)
	{
	    for (f64 x = startX; x < endX; x += tileShift)
	    {
		++counter;
		renderer_submit_rectangle((vec3) {x, y, 0.0f}, (vec2) { tileSize, tileSize }, NULL, &chibiTexture);
	    }
	}

	PRINT_ONESF(GLOG, "Counter: %10.5d\n", counter);
    }

    renderer_submit_atlas((vec3) {1.0f, 1.0f, 0.0f}, (vec2) {2.0f, 2.0f}, &atlas, 3, 2);

    renderer_flush();

#if 1
    PRINT("Timestep: %fms, Frames: %d, DrawCalls: %d, RectanglesCount: %d\r", 1000*timestep, (i32)(1 / timestep), Statistics.DrawCalls, Statistics.RectanglesCount);
#endif

#if RENDER_TO_FRAMEBUFFER == 1
    framebuffer_unbind();
#endif
}
#endif

void
sandbox_on_event(Event* event)
{
    //orthographic_camera_on_event(&Camera, event);

    if (event->Category == KeyCategory)
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	if (window_is_key_pressed(&g_Window, KEY_ESCAPE))
	{
	    window_set_should_close(&g_Window, 1);
	}

	event->IsHandled = 1;
    }
    else if (event->Category == MouseCategory && event->Type == MouseScrolled)
    {
	GFORMAT(WindowTitle, "Demo %f", Camera.ZoomLevel);
	window_set_title(&g_Window, WindowTitle);
    }
}
