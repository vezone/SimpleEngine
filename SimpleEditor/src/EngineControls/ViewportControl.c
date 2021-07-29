#include "ViewportControl.h"
#include <UI/ui.h>
#include <ECS/Components/SpriteComponent.h>
#include <ECS/Components/TransformComponent.h>
#include "WorldOutliner.h"

ImVec2 g_ViewportSize;
static NativeWindow g_Window;
static EditorCamera* g_Camera;
static FrameBuffer* g_Framebuffer;

static bool g_IsViewportFocused = false;
static bool g_IsViewportHovered = false;
static bool g_IsCameraSettingsOpen = false;

static i32 g_OperationType = TRANSLATE;
static i32 g_GizmoRotationKeyBinding = KEY_R;
static i32 g_GizmoTranslationKeyBinding = KEY_T;
static i32 g_GizmoScaleKeyBinding = KEY_S;
static i32 g_SnapKeyBinding = KEY_LEFT_CONTROL;

force_inline void
guizmo(Scene* scene, Entity selectedEntity, EditorCamera* camera)
{
    ImVec2 windowPos;
    f32 width;
    f32 height;

    width = igGetWindowWidth();
    height = igGetWindowHeight();
    igGetWindowPos(&windowPos);

    ImGuizmo_SetOrthographic(true);
    ImGuizmo_SetDrawlist(igGetWindowDrawList());

    ImGuizmo_SetRect(windowPos.x, windowPos.y, width, height);

    TransformComponent* transformComponent = ECS_ENTITY_GET_COMPONENT(scene->World, selectedEntity.ID, TransformComponent);

    static v3 snap;
    static bool useSnap = false;

    if (window_is_key_pressed(&g_Window, g_SnapKeyBinding))
	useSnap = true;
    else
	useSnap = false;

    if (useSnap)
    {
	if (g_OperationType == TRANSLATE)
	{
	    v3_set(snap, 0.5f);
	}
	else if (g_OperationType == ROTATE)
	{
	    v3_set(snap, 5.0f);
	}
	else if (g_OperationType == SCALE)
	{
	    v3_set(snap, 0.1f);
	}
    }

    ImGuizmo_Manipulate(camera->Orthographic.ViewMatrix, camera->Orthographic.ProjectionMatrix, g_OperationType, LOCAL, transformComponent->Transform, NULL, useSnap ? snap : NULL, NULL, NULL);
}

void
viewport_create(NativeWindow window, EditorCamera* camera, FrameBuffer* framebuffer)
{
    g_Window = window;
    g_Camera = camera;
    g_Framebuffer = framebuffer;
}

void
viewport(Scene* scene)
{
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0.0f, 0.0f));
    static bool viewportOpen = true;
    if (igBegin("Viewport", &viewportOpen, ImGuiWindowFlags_None))
    {
	igSetItemDefaultFocus();

	g_IsViewportFocused = igIsWindowFocused(ImGuiWindowFlags_None);
	g_IsViewportHovered = igIsWindowHovered(ImGuiWindowFlags_None);

	ImVec2 availableRegion;
	igGetContentRegionAvail(&availableRegion);
	if (!ImVec2_Equals(g_ViewportSize, availableRegion))
	{
	    ImVec2_Assign(g_ViewportSize, availableRegion);
	    framebuffer_invalidate(g_Framebuffer, g_ViewportSize.x, g_ViewportSize.y);
	}

	igImage((ImTextureID)g_Framebuffer->ColorAttachment, ImVec2_(g_ViewportSize.x, g_ViewportSize.y), ImVec2_(0, 1), ImVec2_(1, 0), ImVec4_(1, 1, 1, 1), ImVec4_(1, 1, 1, 0));

	Entity entity = world_outliner_get_selected_entity();
	if (entity.Name)
	    guizmo(scene, entity, g_Camera);
    }

    if (igBegin("Camera Settings", &g_IsCameraSettingsOpen, ImGuiWindowFlags_None))
    {
	igSliderFloat("ZoomLevel", &g_Camera->ZoomLevel, 0.1f, 10.0f, "%0.1f", ImGuiSliderFlags_None);
	igSliderFloat("Speed", &g_Camera->Speed, 0.1f, 10.0f, "%0.1f", ImGuiSliderFlags_None);
    }

    igEnd();

    igPopStyleVar(1);

    igEnd();
}

