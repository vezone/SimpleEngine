#include "Window.h"
#include "Utils/Logger.h"

static void (*_window_on_event_function)(Event* event);

static void
_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset)
{
    MouseScrolledEvent event = {};
    event.Base.IsHandled = 0;
    event.Base.Type = MouseScrolled;
    event.Base.Category = MouseCategory;
    event.XOffset = xoffset;
    event.YOffset = yoffset;

    _window_on_event_function((Event*)&event);
}

static void
_window_size_callback(GLFWwindow* window, i32 width, i32 height)
{
    WindowResizedEvent event = {};
    event.Base.IsHandled = 0;
    event.Base.Type = WindowResized;
    event.Base.Category = WindowCategory;
    event.Width = width;
    event.Height = height;

    _window_on_event_function((Event*)&event);
}

static void
_key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
    KeyPressedEvent event = {};
    event.Base.IsHandled = 0;
    event.Base.Category = KeyCategory;
    event.KeyCode = key;
    event.Modificator = mods;

    if (action == GLFW_PRESS)
    {
	event.Base.Type = KeyPressed;
	event.RepeatCount = 0;
    }
    else if (action == GLFW_REPEAT)
    {
	event.Base.Type = KeyRepeatPressed;
	event.RepeatCount = 1;
    }
    else if (action == GLFW_RELEASE)
    {
	event.Base.Type = KeyRealeased;
	event.RepeatCount = 0;
    }

    _window_on_event_function((Event*)&event);
}

static void
_mouse_button_callback(GLFWwindow* window, i32 button, i32 action, i32 mods)
{
    MouseButtonEvent event = {};
    event.MouseCode = button;
    event.Base.IsHandled = 0;
    event.Base.Category = MouseCategory;

    if (action == GLFW_PRESS)
    {
	event.Base.Type = MouseButtonPressed;
    }
    else if (action == GLFW_RELEASE)
    {
	event.Base.Type = MouseButtonReleased;
    }

    _window_on_event_function((Event*)&event);
}

static void
_window_minimized_callback(GLFWwindow* window, i32 isMinimized)
{
    if (isMinimized)
    {
	Event event = {
	    .IsHandled = 0,
	    .Type = WindowMinimized,
	    .Category = WindowCategory
	};
	_window_on_event_function((Event*)&event);
    }
    else
    {
	Event event = {
	    .IsHandled = 0,
	    .Type = WindowRestored,
	    .Category = WindowCategory
	};
	_window_on_event_function((Event*)&event);
    }
}

static void
_window_maximized_callback(GLFWwindow* window, i32 isMaximized)
{
    if (isMaximized)
    {
	Event event = {
	    .IsHandled = 0,
	    .Type = WindowMaximized,
	    .Category = WindowCategory
	};
	_window_on_event_function((Event*)&event);
    }
}

static void
_window_close_callback(GLFWwindow* window)
{
    Event event = {
	.IsHandled = 0,
	.Type = WindowShouldBeClosed,
	.Category = WindowCategory
    };
    _window_on_event_function((Event*)&event);
}

static void
_window_refreshed(GLFWwindow* window)
{
    Event event = {
	.IsHandled = 0,
	.Type = WindowRestored,
	.Category = WindowCategory
    };
    _window_on_event_function((Event*)&event);
}

i32
window_create(NativeWindow* window, u32 width, u32 height, const char* tittle, void (*onEvent)(Event* event))
{
    i32 major, minor, revision;

    if (!glfwInit())
    {
	GERROR("GLFW is not initialized!\n");
	return(-1);
    }

    glfwGetVersion(&major, &minor, &revision);
    GLOG(MAGNETA("GLFW version: %d.%d.%d\n"), major, minor, revision);

    window->Width = width;
    window->Height = height;
    window->Title = tittle;
    window->GlfwWindow = glfwCreateWindow(width, height, tittle, NULL, NULL);
    window->OnEvent = onEvent;
    _window_on_event_function = onEvent;

    glfwMakeContextCurrent(window->GlfwWindow);

    glfwSetKeyCallback(window->GlfwWindow, _key_callback);
    glfwSetMouseButtonCallback(window->GlfwWindow, _mouse_button_callback);
    glfwSetScrollCallback(window->GlfwWindow, _scroll_callback);
    glfwSetWindowSizeCallback(window->GlfwWindow, _window_size_callback);
    glfwSetWindowIconifyCallback(window->GlfwWindow, _window_minimized_callback);
    glfwSetWindowMaximizeCallback(window->GlfwWindow, _window_maximized_callback);
    glfwSetWindowCloseCallback(window->GlfwWindow, _window_close_callback);

    return(1);
}

static i32 g_WindowPos[2];
static i32 g_WindowSize[2];

void
window_set_fullscreen(NativeWindow* window)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videomode = glfwGetVideoMode(monitor);

    if (glfwGetWindowMonitor(window->GlfwWindow) == NULL)
    {
	glfwGetWindowPos(window->GlfwWindow, &g_WindowPos[0], &g_WindowPos[1]);
	glfwGetWindowSize(window->GlfwWindow, &g_WindowSize[0], &g_WindowSize[1]);

	glfwSetWindowMonitor(window->GlfwWindow, monitor, 0, 0, videomode->width, videomode->height, 0);
    }
    else
    {
	glfwSetWindowMonitor(window->GlfwWindow, NULL, g_WindowPos[0], g_WindowPos[1], g_WindowSize[0], g_WindowSize[1], 0);
    }
}
