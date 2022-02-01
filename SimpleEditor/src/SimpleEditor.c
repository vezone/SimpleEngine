#include "SimpleEditor.h"

#include "EditorControls/FileDialog.h"
#include "EditorControls/ViewportControl.h"
#include "EditorControls/WorldOutliner.h"
#include "EditorControls/PhysicsPanel.h"
#include "EditorPanels/ProfilerPanel.h"

static NativeWindow Window;
static char WindowTitle[32];

static Scene CurrentScene;
static EditorCamera CurrentCamera;

static Scene ActiveScene;
static Camera* ActiveCamera = NULL;

static Scene RuntimeScene;

static FrameBuffer CurrentFramebuffer;
static Renderer2DStatistics RendererStatistics;
static Shader CurrentShader;
static i32 IsPhysicsEnabled;

void
simple_editor_on_attach(NativeWindow window)
{
    f32 aspectRatio = window.Width / window.Height;

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
    {
	i32* arr = NULL;
	array_push(arr, 1);
	array_push(arr, 1);
	array_push(arr, 1);
	array_push(arr, 1);
	array_push(arr, 1);
    }
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
	    .Position = { 0, 0, 5.0f },
	    .Axis = { 0, 0, 0 }
	};
    //NOTE(): we always use default declared in editor_camera_update
    CurrentCamera = editor_camera_create(1000.0f, -10.0f, settings);
    //editor_camera_set_orthograhic(&CurrentCamera);

    ActiveCamera = &CurrentCamera.InternalCamera;
    char* json = file_read_string("/home/bies/Data/programming/C/SimpleEngine/Def.json");
    deserialize_scene_from_json(&CurrentScene, json);

    /*
      NOTE(bies): All Renderer Init things should be on editor side
    */
    // Initialize Renderer
    Texture2D* whiteTexture = texture2d_create(asset_texture("default/white_texture.png"));
    {
	const char* shaderPath = asset_shader("BatchedShader.glsl");
	ShaderSource shaderSource = shader_load(shaderPath);
	vassert(shaderSource.name);
	CurrentShader = shader_compile(shaderSource);
	if (CurrentShader.ShaderID == -1)
	{
	    GERROR("We fucked up with shader sources!!!\n");
	    return;
	}

	renderer_batch_init(&RendererStatistics, &CurrentShader, whiteTexture, ActiveCamera);
    }

    //Entity camera = entity_create(&ActiveScene, "Camera");
    //m4 ortho;
    //orthographic(-16, 16, -9, 9, -1.f, 1.f, ortho);
    //ECS_ENTITY_ADD_COMPONENT(ActiveScene.World, camera.ID, CameraComponent);
    //ECS_ENTITY_SET_COMPONENT(ActiveScene.World, camera.ID, CameraComponent, CameraComponent_(1, ortho));

    viewport_create(Window, &CurrentCamera, &CurrentFramebuffer);
    world_outliner_on_attach(whiteTexture);
}

void
simple_editor_on_update(f32 timestep)
{
    framebuffer_bind(&CurrentFramebuffer);
    renderer_reset_statistics(&RendererStatistics, timestep);
    renderer_clear(v4_(0.1f, 0.1f, 0.1f, 1.0f));

    viewport_on_update(timestep);
    profiler_panel_on_update(timestep);

#if DO_LUCHSHIH_VREMEN
    if (IsPhysicsEnabled)
    {

    }
#endif

    // &CurrentCamera
    static i32 isEditorRuntime = 1;
    if (isEditorRuntime)
    {
	scene_on_update(&CurrentScene, ActiveCamera);
    }
    else
    {
    }

    renderer_flush();

    framebuffer_unbind(&CurrentFramebuffer);
}

bool g_IsRendererStatisticPanelVisible = 0;
bool g_IsStyleWindowVisible = 0;
bool g_IsDockSpaceOpen = 1;
ImGuiDockNodeFlags g_DockspaceFlags = ImGuiDockNodeFlags_None;

static bool g_IsNewFileDialogBeenOpened = false;
static bool g_IsSaveFileDialogBeenOpened = 0;
static bool g_IsSaveAsFileDialogBeenOpened = 0;
static i32 g_IsEditorRuntime = 1;

force_inline void
simple_editor_on_file_new()
{
    if (g_IsNewFileDialogBeenOpened)
    {
	return;
    }

    file_dialog_create();

    g_IsNewFileDialogBeenOpened = true;
}

void
save_scene_to_file(Scene* scene, const char* filename)
{
    char* sceneJson = serialize_scene_to_json(scene);
    vassert(sceneJson && "Can't get scene in json format!!!");

    file_write_string(filename, sceneJson, string_builder_count(sceneJson));
    string_builder_free(sceneJson);
}

force_inline void
simple_editor_on_file_save()
{
    g_IsSaveFileDialogBeenOpened = 1;

    save_scene_to_file(&CurrentScene, "Default Scene Actually.json");
}

