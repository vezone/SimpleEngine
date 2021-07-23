#include "SimpleTest.h"
#include "UI/ui.h"

#include "UTests/Test.h"
#include "UTests/Math/UTest_BaseMath.h"
#include "UTests/Math/UTest_V2.h"
#include "UTests/Math/UTest_M4.h"
#include "UTests/Utils/UTest_String.h"
#include "UTests/Utils/UTest_Path.h"
#include "UTests/Utils/UTest_Array.h"
#include "UTests/Utils/UTest_HashTable.h"
#include "UTests/ECS/UTest_ECS.h"

#include "ToggleButton.h"

#define ImVec2(x, y) (ImVec2) {x, y}
#define ImVec4(x, y, z, w) (ImVec4) {x, y, z, w}

static NativeWindow g_Window;
static ImFont** g_Fonts = NULL;

void simple_test_on_attach(NativeWindow window)
{
    g_Window = window;

    //memory_allocator_test();
    base_math_test();
    v2_test();
    m4_test();

    string_test();
    path_test();
    array_test();
    hash_test();
    ecs_test();
    string_builder_test();

    GSUCCESS("Ran all test's!\n");

    g_Fonts = ui_get_fonts();

    script_test();
}

void simple_test_on_update(f32 timestep)
{
}

void simple_test_on_event(Event* event)
{
    if (event->Category == KeyCategory && event->Type == KeyPressed)
    {
	KeyPressedEvent* keyEvent = (KeyPressedEvent*) event;
	if (keyEvent->KeyCode == KEY_ESCAPE)
	{
	    application_close();
	    event->IsHandled = 1;
	}
    }
    else if (event->Category == WindowCategory && event->Type == WindowShouldBeClosed)
    {
	application_close();
    }
}

static i32 selectedIndex = -1;
const char* selectedFileName = NULL;

void simple_test_on_ui()
{
    static bool opt_fullscreen = 1;
    static bool newItem = 0;
    static bool openItem = 0;
    static bool opt_padding = 0;
    static bool p_open = 1;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
	igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }

    igBegin("DockSpace Demo", &p_open, window_flags);

    if (!opt_padding)
	igPopStyleVar(1);

    if (opt_fullscreen)
	igPopStyleVar(2);

    // DockSpace
    ImGuiIO* io = igGetIO();
    if (io->ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
	ImGuiID dockspace_id = igGetID_Str("MyDockSpace");
	igDockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, &class);
    }

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

	    if (igMenuItem_Bool("Close", NULL, false, 0))
		p_open = false;

	    igEndMenu();
	}

	igEndMenuBar();
    }
    igEnd();

    // NOTE(bies): we can get exaples from demo
    //igShowDemoWindow(&isDemoOpen);

    static bool isDemoOpen = 1;
    static bool isTestPanelOpen = 1;

    const char** fileNames = test_get_filenames();
    i32 length = array_len(fileNames);

    static char* testText = "No text";
    if (igBegin("Test's", &isTestPanelOpen, ImGuiWindowFlags_None))
    {
	i32 state = 0;
	igToggleButton("E", ImVec2(25, 25), &state);
	TestInfo testInfo = test_get_info();
	igSameLine(35.0f, 20);
	igText("S: %d E: %d TestCount: %d", testInfo.SuccessCount, testInfo.ErrorsCount, testInfo.Count);
	for (i32 i = 0; i < length; i++)
	{
	    const char* filename = fileNames[i];
	    if (igCollapsingHeader_BoolPtr(filename, NULL, ImGuiWindowFlags_NoCollapse))
	    {
		FileInfo* fileInfo = file_get_info(filename);
		if (!fileInfo || !fileInfo->Functions)
		{
		    continue;
		}

		i32 functionsCount = array_len(fileInfo->Functions);
		for (i32 f = 0; f < functionsCount; f++)
		{
		    const char* functionName = fileInfo->Functions[f];
		    FunctionResult* functionResult =  file_info_get_function_result(fileInfo, functionName);
		    if (!functionName || !functionResult)
		    {
			continue;
		    }

		    if ((state == 1 && functionResult->IsSuccess) || (state == 2 && !functionResult->IsSuccess))
		    {
			continue;
		    }

		    f32 redVal;
		    f32 greenVal;
		    f32 blueVal;

		    if (functionResult->IsSuccess)
		    {
			redVal = 0.3f;
			greenVal = 0.8f;
			blueVal = 0.6f;
		    }
		    else
		    {
			redVal = 1.0f;
			greenVal = 0.5f;
			blueVal = 0.5f;
		    }

		    igPushID_Int(1);
		    if (f == selectedIndex && selectedFileName != NULL && vstring_compare(filename, selectedFileName))
		    {
			static f32 selectedRedVal   = 0.9f;
			static f32 selectedGreenVal = 0.5f;
			static f32 selectedBluesVal = 0.1f;

			igPushStyleColor_Vec4(ImGuiCol_Button, ImVec4(selectedRedVal, selectedGreenVal, selectedBluesVal, 1.0f));
			igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, ImVec4(selectedRedVal, selectedGreenVal + 0.1f, selectedBluesVal + 0.1f, 1.0f));
			igPushStyleColor_Vec4(ImGuiCol_ButtonActive, ImVec4(selectedRedVal, selectedGreenVal + 0.2f, selectedBluesVal + 0.2f, 1.0f));
		    }
		    else
		    {
			igPushStyleColor_Vec4(ImGuiCol_Button, ImVec4(redVal, greenVal, blueVal, 1.0f));
			igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, ImVec4(redVal, greenVal + 0.1f, blueVal + 0.1f, 1.0f));
			igPushStyleColor_Vec4(ImGuiCol_ButtonActive, ImVec4(redVal, greenVal + 0.2f, blueVal + 0.2f, 1.0f));
		    }

		    if (igButton(functionName, ImVec2(0, 0)))
		    {
			selectedIndex = f;
			selectedFileName = filename;

			window_set_title(&g_Window, functionName);
			testText = functionResult->Builder;
		    }

		    igPopStyleColor(3);
		    igPopID();
		}
	    }
	}


	igEnd();
    }

    if (igBegin("Test Results", NULL, ImGuiWindowFlags_None))
    {
	igPushFont(g_Fonts[1]);
	igText(testText);
	igPopFont();
	igEnd();
    }
}

void simple_test_on_destroy()
{

}
