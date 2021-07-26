#include "SimpleEditor.h"

#include <stdarg.h>
#include "EngineControls/FileDialog.h"
#include "EngineControls/FileDialog.h"
#include "EngineControls/ViewportControl.h"
#include "EngineControls/WorldOutliner.h"

static NativeWindow g_Window;
static char WindowTitle[32];
static EditorCamera g_Camera;
static Scene g_Scene;
static FrameBuffer g_Framebuffer;
static Renderer2DStatistics g_RendererStatistics;
static Shader g_Shader;

void
simple_editor_on_attach(NativeWindow window)
{
    f32 aspectRatio = window.Width / window.Height;

    g_Window = window;

    EditorCameraSettings settings = (EditorCameraSettings) { .ZoomLevel = 5.0f, .AspectRatio = aspectRatio, .Speed = 5.0f };
    g_Camera = editor_camera_create(5.0f * aspectRatio, 5.0f * aspectRatio, 5.0f, -5.0f, v3_(0, 0, 0), settings);
    window_set_vsync(3);
    window_set_icon(&g_Window, "resources/Logo1.png");

    char cwd[1024];
	char* currentDirectory = path_get_current_directory();
    if (currentDirectory != NULL)
    {
	GLOG(GREEN("Current working dir: %s")"\n", currentDirectory);
    }

    /*
      NOTE(bies): All Renderer Init things should be on editor side
    */
    // Initialize Renderer
    {
	const char* shaderPath = asset_shader("BatchedShader.glsl");
	ShaderSource shaderSource = shader_load(shaderPath);
	vassert(shaderSource.name);
	g_Shader = shader_compile(shaderSource);
	if (g_Shader.ShaderID == -1)
	{
	    GERROR("We fucked up with shader sources!!!\n");
	    return;
	}
	Texture2D* whiteTexture = texture2d_create(asset_texture("default/white_texture.png"));
	renderer_batch_init(&g_RendererStatistics, &g_Shader, whiteTexture, &g_Camera.Orthographic);
    }

    framebuffer_invalidate(&g_Framebuffer, window.Width, window.Height);

    scene_create(&g_Scene, &g_Camera);

    v4 blueColor   = v4_(0.1f, 0.1f, 0.8f, 1.0f);
    // TODO(bies): подобрать цвета
    v4 yellowColor = v4_(1.f, 1.f, 0.0f, 1.0f);
    // v4 greenColor  = v4_(0.2f, 0.7f, 0.2f, 0.3f);
    Texture2D* chibiTexture = texture2d_create(asset_texture("other/anime_chibi.png"));

    Entity rectangleEntity = entity_create(&g_Scene, "Blue Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, rectangleEntity.ID, SpriteComponent, SpriteComponent_Color(blueColor));

    Entity yellowRectangle = entity_create(&g_Scene, "Yellow Rectangle");
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent);
    // TransformComponent_(position, scale, rotation)
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, TransformComponent, TransformComponent_(v3_(-4.5f, -2.5f, 1.0f), v3_(3.0f, 3.0f, 1.0f), v3_(0.0f, 0.0f, 25.0f)));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, yellowRectangle.ID, SpriteComponent, SpriteComponent_Color(yellowColor));

    Entity chibi = entity_create(&g_Scene, "Chibi Rectangle");
    //error here seg fault
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, TransformComponent, TransformComponent_Position(3.0f, 1.5f, 0.0f));
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, chibi.ID, SpriteComponent, SpriteComponent_Texture(chibiTexture));

    Entity camera = entity_create(&g_Scene, "Camera");
    m4 ortho;
    orthographic(-16, 16, -9, 9, -1.f, 1.f, ortho);
    ECS_ENTITY_ADD_COMPONENT(g_Scene.World, camera.ID, CameraComponent);
    ECS_ENTITY_SET_COMPONENT(g_Scene.World, camera.ID, CameraComponent, CameraComponent_(1, ortho));

    file_dialog_create();
}

