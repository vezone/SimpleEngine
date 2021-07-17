#pragma once

#include <GLFW/glfw3.h>

#include "Event/Event.h"
#include <Utils/Types.h>
#include <Utils/Logger.h>

//#define window_get_frequency() glfwGetTimerFrequency()
//#define window_get_cursor_position(window, xPos, yPos) glfwGetCursorPos((window)->GlfwWindow, xPos, yPos);
//#define window_set_drop_callback(window, callback) glfwSetDropCallback((window)->GlfwWindow, callback)

typedef struct NativeWindow
{
    u32 Width;
    u32 Height;
    const char* Title;
    void (*OnEvent)(Event* event);
    GLFWwindow* GlfwWindow;
} NativeWindow;

force_inline i32
window_should_close(NativeWindow* window)
{
    return glfwWindowShouldClose(window->GlfwWindow);
}

force_inline void
window_set_should_close(NativeWindow* window, i8 shouldClose)
{
    glfwSetWindowShouldClose(window->GlfwWindow, shouldClose);
}

force_inline void
window_terminate()
{
    GSUCCESS("\n\n\n TERMINATING THE WINDOW !!! \n\n\n");
    glfwTerminate();
}

force_inline void
window_set_title(NativeWindow* window, const char* title)
{
    window->Title = title;
    glfwSetWindowTitle(window->GlfwWindow, title);
}

force_inline void
window_set_resize_callback(NativeWindow* window, void (*callback) (GLFWwindow* window, i32 width, i32 height))
{
    glfwSetWindowSizeCallback(window->GlfwWindow, callback);
}

i32
window_create(NativeWindow* window, u32 width, u32 height, const char* tittle, void (*onEvent)(Event* event));

force_inline i32
window_is_key_pressed(NativeWindow* window, i32 key)
{
    i32 state = glfwGetKey(window->GlfwWindow, key);
    return(state == GLFW_PRESS || state == GLFW_REPEAT);
}

force_inline i32
window_is_mouse_pressed(NativeWindow* window, i32 key)
{
    i32 state = glfwGetMouseButton(window->GlfwWindow, key);
    return(state == GLFW_PRESS);
}

force_inline void
window_set_vsync(i32 isVsync)
{
    glfwSwapInterval(isVsync);
}

force_inline void
window_on_update(NativeWindow* window)
{
    glfwSwapBuffers(window->GlfwWindow);
    glfwPollEvents();
}

force_inline void
window_get_cursor_position(NativeWindow* window, double* xpos, double* ypos)
{
    glfwGetCursorPos(window->GlfwWindow, xpos, ypos);
}

force_inline void
window_get_size(NativeWindow* window, i32* width, i32* height)
{
    glfwGetWindowSize(window->GlfwWindow, width, height);
}

force_inline void
window_get_frame_size(NativeWindow* window, i32* left, i32* top, i32* right, i32* bottom)
{
    glfwGetWindowFrameSize(window->GlfwWindow, left, top, right, bottom);
}

force_inline void
window_minimize(NativeWindow* window)
{
    glfwIconifyWindow(window->GlfwWindow);
}

force_inline void
window_restore(NativeWindow* window)
{
    glfwRestoreWindow(window->GlfwWindow);
}

force_inline void
window_maximize(NativeWindow* window)
{
    glfwMaximizeWindow(window->GlfwWindow);
}

void window_set_fullscreen(NativeWindow* window);
