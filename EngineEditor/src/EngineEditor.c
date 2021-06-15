#include "EngineEditor.h"

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdarg.h>
#include "EngineControls/Core.h"
#include "EngineControls/FileDialog.h"

#define APP_NAME "Engine Editor"
static NativeWindow g_Window;
char WindowTitle[32];
EditorCamera Camera;
Scene g_Scene;
FrameBuffer g_Framebuffer;
Renderer2DStatistics g_RendererStatistics;
Shader g_Shader;

void engine_editor_on_attach(NativeWindow window)
{
    f32 aspectRatio = window.Width / window.Height;

    g_Window = window;

    Camera = editor_camera_create(aspectRatio * 5.0f, aspectRatio * 5.0f, 5.0f, -5.0f);
    Camera.OrthoCamera.AspectRatio = aspectRatio;
    Camera.OrthoCamera.ZoomLevel = 5.0f;
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

    /*
       NOTE(bies): All Renderer Init things should be on editor side
    */
    // Initialize Renderer
    const char* shaderPath = asset_shader("BatchedShader.glsl");
    ShaderSource shaderSource = shader_load(shaderPath);
    assert(shaderSource.name);
    g_Shader = shader_compile(shaderSource);
    if (g_Shader.ShaderID == -1)
    {
	GERROR("We fucked up with shader sources!!!\n");
	return;
    }
    Texture2D whiteTexture = texture2d_create(asset_texture("default/white_texture.png"));
    renderer_batch_init(&g_RendererStatistics, &g_Shader, &whiteTexture, &Camera.OrthoCamera);
    framebuffer_invalidate(&g_Framebuffer, window.Width, window.Height);

    scene_create(&g_Scene, &Camera);

    v4 blueColor   = v4_(0.1f, 0.1f, 0.8f, 1.0f);
    v4 greenColor  = v4_(0.2f, 0.7f, 0.2f, 0.3f);
    v4 yellowColor = v4_(0.2f, 0.7f, 0.7f, 1.0f);
    Texture2D chibiTexture = texture2d_create(asset_texture("other/anime_chibi.png"));

    Entity rectangleEntity = entity_create(&g_Scene, "Blue Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent, SpriteComponent_Color(blueColor, v2_(1.0f, 1.0f )));

    Entity yellowRectangle = entity_create(&g_Scene, "Yellow Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, PositionComponent, PositionComponent_(v3_(3.0f, -2.5f, 1.0f)));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent, SpriteComponent_Color(yellowColor, v2_(1.0f, 0.5f)));

    Entity chibi = entity_create(&g_Scene, "Chibi Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, PositionComponent, PositionComponent_(v3_(3.0f, 1.5f, 0.0f)));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent, SpriteComponent_Texture(chibiTexture, v2_(2.0f, 2.0f)));
}

void engine_editor_on_update(f32 timestep)
{
    framebuffer_bind(&g_Framebuffer);

    renderer_reset_statistics(&g_RendererStatistics, timestep);
    renderer_clear(v4_(0.2f, 0.245f, 0.356f, 1.0f));
    renderer_clear(v4_(0.1f, 0.1f, 0.1f, 1.0f));

    editor_camera_on_update(&Camera, &g_Window, timestep);
    scene_on_update(&g_Scene);

    renderer_flush();

    framebuffer_unbind(&g_Framebuffer);
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
    framebuffer_bind(&g_Framebuffer);

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
	Entity* entities = scene_get_entities();
	i32 count = array_count(entities);
	for (i32 i = 0; i < count; i++)
	{
	    Entity entity = entities[i];
	    // for each entity
	    if (igCollapsingHeaderBoolPtr(entity.Name, NULL, ImGuiWindowFlags_NoCollapse))
	    {
		// ECS_ENTITY_GET_COMPONENTS();
	    }
	}

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
	    framebuffer_invalidate(&g_Framebuffer, g_ViewportSize.x, g_ViewportSize.y);
	}

	igImage((ImTextureID)g_Framebuffer.ColorAttachment, ImVec2(g_ViewportSize.x, g_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(1,1,1,0));

	igPopStyleVar(1);

	igEnd();
    }

    if (g_IsRendererStatisticDrawing)
    {
	if (igBegin("Renderer statistic", &g_IsRendererStatisticDrawing, ImGuiWindowFlags_None))
	{
	    Renderer2DStatistics rendererStatistics = g_RendererStatistics;
	    igText("Frametime: %f ms", 1000 * rendererStatistics.Frametime);
	    igText("Draw Calls: %d", rendererStatistics.DrawCalls);
	    igText("Rectangles Count: %d", rendererStatistics.RectanglesCount);
	    igText("Max object to draw: %d", rendererStatistics.MaximumObjectToDraw);
	    igText("Max texture slots: %d", rendererStatistics.MaximumTextureSlots);

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
	    framebuffer_invalidate(&g_Framebuffer, windowEvent->Width, windowEvent->Height);
	    event->IsHandled = 1;
	}
    }
}
