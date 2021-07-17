#include "EngineEditor.h"

#include <cglm/cglm.h>
#include <unistd.h>
#include <stdarg.h>
#include "EngineControls/Core.h"
#include "EngineControls/FileDialog.h"
#include "EngineControls/FileDialog.h"
#include "EngineControls/ViewportControl.h"

#define APP_NAME "Engine Editor"
static NativeWindow g_Window;
char WindowTitle[32];
EditorCamera g_Camera;
Scene g_Scene;
FrameBuffer g_Framebuffer;
Renderer2DStatistics g_RendererStatistics;
Shader g_Shader;
bool g_IsViewportFocused = 0;

void
engine_editor_on_attach(NativeWindow window)
{
    f32 aspectRatio = window.Width / window.Height;

    g_Window = window;

    g_Camera = editor_camera_create((EditorCameraSettings) {.ZoomLevel = 5.0f, .AspectRatio = aspectRatio, .Speed = 5.0f, .Left = 5.0f * aspectRatio, .Right = 5.0f * aspectRatio, .Bot = 5.0f, .Top = -5.0f});
    window_set_vsync(0);
    GFORMAT(WindowTitle, "%s %f", APP_NAME, g_Camera.ZoomLevel);
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
    Texture2D* whiteTexture = texture2d_create(asset_texture("default/white_texture.png"));
    renderer_batch_init(&g_RendererStatistics, &g_Shader, whiteTexture, &g_Camera.Orthographic);
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

    //const char* comps = COMP_ARGS(11, 2, SpriteComponent, TransformComponent);
    //GERROR("Comps: %s\n", comps);

}

void
engine_editor_on_update(f32 timestep)
{
    framebuffer_bind(&g_Framebuffer);

    renderer_reset_statistics(&g_RendererStatistics, timestep);
    renderer_clear(v4_(0.2f, 0.245f, 0.356f, 1.0f));
    renderer_clear(v4_(0.1f, 0.1f, 0.1f, 1.0f));

    if (g_IsViewportFocused)
	editor_camera_on_update(&g_Camera, &g_Window, timestep);

    scene_on_update(&g_Scene);

    renderer_flush();

    framebuffer_unbind(&g_Framebuffer);
}

bool g_IsRendererStatisticDrawing = 0;
ImVec2 g_ViewportSize;
i8 opt_padding = 0;
i8 opt_fullscreen = 1;
i8 opt_fullscreen_persistant = 1;
bool is_docspace_open = 1;
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
		is_docspace_open = 0;
	    }

	    igEndMenu();
	}

	if (igBeginMenu("Windows", 1))
	{
	    igSeparator();
	    if (igMenuItem_Bool("Info", NULL, g_IsRendererStatisticDrawing, 1))
	    {
		g_IsRendererStatisticDrawing = !g_IsRendererStatisticDrawing;
	    }

	    igEndMenu();
	}

	viewport_menu_item();

	igEndMenuBar();
    }
}

static i32 g_IsPopupShowsUp = 0;

void
igSimplePopup()
{
    // Always center this window when appearing
    g_IsPopupShowsUp = 0;

    ImVec2 center;
    ImGuiViewport* igViewport =  igGetMainViewport();
    ImGuiViewport_GetCenter(&center, igViewport);
    igSetNextWindowPos(center, ImGuiCond_Appearing, ImVec2_(0.5f, 0.5f));

    if (igBeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
	igText("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
	igSeparator();

	static bool dont_ask_me_next_time = false;
	igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, ImVec2_(0, 0));
	igCheckbox("Don't ask me next time", &dont_ask_me_next_time);
	igPopStyleVar(1);

	if (igButton("OK", ImVec2_(120, 0)))
	{
	    application_close();
	}

	igSetItemDefaultFocus();
	igSameLine(50.0f, 12.f);

	if (igButton("Cancel", ImVec2_(120, 0)))
	{
	    igCloseCurrentPopup();
	}

	igEndPopup();
    }
}

typedef struct SceneState
{
    i32 SelectedEntity;
} SceneState;

