#include "PhysicsPanel.h"

void
physics2d_panel_create()
{

}

void
physics2d_panel(i32* isPhysicsEnabled)
{
    static bool isPhysicsPanelVisible = true;
    if (igBegin("Physics Panel", &isPhysicsPanelVisible, ImGuiWindowFlags_None))
    {
	igText("This is info %d", 1000);

	if (igButton("Enable physics simulation", ImVec2_(0, 0)))
	{
	    TYPE_REVERSE(*isPhysicsEnabled);
	}
    }
    igEnd();
}

void
physics2d_panel_on_update()
{

}
