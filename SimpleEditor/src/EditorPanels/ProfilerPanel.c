#include "ProfilerPanel.h"

#include <cimgui.h>
#include <UI/ui.h>
#include <Graphics/Texture2D.h>

static i32 g_IsProfilerOpened = 1;
static f32 g_Timestep = 0;

void
profiler_panel_on_update(f32 timestep)
{
    g_Timestep = timestep;
}

static i32 memoryCount = 60;
static i32 memoryOffset = 59;
static f32 memoryValues[60] = {};
static f32 memoryMax = 50;

static i32 cpuCount = 60;
static i32 cpuOffset = 59;
static f32 cpuValues[60] = {};
static f32 cpuMax = 5;

void
rring_buffer_push_back(f32 array[60], f32 value, i32* offset)
{
    i32 offsetVal = *offset;

    if (offsetVal > 59)
    {
	offsetVal = 0;
    }

    array[offsetVal] = value;
    *offset = offsetVal + 1;
}

void
profiler_panel()
{
    if (!g_IsProfilerOpened)
	return;

    if (igBegin("Profiler", &g_IsProfilerOpened, ImGuiWindowFlags_None))
    {
	igSeparator();

	i32 ms = (1000 * g_Timestep);
	i32 bytes = memory_helper_get_allocated_size();
	i32 kbytes = (i32)TOKB(bytes);
	igText("[CPU Usage] %d ms", ms);
	igText("[Memory Usage] In bytes: %d kb: %d", bytes, kbytes);

	igSeparator();

	rring_buffer_push_back(cpuValues, ms, &cpuOffset);
	rring_buffer_push_back(memoryValues, kbytes, &memoryOffset);

	{
	    if (1.2 * ms >= cpuMax)
	    {
		cpuMax = 2 * ms;
	    }
	    else if ((2 * ms) < cpuMax)
	    {
		cpuMax = cpuMax / 2;
	    }

	    if (1.2 * kbytes > memoryMax)
	    {
		memoryMax = 2 * kbytes;
	    }

	    igPlotLines_FloatPtr("CPU Usage", cpuValues, cpuCount, cpuOffset, "", 0, cpuMax, ImVec2_(0, 80.0f), 4);
	    igPlotLines_FloatPtr("Memory Usage", memoryValues, memoryCount, memoryOffset, "", 0, memoryMax, ImVec2_(0, 80.0f), 4);
	}
    }
    igEnd();
}

void
profiler_panel_set_visibility(i32 visibility)
{
    g_IsProfilerOpened = visibility;
}