force_inline void
simple_editor_on_file_save_as()
{
    if (g_IsSaveAsFileDialogBeenOpened)
	return;

    file_dialog_create();

    g_IsSaveAsFileDialogBeenOpened = 1;
}

force_inline i32
simple_editor_is_scene_empty()
{
    return 1;
}

force_inline void
menu_bar()
{
    if (igBeginMenuBar())
    {
	if (igBeginMenu("File", 1))
	{
	    if (igMenuItem_Bool("New", "Ctrl + N", 0, 1))
	    {
		simple_editor_on_file_new();
	    }
	    if (igMenuItem_Bool("Save", "Ctrl + Alt + S", 0, 1))
	    {
		simple_editor_on_file_save();
	    }
	    if (igMenuItem_Bool("Save As", "Ctrl + Shift + S", 0, 1))
	    {
		simple_editor_on_file_save_as();
	    }
	    igSeparator();

	    igSeparator();

	    if (igMenuItem_Bool("Close", NULL, 0, 1))
	    {
		g_IsDockSpaceOpen = 0;
	    }
	    igEndMenu();
	}

	if (igBeginMenu("Project", 1))
	{
	    if (igMenuItem_Bool("Run", NULL, 0, 1))
	    {
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
    }

    igEndMenuBar();
}

static i32 g_IsPopupShowsUp = 0;
static bool g_PopupDontAsk = 0;

void
simple_editor_exit_popup()
{
    g_IsPopupShowsUp = 0;

    if (g_IsPopupShowsUp == 1)
    {
	ImVec2 center;
	ImGuiViewport* igViewport =  igGetMainViewport();
	ImGuiViewport_GetCenter(&center, igViewport);
	igSetNextWindowPos(center, ImGuiCond_Appearing, ImVec2_(0.5f, 0.5f));
    }

    if (igBeginPopupModal("Exit From SimpleEditor", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
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
	    Renderer2DStatistics rendererStatistics = RendererStatistics;
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
    }
}

void
simple_editor_on_ui_render()
{
    framebuffer_bind(&CurrentFramebuffer);

    static i8 noPadding = 1;
    static i8 optFullscreen = 1;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
    ImGuiWindowClass class;
    if (optFullscreen)
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
	g_DockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (g_DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
	windowFlags |= ImGuiWindowFlags_NoBackground;
    if (noPadding)
	igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0, 0));

    igBegin("DockSpace Demo", &g_IsDockSpaceOpen, windowFlags);

    // DockSpace
    ImGuiIO* io = igGetIO();
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
	ImGuiID dockspaceID = igGetID_Str("MyDockSpace");
	igDockSpace(dockspaceID, ImVec2_(0.0f, 0.0f), g_DockspaceFlags, &class);
    }

    menu_bar();
    world_outliner(&CurrentScene, &CurrentCamera);
    viewport(&CurrentScene, &CurrentCamera.InternalCamera);
    renderer_statistic_panel();
    style_panel();
    profiler_panel();
    //profiler_panel();
    //physics2d_panel(&IsPhysicsEnabled);
    {
	const char* selectedFile;
	if (igFileDialog(selectedFile, &g_IsNewFileDialogBeenOpened, FilterFlag_None))
	{
	    GWARNING("FILE->NEW Selected File: %s\n", selectedFile);

	    char* json = file_read_string(selectedFile);
	    deserialize_scene_from_json(&CurrentScene, json);

	    //g_IsNewFileDialogBeenOpened = false;
	}
    }

    {
	const char* selectedFile;
	if (igFileDialog(selectedFile, &g_IsSaveAsFileDialogBeenOpened, FilterFlag_None))
	{
	    GWARNING("FILE->SAVE_AS Selected File: %s\n", selectedFile);

	    save_scene_to_file(&CurrentScene, "Default Scene Actually.json");

	    //g_IsSaveAsFileDialogBeenOpened = 0;
	}
    }

    if (g_IsPopupShowsUp)
    {
	if (g_PopupDontAsk)
	{
	    application_close();
	}

	igOpenPopup_Str("Exit From SimpleEditor", ImGuiPopupFlags_None);
    }

    simple_editor_exit_popup();

#if 0
    igShowDemoWindow(NULL);
#endif

    if (optFullscreen)
	igPopStyleVar(2);
    if (noPadding)
	igPopStyleVar(1);

    // for dockspace
    igEnd();


    framebuffer_unbind();
}

void
simple_editor_on_event(Event* event)
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
	case KEY_N:
	    if (keyEvent->Modificator == MOD_CONTROL)
	    {
		simple_editor_on_file_new();
	    }
	    break;
	case KEY_S:
	    if (keyEvent->Modificator == (MOD_CONTROL | MOD_ALT))
	    {
		simple_editor_on_file_save();
	    }
	    else if (keyEvent->Modificator == (MOD_CONTROL | MOD_SHIFT))
	    {
		simple_editor_on_file_save_as();
	    }
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

void
simple_editor_on_destroy()
{
    shader_delete_collection();
    renderer_destroy();
}
