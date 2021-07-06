#include "FileDialog.h"

#include <cimgui.h>
#include "Core.h"
#include "Utils/Path.h"

static const char* g_SelectedFile;
static const char* g_CurrentDirectory;
/*
  return i32
  NOTE(bies): если не нашли файл то 0, если файл Open || Close тогда 1
*/

force_inline const char*
s()
{
    char* homeDir = path_get_home_directory();
    const char* absPath = path_combine_interning(homeDir, "Documents");
    return absPath;
}

char* homeDir;

const char*
_ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter)
{
    if (!g_CurrentDirectory)
	g_CurrentDirectory = path_get_current_directory();

    if (!homeDir)
	homeDir = path_get_home_directory();

    if (igBegin("Open File Dialog", shouldOpen, ImGuiWindowFlags_None))
    {
	// TODO(bies): this should be a function
	if (igButton("Documents", ImVec2(0, 0)))
	{
	    const char* absPath = path_combine_interning(homeDir, "Documents");
	    g_CurrentDirectory = absPath;
	}

	igSameLine(115, 0);
	if (igButton("Desktop", ImVec2(0, 0)))
	{
	    const char* absPath = path_combine_interning(homeDir, "Desktop");
	    g_CurrentDirectory = absPath;
	}
	// TODO(bies): this should be a function

	if (igButton("^", ImVec2(0, 0)))
	{
	    char* prev = path_get_prev_directory(g_CurrentDirectory);
	    if (prev)
	    {
		g_CurrentDirectory = prev;
	    }
	}
	igSameLine(75, 0);

	igText(g_CurrentDirectory);

	const char** dirs = directory_get_directories(g_CurrentDirectory);
	for (i32 i = 0; i < array_len(dirs); i++)
	{
	    const char* dir = dirs[i];
	    if (dir[0] == '.')
	    {
		continue;
	    }

	    if (!(filter & FilterFlag_NoDirectories))
	    {
		if (igButton(dir, ImVec2(0.0f, .0f)))
		{
		    char* newDirectory = path_combine(g_CurrentDirectory, dir);
		    g_CurrentDirectory = newDirectory;
		}
	    }
	}

	if (!(filter & FilterFlag_NoFiles))
	{
	    const char** files = directory_get_files(g_CurrentDirectory);
	    for (i32 i = 0; i < array_len(files); i++)
	    {
		const char* file = files[i];
		const char* extension = path_get_extension(file);
		if (!vstring_compare(extension, ".png"))
		{
		    continue;
		}

		if (filter & FilterFlag_NoHiddenFiles)
		{
		    if (file[0] == '.')
		    {
			continue;
		    }
		}

		if (igButton(file, ImVec2(0.0f, .0f)))
		{
		    // TODO(bies): make a function for this

		    GSUCCESS("EXT: %s\n", extension);

		    g_SelectedFile = path_combine_interning(g_CurrentDirectory, file);
		}
	    }
	}

	if (igButton("Cancel", ImVec2(150.0f, 50.0f)))
	{
	    selectedFile = NULL;
	    *shouldOpen = 0;
	}

	igSameLine(160, 0);
	if (igButton("Open", ImVec2(150.0f, 50.0f)))
	{
	    selectedFile = g_SelectedFile;
	}
    }
    igEnd();

    return selectedFile;
}
