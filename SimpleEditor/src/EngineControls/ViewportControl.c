#include "ViewportControl.h"
#include <UI/ui.h>

ViewportSettings g_Settings;
EditorMatrixType g_MatrixType;

ImVec2 g_ViewportSize;
bool g_ViewportOpen;
bool g_IsViewportFocused = 0;

static char g_WindowTitle[32];
static EditorCamera* g_Camera;
static FrameBuffer* g_Framebuffer;

static bool g_IsCameraSettingsOpen = 0;

void
viewport(EditorCamera* camera, FrameBuffer* framebuffer)
{
    g_Camera = camera;
    g_Framebuffer = framebuffer;

    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2_(0.0f, 0.0f));
    if (igBegin("Viewport", &g_ViewportOpen, ImGuiWindowFlags_None))
    {
	igSetItemDefaultFocus();

	bool isWindowFocused = igIsWindowFocused(ImGuiWindowFlags_None);
	g_IsViewportFocused = isWindowFocused;

	ui_block_event(!isWindowFocused);

	ImVec2 availableRegion;
	igGetContentRegionAvail(&availableRegion);
	if (!ImVec2_Equals(g_ViewportSize, availableRegion))
	{
	    ImVec2_Assign(g_ViewportSize, availableRegion);
	    framebuffer_invalidate(framebuffer, g_ViewportSize.x, g_ViewportSize.y);
	}

	igImage((ImTextureID)framebuffer->ColorAttachment, ImVec2_(g_ViewportSize.x, g_ViewportSize.y), ImVec2_(0, 1), ImVec2_(1, 0), ImVec4_(1, 1, 1, 1), ImVec4_(1, 1, 1, 0));

	igPopStyleVar(1);

	if (igBegin("Camera Settings", &g_IsCameraSettingsOpen, ImGuiWindowFlags_None))
	{
	    igSliderFloat("ZoomLevel", &g_Camera->ZoomLevel, 0.1f, 10.0f, "%0.1f", ImGuiSliderFlags_None);
	    igSliderFloat("Speed", &g_Camera->Speed, 0.1f, 10.0f, "%0.1f", ImGuiSliderFlags_None);
	    igEnd();
	}

	igEnd();
    }
}

void
viewport_on_update(EditorCamera* camera, NativeWindow* window, f32 timestep)
{
    if (g_IsViewportFocused)
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

    editor_camera_on_update(camera);
}

void
viewport_on_event(Event* event)
{
    if (!g_IsViewportFocused)
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

	if (igBeginMenu("Matrix", 1))
	{
	    if (igButton("Perspective", ImVec2_(0, 0)))
	    {
		g_Settings.MatrixType = EditorMatrixType_Perspective;
	    }
	    if (igButton("Orthographic", ImVec2_(0, 0)))
	    {
		g_Settings.MatrixType = EditorMatrixType_Orthographic;
	    }

	    igEndMenu();
	}

	igEndMenu();
    }
}

ViewportSettings
viewport_get_settings()
{
    return g_Settings;
}
