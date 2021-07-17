#include "ViewportControl.h"
#include "Core.h"
#include <cimgui.h>

ViewportSettings g_Settings;
EditorMatrixType g_MatrixType;

void viewport_menu_item()
{
    if (igBeginMenu("Viewport", 1))
    {
	igSeparator();
	static bool isOrthographic = true;
	static bool isPerspective = true;

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