SceneState g_SceneState;

force_inline void
world_outliner()
{
    bool worldOutlinerOpen;
    if (igBegin("World Outliner", &worldOutlinerOpen, ImGuiWindowFlags_None))
    {
	Entity* entities = scene_get_entities();
	i32 count = array_count(entities);
	for (i32 e = 0; e < count; e++)
	{
	    Entity entity = entities[e];
	    if (igButton(entity.Name, ImVec2_(0, 0)))
	    {
		g_SceneState.SelectedEntity = e;
	    }
	}

	if (igBeginPopupContextWindow("NONE", ImGuiPopupFlags_MouseButtonRight))
	{
	    if (igButton("Add Entity", ImVec2_(0, 0)))
	    {
	    }
	    igEndPopup();
	}

	igEnd();
    }
}

force_inline void
transform_component_panel(Entity entity)
{
    v4 translation;
    v3 rotation;
    v3 scale;
    TransformComponent* transformComponent = ECS_ENTITY_GET_COMPONENT(g_Scene.World, entity.ID, TransformComponent);
    m4_transform_decompose(transformComponent->Transform, translation, rotation, scale);

    igSliderFloat3("Translation", translation, -10.0f, 10.0f, "%f", ImGuiSliderFlags_None);
    igSliderFloat3("Rotation", rotation, 0.0f, 90.0f, "%f", ImGuiSliderFlags_None);
    igSliderFloat3("Scale", scale, 0.0f, 10.0f, "%f", ImGuiSliderFlags_None);

    m4_transform(translation, scale, rotation, transformComponent->Transform);
}

force_inline void
sprite_component_panel(Entity entity)
{
    igText("Selected entity: %d", g_SceneState.SelectedEntity);
    SpriteComponent* spriteComponent = ECS_ENTITY_GET_COMPONENT(g_Scene.World, entity.ID, SpriteComponent);

    //igCheckbox("IsTextured", &spriteComponent->IsTextured);
    igColorEdit4("Color", spriteComponent->Color, ImGuiColorEditFlags_None);

    if (spriteComponent->IsTextured)
    {
	Texture2D* texture = spriteComponent->Texture;
	static f32 iconWidth = 32.0f;
	static f32 iconHeight = 32.0f;
	static ImVec4 backgroundColor = ImVec4_(0, 0, 0, 1);
	static ImVec2 uv0 = ImVec2_(0, 1);
	static ImVec2 uv1 = ImVec2_(1, 0);
	static bool shouldFileDialogBeenOpened = 0;
	if (igImageButton((ImTextureID)texture->RendererID, ImVec2_(iconWidth, iconHeight), uv0, uv1, 0, backgroundColor, ImVec4_(1.0f, 1.0f, 1.0f, 1.0f)))
	{
	    shouldFileDialogBeenOpened = 1;
	}

	const char* selectedFile;
	if (igFileDialog(selectedFile, &shouldFileDialogBeenOpened, FilterFlag_PNG))
	{
	    GWARNING("Selected File: %s\n", selectedFile);
	    texture2d_delete(spriteComponent->Texture);

	    Texture2D* newTexture = texture2d_create(selectedFile);
	    spriteComponent->Texture = newTexture;

	    shouldFileDialogBeenOpened = 0;
	}
    }

}

force_inline void
properties_panel()
{
    bool propertiesPanelOpen;
    if (igBegin("Properties Panel", &propertiesPanelOpen, ImGuiWindowFlags_None))
    {
	Entity* entities = scene_get_entities();
	Entity entity = entities[g_SceneState.SelectedEntity];

	if (igButton("Add Component", ImVec2_(0, 0)))
	{
	    //
	}

	transform_component_panel(entity);
	sprite_component_panel(entity);

	igEnd();
    }
}

