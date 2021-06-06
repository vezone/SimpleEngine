#include "EngineEditor.h"

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdarg.h>
#include "EngineControls/Core.h"
#include "EngineControls/FileDialog.h"

#define APP_NAME "Engine Editor"

static NativeWindow g_Window;
char WindowTitle[32];
Renderer2DStatistics g_RendererStatistics;
EditorCamera Camera;
Shader batchedTextureShader;
FrameBuffer Framebuffer;

//Textures
const char* shader_batched_texture_path = asset_shader("BatchedShader.glsl");
const char* texture_anime_chibi = asset_texture("other/anime_chibi.png");
const char* white_texture_path = asset_texture("default/white_texture.png");

Texture2D hazelLogoTexture;
Texture2D chibiTexture;
Texture2D hotlineMiamiTexture;
Texture2D atlasTexture;
TextureAtlas atlas;

vec4 BlueColor  = { 0.1f, 0.1f, 0.8f, 1.0f };
vec4 GreenColor = { 0.2f, 0.7f, 0.2f, 0.3f };
vec4 YellowColor = { 0.2f, 0.7f, 0.7f, 1.0f };

Scene g_Scene;

void engine_editor_on_attach(NativeWindow window)
{
    f32 aspectRatio;
    f32 zoomLevel;
    char path[512];
    char buffer[1024];
    ShaderSource shaderSource;
    Texture2D whiteTexture;

    g_Window = window;
    aspectRatio = window.Width / window.Height;
    zoomLevel = 5.0f;

    Camera = editor_camera_create(aspectRatio * zoomLevel, aspectRatio * zoomLevel, zoomLevel, -zoomLevel);
    Camera.OrthoCamera.AspectRatio = aspectRatio;
    Camera.OrthoCamera.ZoomLevel = zoomLevel;
    Camera.OrthoCamera.Speed = 5.0f;

    window_set_vsync(0);
    GFORMAT(WindowTitle, "%s %f", APP_NAME, Camera.OrthoCamera.ZoomLevel);
    window_set_title(&g_Window, WindowTitle);

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
	GLOG(GREEN("Current working dir: %s")"\n", cwd);
    }
    else
    {
	GERROR("getcwd() error");
	return;
    }

    shaderSource = shader_load(shader_batched_texture_path);

    assert(shaderSource.name);
    batchedTextureShader = shader_compile(shaderSource);
    if (batchedTextureShader.ShaderID == -1)
    {
	GERROR("We fucked up with shader sources!!!\n");
	return;
    }

    chibiTexture = texture2d_create(texture_anime_chibi);
    whiteTexture = texture2d_create(white_texture_path);
    renderer_batch_init(&g_RendererStatistics, &batchedTextureShader, &whiteTexture, &Camera.OrthoCamera);

    if (atlasTexture.Slot == -2)
    {
	atlasTexture = whiteTexture;
    }

    framebuffer_invalidate(&Framebuffer, window.Width, window.Height);

    scene_create(&g_Scene);

    Entity rectangleEntity = entity_create(&g_Scene, "Blue Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent, SpriteComponent_Color(BlueColor, ((v2) { 1.0f, 1.0f })));

    Entity yellowRectangle = entity_create(&g_Scene, "Yellow Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, PositionComponent, PositionComponent_(((v3) { 3.0f, -2.5f, 1.0f })));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent, SpriteComponent_Color(YellowColor, ((v2) { 1.0f, 0.5f })));

    Entity chibi = entity_create(&g_Scene, "Chibi Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, PositionComponent, PositionComponent_(((v3) { 3.0f, 1.5f, 0.0f })));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent, SpriteComponent_Texture(chibiTexture, ((v2) { 2.0f, 2.0f })));
}

void engine_editor_on_update(f32 timestep)
{
    framebuffer_bind(&Framebuffer);

    renderer_reset_statistics(&g_RendererStatistics, timestep);
    renderer_clear((vec4) { 0.1f, 0.1f, 0.1f, 1.0f });

    editor_camera_on_update(&Camera, &g_Window, timestep);
    renderer_clear((vec4) { 0.2f, 0.245f, 0.356f, 1.0f });

    scene_on_update(&g_Scene, timestep);

    renderer_flush();
    framebuffer_unbind();
}

i32 g_IsRendererStatisticDrawing = 0;
ImVec2 g_ViewportSize;
i8 newItem = 0;
i8 openItem = 0;
i8 opt_padding = 0;
i8 opt_fullscreen = 1;
i8 opt_fullscreen_persistant = 1;
i8 is_docspace_open = 1;
i8 g_IsOpenFileDialogOpened = 1;
ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

void engine_editor_on_ui_render()
{
    framebuffer_bind(&Framebuffer);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiWindowClass class;
    if (opt_fullscreen)
    {
	ImGuiViewport* viewport = igGetMainViewport();
	igSetNextWindowPos(viewport->Pos, ImGuiCond_None, (ImVec2){0.0f,0.0f});
	igSetNextWindowSize(viewport->Size, ImGuiCond_None);
	igSetNextWindowViewport(viewport->ID);
	igPushStyleVarFloat(ImGuiStyleVar_WindowRounding, 0.0f);
	igPushStyleVarFloat(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
	dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
    {
	igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, (ImVec2) { 0.0f, 0.0f });
    }

    igBegin("DockSpace Demo", &is_docspace_open, window_flags);

    if (!opt_padding)
	igPopStyleVar(1);

    if (opt_fullscreen)
	igPopStyleVar(2);

    // DockSpace
    ImGuiIO* io = igGetIO();
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
	ImGuiID dockspace_id = igGetIDStr("MyDockSpace");
	igDockSpace(dockspace_id, (ImVec2) { 0.0f, 0.0f }, dockspace_flags, &class);
    }
    else
    {
	//ShowDockingDisabledMessage();
    }

    if (igBeginMenuBar())
    {
	if (igBeginMenu("File", 1))
	{
	    // Disabling fullscreen would allow the window to be moved to the front of other windows,
	    // which we can't undo at the moment without finer window depth/z control.
	    if (igMenuItemBool("Open", "Ctrl + O", 0, 1))
	    {
		GWARNING("Ctrl O\n");
	    }
	    if (igMenuItemBool("New", "Ctrl+N", 0, 1))
	    {
		GWARNING("Ctrl N\n");
	    }
	    igSeparator();

	    igSeparator();

	    if (igMenuItemBool("Close", NULL, false, 0))
		is_docspace_open = 0;

	    igEndMenu();
	}

	if (igBeginMenu("Windows", 1))
	{
	    igSeparator();
	    if (igMenuItemBool("Info", NULL, g_IsRendererStatisticDrawing, 1))
	    {
		g_IsRendererStatisticDrawing = !g_IsRendererStatisticDrawing;
	    }

	    igEndMenu();
	}

	igEndMenuBar();
    }

    i8 worldOutlinerOpen;
    if (igBegin("World Outliner", &worldOutlinerOpen, ImGuiWindowFlags_None))
    {
	igEnd();
    }

    igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, (ImVec2) { 0.0f, 0.0f });
    i8 viewportOpen;
    if (igBegin("Viewport", &viewportOpen, ImGuiWindowFlags_None))
    {
	i8 isWindowFocused = igIsWindowFocused(ImGuiWindowFlags_None);
	i8 isWindowHovered = igIsWindowHovered(ImGuiWindowFlags_None);
	ui_block_event(!isWindowFocused && !isWindowHovered);

	ImVec2 availableRegion;
	igGetContentRegionAvail(&availableRegion);
	if (!ImVec2_Equals(g_ViewportSize, availableRegion))
	{
	    ImVec2_Assign(g_ViewportSize, availableRegion);
	    framebuffer_invalidate(&Framebuffer, g_ViewportSize.x, g_ViewportSize.y);
	}

	igImage((ImTextureID)Framebuffer.ColorAttachment, ImVec2(g_ViewportSize.x, g_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(1,1,1,0));

	igPopStyleVar(1);

	igEnd();
    }

    if (g_IsRendererStatisticDrawing)
    {
	if (igBegin("Renderer statistic", &g_IsRendererStatisticDrawing, ImGuiWindowFlags_None))
	{
	    igText("Frametime: %f ms", 1000 * g_RendererStatistics.Frametime);
	    igText("Draw Calls: %d", g_RendererStatistics.DrawCalls);
	    igText("Rectangles Count: %d", g_RendererStatistics.RectanglesCount);
	    igText("Max object to draw: %d", g_RendererStatistics.MaximumObjectToDraw);
	    igText("Max texture slots: %d", g_RendererStatistics.MaximumTextureSlots);

	    igEnd();
	}
    }
    igEnd();

#if 0
    char* sceneName = NULL;
    if ((sceneName = igFileDialog()) != NULL)
    {
	GWARNING("Load scene: %s\n", sceneName);
    }
#endif

    framebuffer_unbind();
}

void engine_editor_on_event(Event* event)
{
    editor_camera_on_event(&Camera, event);

    if (event->Category == KeyCategory)
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	if (window_is_key_pressed(&g_Window, KEY_ESCAPE))
	{
	    window_set_should_close(&g_Window, 1);
	    event->IsHandled = 1;
	}
	else if (window_is_key_pressed(&g_Window, KEY_SPACE))
	{
	    Camera.OrthoCamera.Position[0] = 3.0f;
	    Camera.OrthoCamera.Position[1] = 1.5f;
	    Camera.OrthoCamera.Position[2] = 0.0f;
	    event->IsHandled = 1;
	}
	else if (window_is_key_pressed(&g_Window, KEY_F11))
	{
	    window_set_fullscreen(&g_Window);
	}
    }
    else if (event->Category == MouseCategory && event->Type == MouseScrolled)
    {
	//check if viewport is focused
	GFORMAT(WindowTitle, "%s %f", APP_NAME, Camera.OrthoCamera.ZoomLevel);
	window_set_title(&g_Window, WindowTitle);
	event->IsHandled = 1;
    }
    else if (event->Category == WindowCategory)
    {
	if (event->Type == WindowResized)
	{
	    WindowResizedEvent* windowEvent = (WindowResizedEvent*)event;
	    framebuffer_invalidate(&Framebuffer, windowEvent->Width, windowEvent->Height);
	    event->IsHandled = 1;
	}
    }
}
