#include "cimgui.h"
#include "cimgui_impl_glfw.h"

void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != NULL)
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;
}

void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != NULL)
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO* io = igGetIO();
    io->MouseWheelH += (float)xoffset;
    io->MouseWheel += (float)yoffset;
}

void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != NULL)
        g_PrevUserCallbackKey(window, key, scancode, action, mods);


    ImGuiIO* io = igGetIO();
    if (action == GLFW_PRESS)
        io->KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io->KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io->KeyCtrl = io->KeysDown[GLFW_KEY_LEFT_CONTROL] || io->KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io->KeyShift = io->KeysDown[GLFW_KEY_LEFT_SHIFT] || io->KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io->KeyAlt = io->KeysDown[GLFW_KEY_LEFT_ALT] || io->KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
    io->KeySuper = false;
#else
    io->KeySuper = io->KeysDown[GLFW_KEY_LEFT_SUPER] || io->KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void ImGui_ImplGlfw_CharCallback(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != NULL)
        g_PrevUserCallbackChar(window, c);

    ImGuiIO* io = igGetIO();
    //io->AddInputCharacter(c);
}

bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_OpenGL);
}

bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks)
{
    return ImGui_ImplGlfw_Init(window, install_callbacks, GlfwClientApi_Vulkan);
}

void ImGui_ImplGlfw_Shutdown()
{
    if (g_InstalledCallbacks)
    {
        glfwSetMouseButtonCallback(g_Window, g_PrevUserCallbackMousebutton);
        glfwSetScrollCallback(g_Window, g_PrevUserCallbackScroll);
        glfwSetKeyCallback(g_Window, g_PrevUserCallbackKey);
        glfwSetCharCallback(g_Window, g_PrevUserCallbackChar);
        g_InstalledCallbacks = false;
    }

    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = NULL;
    }
    g_ClientApi = GlfwClientApi_Unknown;
}

void ImGui_ImplGlfw_NewFrame()
{
    ImGuiIO* io = igGetIO();
    //IM_ASSERT(io->Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
    ImVec2 displaySize;
    displaySize.x = (float) w;
    displaySize.y = (float) h;
    io->DisplaySize = displaySize;
    if (w > 0 && h > 0)
    {
	ImVec2 displaySize;
	displaySize.x = (float)display_w / w;
	displaySize.y = (float)display_h / h;
        io->DisplayFramebufferScale = displaySize;
    }

    // Setup time step
    double current_time = glfwGetTime();
    io->DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;

    ImGui_ImplGlfw_UpdateMousePosAndButtons();
    ImGui_ImplGlfw_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    ImGui_ImplGlfw_UpdateGamepads();
}