force_inline void
viewport()
{
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0.0f, 0.0f));
    bool viewportOpen;
    if (igBegin("Viewport", &viewportOpen, ImGuiWindowFlags_None))
    {
	igSetItemDefaultFocus();

	bool isWindowFocused = igIsWindowFocused(ImGuiWindowFlags_None);
	bool isWindowHovered = igIsWindowHovered(ImGuiWindowFlags_None);
	g_IsViewportFocused = isWindowFocused;

	ui_block_event(!isWindowFocused && !isWindowHovered);

	ImVec2 availableRegion;
	igGetContentRegionAvail(&availableRegion);
	if (!ImVec2_Equals(g_ViewportSize, availableRegion))
	{
	    ImVec2_Assign(g_ViewportSize, availableRegion);
	    framebuffer_invalidate(&g_Framebuffer, g_ViewportSize.x, g_ViewportSize.y);
	}

	igImage((ImTextureID)g_Framebuffer.ColorAttachment, ImVec2_(g_ViewportSize.x, g_ViewportSize.y), ImVec2_(0, 1), ImVec2_(1, 0), ImVec4_(1, 1, 1, 1), ImVec4_(1, 1, 1, 0));

	igPopStyleVar(1);

	igEnd();
    }
}

force_inline void
window_renderer_statistic()
{
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
}

void
engine_editor_on_ui_render()
{
    framebuffer_bind(&g_Framebuffer);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiWindowClass class;
    if (opt_fullscreen)
    {
	ImGuiViewport* viewport = igGetMainViewport();
	igSetNextWindowPos(viewport->Pos, ImGuiCond_None, ImVec2_(0, 0));
	igSetNextWindowSize(viewport->Size, ImGuiCond_None);
	igSetNextWindowViewport(viewport->ID);
	igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
	igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
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
	igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0, 0));
    }

    if (igBegin("DockSpace Demo", &is_docspace_open, window_flags))
    {
	if (!opt_padding)
	    igPopStyleVar(1);

	if (opt_fullscreen)
	    igPopStyleVar(2);

	// DockSpace
	ImGuiIO* io = igGetIO();
	if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
	    ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
	    igDockSpace(dockspace_id, ImVec2_(0.0f, 0.0f), dockspace_flags, &class);
	}

	menu_bar();
	world_outliner();
	properties_panel();
	viewport();
	window_renderer_statistic();

	if (g_IsPopupShowsUp)
	    igOpenPopup_Str("Delete?", ImGuiPopupFlags_None);

	igSimplePopup();
	//igShowDemoWindow(NULL);

	// for dockspace
	igEnd();
    }

    framebuffer_unbind();
}

void engine_editor_on_event(Event* event)
{
    if (g_IsViewportFocused)
	editor_camera_on_event(&g_Camera, event);

    switch (event->Category)
    {

    case KeyCategory:
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	if (window_is_key_pressed(&g_Window, KEY_ESCAPE))
	{
	    g_IsPopupShowsUp = 1;
	    event->IsHandled = 1;
	}
	else if (window_is_key_pressed(&g_Window, KEY_SPACE))
	{
	    g_Camera.Orthographic.Position[0] = 3.0f;
	    g_Camera.Orthographic.Position[1] = 1.5f;
	    g_Camera.Orthographic.Position[2] = 0.0f;
	    event->IsHandled = 1;
	}
	else if (window_is_key_pressed(&g_Window, KEY_F11))
	{
	    window_set_fullscreen(&g_Window);
	    event->IsHandled = 1;
	}
	break;
    }

    case MouseCategory:
    {
	if (event->Type == MouseScrolled)
	{
	    //check if viewport is focused
	    GFORMAT(WindowTitle, "%s %f", APP_NAME, g_Camera.ZoomLevel);
	    window_set_title(&g_Window, WindowTitle);
	    event->IsHandled = 1;
	}
	break;
    }

    case WindowCategory:
    {
	if (event->Type == WindowResized)
	{
	    WindowResizedEvent* windowEvent = (WindowResizedEvent*)event;
	    framebuffer_invalidate(&g_Framebuffer, windowEvent->Width, windowEvent->Height);
	    event->IsHandled = 1;
	}
	else if (event->Type == WindowShouldBeClosed)
	{
	    application_close();
	}
	break;
    }

    }

}
