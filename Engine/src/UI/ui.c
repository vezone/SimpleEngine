#include "ui.h"
#include "Application/Application.h"
#include "Graphics/KeyCodes.h"
#include "Event/Event.h"
#include "Utils/Logger.h"
#include "Utils/Asset.h"
#include "Utils/Array.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "cimgui_impl_glfw.h"
#include "cimgui_impl_opengl3.h"

ImFont** g_Fonts = NULL;
i8 g_IsBlockingEvents = 0;

void
ui_on_attach(NativeWindow window)
{
    igCreateContext(NULL);
    ImGuiIO* io = igGetIO();
    //(void)io;
    // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Enable Docking
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    GERROR("FONT: %s\n", asset_font("NotoSans.ttf"));

    io->FontDefault = ImFontAtlas_AddFontFromFileTTF(io->Fonts, asset_font("NotoSans.ttf"), 22.0f, NULL, NULL);

    array_push(g_Fonts, io->FontDefault);
    array_push(g_Fonts, ImFontAtlas_AddFontFromFileTTF(io->Fonts, asset_font("NotoSans.ttf"), 32.0f, NULL, NULL));

    igStyleColorsDark(igGetStyle());

    ImGui_ImplGlfw_InitForOpenGL(window.GlfwWindow, 1);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void
ui_on_event(Event* event)
{
    if (g_IsBlockingEvents)
    {
	ImGuiIO* io = igGetIO();
	if (event->Category == MouseCategory)
	    event->IsHandled = io->WantCaptureMouse;
	if (event->Category == KeyCategory)
	    event->IsHandled = io->WantCaptureKeyboard;
    }
}

void
ui_on_destoy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(igGetCurrentContext());
}

void
ui_block_event(i8 block)
{
    g_IsBlockingEvents = block;
}

void
ui_begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
}

void
ui_end()
{
    ImGuiIO* io = igGetIO();
    i32 width, height;
    window_get_size(&(application_get()->Window), &width, &height);
    io->DisplaySize = (ImVec2) { width, height };

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

ImFont**
ui_get_fonts()
{
    return g_Fonts;
}
