#include "SimpleTest.h"

#include "UTests/Test.h"
#include "UTests/Math/UTest_BaseMath.h"
#include "UTests/Math/UTest_V2.h"
#include "UTests/Utils/UTest_String.h"
#include "UTests/Utils/UTest_Path.h"
#include "UTests/Utils/UTest_Array.h"
#include "UTests/Utils/UTest_HashTable.h"
#include "UTests/ECS/UTest_ECS.h"

#include "ToggleButton.h"

#define ImVec2(x, y) (ImVec2) {x, y}
#define ImVec4(x, y, z, w) (ImVec4) {x, y, z, w}

static NativeWindow g_Window;

void simple_test_on_attach(NativeWindow window)
{
    g_Window = window;

    //memory_allocator_test();
#if 1
    base_math_test();
    v2_test();

    string_test();
    path_test();
    array_test();
    hash_test();
    ecs_test();
#endif

    string_builder_test();
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
	    window_set_should_close(&g_Window, 1);
	    event->IsHandled = 1;
	}
    }
}

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
	ImVec2 sizeOut;
	ImVec2 posOut;
	ImGuiViewport_GetWorkSize(&sizeOut, viewport);
	ImGuiViewport_GetWorkPos(&posOut, viewport);

	igSetNextWindowPos(posOut, ImGuiCond_FirstUseEver, ImVec2(0.0f, 0.0f));
	igSetNextWindowSize(sizeOut, ImGuiCond_FirstUseEver);
	igSetNextWindowViewport(viewport->ID);
	igPushStyleVarFloat(ImGuiStyleVar_WindowRounding, 0.0f);
	igPushStyleVarFloat(ImGuiStyleVar_WindowBorderSize, 0.0f);
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
	igPushStyleVarVec2(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
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
	ImGuiID dockspace_id = igGetIDStr("MyDockSpace");
	igDockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, &class);
    }

    if (igBeginMenuBar())
    {
	if (igBeginMenu("File", 1))
	{
	    // Disabling fullscreen would allow the window to be moved to the front of other windows,
	    // which we can't undo at the moment without finer window depth/z control.
	    if (igMenuItemBool("Open", "Ctrl + O", 0, 1))
	    {
		GWARNING("Ctrl O\n");
	    }
	    if (igMenuItemBool("New", "Ctrl+N", 0, 1))
	    {
		GWARNING("Ctrl N\n");
	    }
	    igSeparator();

	    igSeparator();

	    if (igMenuItemBool("Close", NULL, false, 0))
		p_open = false;

	    igEndMenu();
	}

	igEndMenuBar();
    }
    igEnd();

    static bool isDemoOpen = 1;
    static bool isTestPanelOpen = 1;
    //igShowDemoWindow(&isDemoOpen);

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
	    if (igCollapsingHeaderBoolPtr(fileNames[i], NULL, ImGuiWindowFlags_NoCollapse))
	    {
		FileInfo* fileInfo = file_get_info(fileNames[i]);
		if (!fileInfo || !fileInfo->Functions)
		{
		    continue;
		}

		i32 functionsCount = array_len(fileInfo->Functions);
		for (i32 r = 0; r < functionsCount; r++)
		{
		    const char* functionName = fileInfo->Functions[r];
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

		    igPushIDInt(1);
		    igPushStyleColorVec4(ImGuiCol_Button, ImVec4(redVal, greenVal, blueVal, 1.0f));
		    igPushStyleColorVec4(ImGuiCol_ButtonHovered, ImVec4(redVal, greenVal + 0.1f, blueVal + 0.1f, 1.0f));
		    igPushStyleColorVec4(ImGuiCol_ButtonActive, ImVec4(redVal, greenVal + 0.2f, blueVal + 0.2f, 1.0f));
		    if (igButton(functionName, ImVec2(0, 0)))
		    {
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
	igText(testText);
	igEnd();
    }
}

void simple_test_on_destroy()
{

}