void
viewport_on_update(f32 timestep)
{
    NativeWindow* window = &g_Window;
    EditorCamera* camera = g_Camera;

    if (g_IsViewportFocused && g_IsViewportHovered)
    {
	if (!window_is_key_pressed(window, g_SnapKeyBinding))
	{
	    if (window_is_key_pressed(window, KEY_W))
	    {
		camera->Orthographic.Position[1] += camera->Speed * timestep;
	    }
	    if (window_is_key_pressed(window, KEY_S))
	    {
		camera->Orthographic.Position[1] -= camera->Speed * timestep;
	    }
	    if (window_is_key_pressed(window, KEY_A))
	    {
		camera->Orthographic.Position[0] -= camera->Speed * timestep;
	    }
	    if (window_is_key_pressed(window, KEY_D))
	    {
		camera->Orthographic.Position[0] += camera->Speed * timestep;
	    }
	}
    }

    editor_camera_on_update(camera);
}

void
viewport_on_event(Event* event)
{
    if (!g_IsViewportFocused && !g_IsViewportHovered)
	return;

    switch (event->Category)
    {

    case KeyCategory:
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	switch (keyEvent->KeyCode)
	{
	case KEY_SPACE:
	    g_Camera->Orthographic.Position[0] = 3.0f;
	    g_Camera->Orthographic.Position[1] = 1.5f;
	    g_Camera->Orthographic.Position[2] = 0.0f;
	    event->IsHandled = 1;
	    break;
	case KEY_F11:
	    // disble everythin but viewport
	    break;

	}

	if (keyEvent->Modificator == MOD_CONTROL)
	{
	    if (keyEvent->KeyCode == g_GizmoRotationKeyBinding)
	    {
		g_OperationType = ROTATE;
		event->IsHandled = 1;
	    }
	    else if (keyEvent->KeyCode == g_GizmoTranslationKeyBinding)
	    {
		g_OperationType = TRANSLATE;
		event->IsHandled = 1;
	    }
	    else if (keyEvent->KeyCode == g_GizmoScaleKeyBinding)
	    {
		g_OperationType = SCALE;
		event->IsHandled = 1;
	    }
	}

	break;
    }

    case MouseCategory:
    {
	if (event->Type == MouseScrolled)
	{
	    MouseScrolledEvent* mevent = (MouseScrolledEvent*)event;

	    if (g_Camera->ZoomLevel > 0.1f)
	    {
		g_Camera->ZoomLevel -= mevent->YOffset / 10;
	    }
	    else if (mevent->YOffset < 0.0f)
	    {
		g_Camera->ZoomLevel -= mevent->YOffset / 10;
	    }

	    event->IsHandled = 1;
	}
	break;
    }

    case WindowCategory:
    {
	if (event->Type == WindowResized)
	{
	    WindowResizedEvent* windowEvent = (WindowResizedEvent*)event;
	    framebuffer_invalidate(g_Framebuffer, windowEvent->Width, windowEvent->Height);
	    event->IsHandled = 1;
	}
	break;
    }

    default:
	break;
    }
}

void
viewport_menu_item()
{
    if (igBeginMenu("Viewport", 1))
    {
	igSeparator();
	static bool isOrthographic = true;
	static bool isPerspective = true;

	if (igMenuItem_Bool("Camera Settings", NULL, 0, 1))
	{
	    g_IsCameraSettingsOpen = 1;
	    igEndMenu();
	}

	igEndMenu();
    }
}
