#include "FileDialog.h"

#include <cimgui.h>
#include "Core.h"
#include "Utils/Path.h"

static i8 g_IsOpenFileDialogOpened = 1;
static const char* g_CurrentDirectory;
static const char* g_SelectedFile;

const char*
igFileDialog()
{
    const char* selectedFile = NULL;

    if (!g_CurrentDirectory)
    {
	g_CurrentDirectory = path_get_current_directory();
    }

    if (g_IsOpenFileDialogOpened)
    {
	if (igBegin("Open File Dialog", &g_IsOpenFileDialogOpened, ImGuiWindowFlags_None))
	{
	    if (igButton("Documents", ImVec2(0, 0)))
	    {
		char* homeDir = path_get_home_directory();
		const char* absPath = path_combine_interning(homeDir, "Documents");
		g_CurrentDirectory = absPath;
	    }

	    igSameLine(115, 0);
	    if (igButton("Desktop", ImVec2(0, 0)))
	    {
		char* homeDir = path_get_home_directory();
		const char* absPath = path_combine_interning(homeDir, "Desktop");
		g_CurrentDirectory = absPath;
	    }

	    if (igButton("^", ImVec2(0, 0)))
	    {
		char* prev = path_get_prev_directory(g_CurrentDirectory);
		if (prev)
		{
		    g_CurrentDirectory = prev;
		}
	    }
	    igSameLine(75, 0);

	    const char* currentDirectory = g_CurrentDirectory;
	    igText(currentDirectory);

	    const char** dirs = directory_get_directories(currentDirectory);
	    for (i32 i = 0; i < array_len(dirs); i++)
	    {
		const char* dir = dirs[i];
		if (dir[0] == '.')
		{
		    continue;
		}

		if (igButton(dir, ImVec2(0.0f, .0f)))
		{
		    char* newDirectory = path_combine(currentDirectory, dir);

		    g_CurrentDirectory = newDirectory;
		}
	    }

	    const char** files = directory_get_files(currentDirectory);
	    for (i32 i = 0; i < array_len(files); i++)
	    {
		const char* file = files[i];
		if (igButton(file, ImVec2(0.0f, .0f)))
		{
		    if (file[0] == '.')
		    {
			continue;
		    }

		    g_SelectedFile = path_combine_interning(currentDirectory, file);
		}
	    }

	    if (igButton("Close", ImVec2(150.0f, 50.0f)))
	    {
		g_IsOpenFileDialogOpened = 0;
	    }

	    igSameLine(160, 0);
	    if (igButton("Open", ImVec2(150.0f, 50.0f)))
	    {
		selectedFile = g_SelectedFile;
	    }
	}
	igEnd();
    }

    return selectedFile;
}
