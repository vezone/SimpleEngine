#ifndef CIMGUI_IMPL_GLFW
#define CIMGUI_IMPL_GLFW

#include "cimgui.h"
#include "cimgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <string.h>

#ifdef _WIN32
 #undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
 #include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#endif

#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#ifdef GLFW_RESIZE_NESW_CURSOR
 #define GLFW_HAS_NEW_CURSORS          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400)
#else
 #define GLFW_HAS_NEW_CURSORS          (0)
#endif

// Data
typedef enum GlfwClientApi
{
    GlfwClientApi_Unknown,
    GlfwClientApi_OpenGL,
    GlfwClientApi_Vulkan
} GlfwClientApi;

static GLFWwindow*          g_Window = NULL;    // Main window
static GlfwClientApi        g_ClientApi = GlfwClientApi_Unknown;
static double               g_Time = 0.0;
static bool                 g_MouseJustPressed[ImGuiMouseButton_COUNT] = {};
static GLFWcursor*          g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
static bool                 g_InstalledCallbacks = true;

static GLFWmousebuttonfun   g_PrevUserCallbackMousebutton = NULL;
static GLFWscrollfun        g_PrevUserCallbackScroll = NULL;
static GLFWkeyfun           g_PrevUserCallbackKey = NULL;
static GLFWcharfun          g_PrevUserCallbackChar = NULL;

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c);

static bool ImGui_ImplGlfw_Init(GLFWwindow* window, bool install_callbacks, GlfwClientApi client_api)
{
    g_Window = window;
    g_Time = 0.0;

    // Setup back-end capabilities flags
    ImGuiIO* io = igGetIO();
    io->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io->BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io->KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io->KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io->KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io->KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io->KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io->KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io->KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io->KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io->KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io->KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io->KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io->KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io->KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io->KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io->KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io->KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io->KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io->KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io->KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io->KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io->KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io->KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io->SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io->GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io->ClipboardUserData = g_Window;
#if defined(_WIN32)
    //io->ImeWindowHandle = (void*)glfwGetWin32Window(g_Window);
#endif

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
    g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
    glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = NULL;
    g_PrevUserCallbackScroll = NULL;
    g_PrevUserCallbackKey = NULL;
    g_PrevUserCallbackChar = NULL;
    if (install_callbacks)
    {
	g_InstalledCallbacks = true;
	g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
	g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
	g_PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
	g_PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    }

    g_ClientApi = client_api;
    return true;
}

bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks);

bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks);

void ImGui_ImplGlfw_Shutdown();

static void ImGui_ImplGlfw_UpdateMousePosAndButtons()
{
    // Update buttons
    ImGuiIO* io = igGetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io->MouseDown); i++)
    {
	// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io->MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
	g_MouseJustPressed[i] = false;
    }

    // Update mouse position
    const ImVec2 mouse_pos_backup = io->MousePos;
    ImVec2 newMousePos;
    newMousePos.x = -1;
    newMousePos.y = -1;
    io->MousePos = newMousePos;
#ifdef __EMSCRIPTEN__
    const bool focused = true; // Emscripten
#else
    const bool focused = glfwGetWindowAttrib(g_Window, GLFW_FOCUSED) != 0;
#endif
    if (focused)
    {
	if (io->WantSetMousePos)
	{
	    glfwSetCursorPos(g_Window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
	}
	else
	{
	    double mouse_x, mouse_y;
	    glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
	    ImVec2 newMousePosUnfocused;
	    newMousePosUnfocused.x = (float)mouse_x;
	    newMousePosUnfocused.y = (float)mouse_y;
	    io->MousePos = newMousePosUnfocused;
	}
    }
}

static void ImGui_ImplGlfw_UpdateMouseCursor()
{
    ImGuiIO* io = igGetIO();
    if ((io->ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
	return;

    ImGuiMouseCursor imgui_cursor = igGetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io->MouseDrawCursor)
    {
	// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
	glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
	// Show OS mouse cursor
	// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
	glfwSetCursor(g_Window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
	glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

static void ImGui_ImplGlfw_UpdateGamepads()
{
    ImGuiIO* io = igGetIO();
    memset(io->NavInputs, 0, sizeof(io->NavInputs));
    if ((io->ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
	return;

    // Update gamepad inputs
    #define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io->NavInputs[NAV_NO] = 1.0f; }
    #define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io->NavInputs[NAV_NO] < v) io->NavInputs[NAV_NO] = v; }
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    MAP_BUTTON(ImGuiNavInput_Activate,   0);     // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel,     1);     // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu,       2);     // Square / X
    MAP_BUTTON(ImGuiNavInput_Input,      3);     // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);    // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight,  11);    // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp,     10);    // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown,   12);    // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,  5);     // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,  5);     // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG
    if (axes_count > 0 && buttons_count > 0)
	io->BackendFlags |= ImGuiBackendFlags_HasGamepad;
    else
	io->BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

void ImGui_ImplGlfw_NewFrame();

#endif
