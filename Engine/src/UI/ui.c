#include "ui.h"

#include <Application/Application.h>
#include <Graphics/KeyCodes.h>
#include <Event/Event.h>
#include <Utils/Asset.h>
#include <Utils/SimpleStandardLibrary.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "cimgui_impl_glfw.h"
#include "cimgui_impl_opengl3.h"

ImFont** g_Fonts = NULL;
i8 g_IsBlockingEvents = 0;

force_inline void
ui_hazel_theme(ImVec4 colors[ImGuiCol_COUNT])
{
    colors[ImGuiCol_WindowBg] = ImVec4_(.1, .105, .11, 1.0);

    colors[ImGuiCol_Header] = ImVec4_(.1, .105, .11, 1.0);
    colors[ImGuiCol_HeaderHovered] = ImVec4_(.3, .305, .31, 1.0);
    colors[ImGuiCol_HeaderActive] = ImVec4_(.15, .1505, .151, 1.0);

    colors[ImGuiCol_Button] = ImVec4_(.2, .205, .21, 1.0);
    colors[ImGuiCol_ButtonHovered] = ImVec4_(.3, .305, .31, 1.0);
    colors[ImGuiCol_ButtonActive] = ImVec4_(.15, .1505, .151, 1.0);

    colors[ImGuiCol_FrameBg] = ImVec4_(.2, .205, .21, 1.0);
    colors[ImGuiCol_FrameBgHovered] = ImVec4_(.3, .305, .31, 1.0);
    colors[ImGuiCol_FrameBgActive] = ImVec4_(.15, .1505, .151, 1.0);

    colors[ImGuiCol_Tab] = ImVec4_(.15, .1505, .151, 1.0);
    colors[ImGuiCol_TabHovered] = ImVec4_(.38, .3005, .31, 1.0);
    colors[ImGuiCol_TabActive] = ImVec4_(.28, .2805, .281, 1.0);
    colors[ImGuiCol_TabUnfocused] = ImVec4_(.15, .1505, .151, 1.0);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4_(.2, .205, .21, 1.0);

    colors[ImGuiCol_TitleBg] = ImVec4_(.15, .1505, .151, 1.0);
    colors[ImGuiCol_TitleBgActive] = ImVec4_(.15, .1505, .151, 1.0);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4_(.95, .1505, .951, 1.0);
}

force_inline void
ui_set_colors(ImVec4 colors[ImGuiCol_COUNT])
{
    ImVec4 mainColor = ImVec4_(1.0f/255, 81.0f/255, 152.0f/255, 1.0);
    ImVec4 activeColor = ImVec4_(0, 126.0f/255, 236.0f/255, 1.0);
    ImVec4 greyActive = ImVec4_(46.0f/255, 46.0f/255, 48.0f/255, 1.0);
    ImVec4 black = ImVec4_(19.0f/255, 19.0f/255, 19.0f/255, 1.0);
    ImVec4 bg = ImVec4_(31.0f/255, 31.0f/255, 32.0f/255, 1.0);
    ImVec4 bgBlack = ImVec4_(19.0f/255, 20.0f/255, 22.0f/255, 1.0);

    colors[ImGuiCol_WindowBg] = bg;

    colors[ImGuiCol_Header] = mainColor;
    colors[ImGuiCol_HeaderHovered] = activeColor;
    colors[ImGuiCol_HeaderActive] = activeColor;

    colors[ImGuiCol_Button] = mainColor;
    colors[ImGuiCol_ButtonHovered] = activeColor;
    colors[ImGuiCol_ButtonActive] = activeColor;

    colors[ImGuiCol_FrameBg] = bgBlack;
    colors[ImGuiCol_FrameBgHovered] = greyActive;
    colors[ImGuiCol_FrameBgActive] = greyActive;

    colors[ImGuiCol_Tab] = mainColor;
    colors[ImGuiCol_TabHovered] = activeColor;
    colors[ImGuiCol_TabActive] = activeColor;
    colors[ImGuiCol_TabUnfocused] = mainColor;
    colors[ImGuiCol_TabUnfocusedActive] = mainColor;

    colors[ImGuiCol_TitleBg] = black;
    colors[ImGuiCol_TitleBgActive] = greyActive;
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4_(0, 0, 0, 1.0);

    colors[ImGuiCol_MenuBarBg] = greyActive;
}

force_inline ImFont*
_ui_register_font(ImGuiIO* io, const char* path, f32 fontSize)
{
}


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

    GSUCCESS("FONT: %s\n", asset_font("NotoSans.ttf"));
    GSUCCESS("FONT: %s\n", asset_font("Courier Prime/Courier Prime.ttf"));

    io->FontDefault = ImFontAtlas_AddFontFromFileTTF(io->Fonts, asset_font("NotoSans.ttf"), 22.0f, NULL, NULL);

    array_push(g_Fonts, io->FontDefault);
    array_push(g_Fonts, ImFontAtlas_AddFontFromFileTTF(io->Fonts, asset_font("NotoSans.ttf"), 32.0f, NULL, NULL));
    array_push(g_Fonts, ImFontAtlas_AddFontFromFileTTF(io->Fonts, asset_font("Courier Prime/Courier Prime.ttf"), 22.0f, NULL, NULL));

    ImGuiStyle* style = igGetStyle();
    style->WindowMinSize.x = 200.0f;

    ui_set_colors(style->Colors);

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
    ImGuizmo_BeginFrame();
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