void
simple_editor_on_update(f32 timestep)
{
    framebuffer_bind(&g_Framebuffer);
    renderer_reset_statistics(&g_RendererStatistics, timestep);
    renderer_clear(v4_(0.2f, 0.245f, 0.356f, 1.0f));
    renderer_clear(v4_(0.1f, 0.1f, 0.1f, 1.0f));

    viewport_on_update(&g_Camera, &g_Window, timestep);

    scene_on_update(&g_Scene);

    renderer_flush();

    framebuffer_unbind(&g_Framebuffer);
}

bool g_IsRendererStatisticPanelVisible = 0;
bool g_IsStyleWindowVisible = 0;
bool g_IsDockSpaceOpen = 1;
ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

force_inline void
menu_bar()
{
    if (igBeginMenuBar())
    {
	if (igBeginMenu("File", 1))
	{
	    // Disabling fullscreen would allow the window to be moved to the front of other windows,
	    // which we can't undo at the moment without finer window depth/z control.
	    if (igMenuItem_Bool("Open", "Ctrl + O", 0, 1))
	    {
		GWARNING("Ctrl O\n");
	    }
	    if (igMenuItem_Bool("New", "Ctrl+N", 0, 1))
	    {
		GWARNING("Ctrl N\n");
	    }
	    igSeparator();

	    igSeparator();

	    if (igMenuItem_Bool("Close", NULL, 0, 1))
	    {
		g_IsDockSpaceOpen = 0;
	    }

	    igEndMenu();
	}

	if (igBeginMenu("Windows", 1))
	{
	    igSeparator();
	    if (igMenuItem_Bool("Info", NULL, g_IsRendererStatisticPanelVisible, 1))
	    {
		TYPE_REVERSE(g_IsRendererStatisticPanelVisible);
	    }

	    if (igMenuItem_Bool("Style", NULL, g_IsRendererStatisticPanelVisible, 1))
	    {
		TYPE_REVERSE(g_IsStyleWindowVisible);
	    }

	    igEndMenu();
	}

	viewport_menu_item();

	igEndMenuBar();
    }
}

static i32 g_IsPopupShowsUp = 0;
static bool g_PopupDontAsk = 0;

void
igSimplePopup()
{
    g_IsPopupShowsUp = 0;

    // Always center this window when appearing
    if (g_IsPopupShowsUp == 1)
    {
	ImVec2 center;
	ImGuiViewport* igViewport =  igGetMainViewport();
	ImGuiViewport_GetCenter(&center, igViewport);
	igSetNextWindowPos(center, ImGuiCond_Appearing, ImVec2_(0.5f, 0.5f));
    }

    if (igBeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
	igText("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
	igSeparator();

	igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, ImVec2_(0, 0));
	igCheckbox("Don't ask me next time", &g_PopupDontAsk);
	igPopStyleVar(1);

	if (igButton("OK", ImVec2_(120, 0)))
	{
	    application_close();
	}

	igSetItemDefaultFocus();
	igSameLine(50.0f, 100.f);

	if (igButton("Cancel", ImVec2_(120, 0)))
	{
	    igCloseCurrentPopup();
	}

	igEndPopup();
    }
}

force_inline void
renderer_statistic_panel()
{
    if (g_IsRendererStatisticPanelVisible)
    {
	if (igBegin("Renderer statistic", &g_IsRendererStatisticPanelVisible, ImGuiWindowFlags_None))
	{
	    Renderer2DStatistics rendererStatistics = g_RendererStatistics;
	    igText("Frametime: %f ms", 1000 * rendererStatistics.Frametime);
	    igText("Draw Calls: %d", rendererStatistics.DrawCalls);
	    igText("Rectangles Count: %d", rendererStatistics.RectanglesCount);
	    igText("Max object to draw: %d", rendererStatistics.MaximumObjectToDraw);
	    igText("Max texture slots: %d", rendererStatistics.MaximumTextureSlots);
	}

	igEnd();
    }
}

force_inline void
style_panel()
{
    if (g_IsStyleWindowVisible)
    {
	if (igBegin("Style", &g_IsStyleWindowVisible, ImGuiWindowFlags_None))
	{
	    ImGuiStyle* style = igGetStyle();

	    v4 windowBgColor = v4_imvec4(style->Colors[ImGuiCol_WindowBg]);

	    v4 headerColor = v4_imvec4(style->Colors[ImGuiCol_Header]);
	    v4 headerHoveredColor = v4_imvec4(style->Colors[ImGuiCol_HeaderHovered]);
	    v4 headerActiveColor = v4_imvec4(style->Colors[ImGuiCol_HeaderActive]);

	    v4 buttonColor = v4_imvec4(style->Colors[ImGuiCol_Button]);
	    v4 buttonHoveredColor = v4_imvec4(style->Colors[ImGuiCol_ButtonHovered]);
	    v4 buttonActiveColor = v4_imvec4(style->Colors[ImGuiCol_ButtonActive]);

	    v4 frameBgColor = v4_imvec4(style->Colors[ImGuiCol_FrameBg]);
	    v4 frameBgHoveredColor = v4_imvec4(style->Colors[ImGuiCol_FrameBgHovered]);
	    v4 frameBgActiveColor = v4_imvec4(style->Colors[ImGuiCol_FrameBgActive]);

	    v4 tabColor = v4_imvec4(style->Colors[ImGuiCol_Tab]);
	    v4 tabHoveredColor = v4_imvec4(style->Colors[ImGuiCol_TabHovered]);
	    v4 tabActiveColor = v4_imvec4(style->Colors[ImGuiCol_TabActive]);
	    v4 tabUnfocusedColor = v4_imvec4(style->Colors[ImGuiCol_TabUnfocused]);
	    v4 tabUnfocusedActiveColor = v4_imvec4(style->Colors[ImGuiCol_TabUnfocusedActive]);

	    v4 titleBGColor = v4_imvec4(style->Colors[ImGuiCol_TitleBg]);
	    v4 titleBGActiveColor = v4_imvec4(style->Colors[ImGuiCol_TitleBgActive]);
	    v4 titleBGCollapsedColor = v4_imvec4(style->Colors[ImGuiCol_TitleBgCollapsed]);

	    igColorEdit4("Window Background", windowBgColor, ImGuiColorEditFlags_None);

	    igColorEdit4("Header", headerColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Header Hovered", headerHoveredColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Header Active", headerActiveColor, ImGuiColorEditFlags_None);

	    igColorEdit4("Button", buttonColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Button Hovered", buttonHoveredColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Button Active", buttonActiveColor, ImGuiColorEditFlags_None);

	    igColorEdit4("Frame Background", frameBgColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Frame Background Hovered", frameBgHoveredColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Frame Background Active", frameBgActiveColor, ImGuiColorEditFlags_None);

	    igColorEdit4("Tab", tabColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Tab Hovered", tabHoveredColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Tab Active", tabActiveColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Tab Unfocused", tabUnfocusedColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Tab Unfocused Active", tabUnfocusedActiveColor, ImGuiColorEditFlags_None);

	    igColorEdit4("Title Background", titleBGColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Title Background Active", titleBGActiveColor, ImGuiColorEditFlags_None);
	    igColorEdit4("Title Background Collapsed", titleBGCollapsedColor, ImGuiColorEditFlags_None);

	    style->Colors[ImGuiCol_WindowBg] = ImVec4_Array(windowBgColor);

	    style->Colors[ImGuiCol_Header] = ImVec4_Array(headerColor);
	    style->Colors[ImGuiCol_HeaderHovered] = ImVec4_Array(headerHoveredColor);
	    style->Colors[ImGuiCol_HeaderActive] = ImVec4_Array(headerActiveColor);

	    style->Colors[ImGuiCol_Button] = ImVec4_Array(buttonColor);
	    style->Colors[ImGuiCol_ButtonHovered] = ImVec4_Array(buttonHoveredColor);
	    style->Colors[ImGuiCol_ButtonActive] = ImVec4_Array(buttonActiveColor);

	    style->Colors[ImGuiCol_FrameBg] = ImVec4_Array(frameBgColor);
	    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4_Array(frameBgHoveredColor);
	    style->Colors[ImGuiCol_FrameBgActive] = ImVec4_Array(frameBgActiveColor);

	    style->Colors[ImGuiCol_Tab] = ImVec4_Array(tabColor);
	    style->Colors[ImGuiCol_TabHovered] = ImVec4_Array(tabHoveredColor);
	    style->Colors[ImGuiCol_TabActive] = ImVec4_Array(tabActiveColor);
	    style->Colors[ImGuiCol_TabUnfocused] = ImVec4_Array(tabUnfocusedColor);
	    style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4_Array(tabUnfocusedActiveColor);

	    style->Colors[ImGuiCol_TitleBg] = ImVec4_Array(titleBGColor);
	    style->Colors[ImGuiCol_TitleBgActive] = ImVec4_Array(titleBGActiveColor);
	    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4_Array(titleBGCollapsedColor);

	}

	igEnd();
#if 0
	colors[ImGuiCol_TitleBg] = ImVec4_(.15, .1505, .151, 1.0);
	colors[ImGuiCol_TitleBgActive] = ImVec4_(.15, .1505, .151, 1.0);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4_(.95, .1505, .951, 1.0);
#endif
    }
}

void
simple_editor_on_ui_render()
{
    framebuffer_bind(&g_Framebuffer);

    static i8 noPadding = 1;
    static i8 opt_fullscreen = 1;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
    ImGuiWindowClass class;
    if (opt_fullscreen)
    {
	ImGuiViewport* viewport = igGetMainViewport();
	igSetNextWindowPos(viewport->Pos, ImGuiCond_None, ImVec2_(0, 0));
	igSetNextWindowSize(viewport->Size, ImGuiCond_None);
	igSetNextWindowViewport(viewport->ID);
	igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
	igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
	dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	windowFlags |= ImGuiWindowFlags_NoBackground;

    if (noPadding)
    {
	igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0, 0));
    }

    igBegin("DockSpace Demo", &g_IsDockSpaceOpen, windowFlags);

    // DockSpace
    ImGuiIO* io = igGetIO();
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
	ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
	igDockSpace(dockspace_id, ImVec2_(0.0f, 0.0f), dockspace_flags, &class);
    }

    menu_bar();
    world_outliner(&g_Scene);
    viewport(&g_Camera, &g_Framebuffer);
    renderer_statistic_panel();
    style_panel();

    if (g_IsPopupShowsUp)
    {
	if (g_PopupDontAsk)
	{
	    application_close();
	}

	igOpenPopup_Str("Delete?", ImGuiPopupFlags_None);
    }

    igSimplePopup();

    //igShowDemoWindow(NULL);

    if (opt_fullscreen)
	igPopStyleVar(2);

    if (noPadding)
	igPopStyleVar(1);


    // for dockspace
    igEnd();

    framebuffer_unbind();
}

void simple_editor_on_event(Event* event)
{
    switch (event->Category)
    {

    case KeyCategory:
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	switch (keyEvent->KeyCode)
	{
	case KEY_ESCAPE:
	    g_IsPopupShowsUp = 1;
	    event->IsHandled = 1;
	    break;
	}
	break;
    }

    case WindowCategory:
    {
	if (event->Type == WindowShouldBeClosed)
	{
	    application_close();
	}
	break;
    }

    default:
	break;
    }

    viewport_on_event(event);
}
