#include "FileDialog.h"

#include <cimgui.h>
#include "Core.h"
#include "Utils/Path.h"
#include "Utils/IO.h"

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

force_inline void
entry_point_button(const char* entryPointName)
{
    if (igButton(entryPointName, ImVec2(100, 30)))
    {
	const char* absPath = path_combine_interning(homeDir, entryPointName);
	g_CurrentDirectory = absPath;
    }
}

const char*
_ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter)
{
    if (!g_CurrentDirectory)
	g_CurrentDirectory = path_get_current_directory();

    if (!homeDir)
	homeDir = path_get_home_directory();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar & ImGuiWindowFlags_HorizontalScrollbar;

    if (igBegin("Open Dialog", shouldOpen, ImGuiWindowFlags_None))
    {
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

	igEnd();
    }

    if (igBegin("Open File Dialog", shouldOpen, ImGuiWindowFlags_NoTitleBar & ImGuiWindowFlags_NoResize & ImGuiWindowFlags_MenuBar))
    {
	static bool entryPointOpen = true;
	if (igBegin("Entry Points", &entryPointOpen, flags))
	{
	    // TODO(bies): this should be a function
	    entry_point_button("Desktop");
	    entry_point_button("Documents");
	    entry_point_button("Pictures");
	    entry_point_button("Videos");
	    entry_point_button("Music");

	    igEnd();
	}

	if (igBeginTable("Table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_Sortable, (ImVec2){0,0}, 10))
	{
	    igTableSetupColumn("Name", ImGuiTableColumnFlags_None, 10, 0);
	    igTableSetupColumn("Size", ImGuiTableColumnFlags_None, 10, 0);
	    igTableHeadersRow();

	    //igTableNextColumn();
	    //igButton("Second", ImVec2(0.0f, 0.0f));
	    //igTableNextColumn();
	    //igButton("Third", ImVec2(0.0f, 0.0f));

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
		    //igTableNextRow();
		    igTableNextColumn();
		    if (igButton(dir, ImVec2(0.0f, .0f)))
		    {
			char* newDirectory = path_combine(g_CurrentDirectory, dir);
			g_CurrentDirectory = newDirectory;
		    }

		    igTableNextColumn();
		    igText("0 bytes");
		}
	    }

	    if (!(filter & FilterFlag_NoFiles))
	    {
		const char** files = directory_get_files_absolute(g_CurrentDirectory);
		for (i32 i = 0; i < array_len(files); i++)
		{
		    const char* file = files[i];
		    const char* smallFile = path_get_name(file);

		    const char* extension = path_get_extension(smallFile);
		    if (!vstring_compare(extension, ".png"))
		    {
			continue;
		    }

		    if (filter & FilterFlag_NoHiddenFiles)
		    {
			if (smallFile[0] == '.')
			{
			    continue;
			}
		    }

		    igTableNextColumn();
		    if (igButton(smallFile, ImVec2(0.0f, .0f)))
		    {
			// TODO(bies): make a function for this

			GSUCCESS("EXT: %s\n", extension);

			g_SelectedFile = path_combine_interning(g_CurrentDirectory, smallFile);
		    }


		    i32 fileSize = 0;//file_get_size(file);
		    igTableNextColumn();
		    igText("%d bytes", fileSize);
		}
	    }

	    igEndTable();
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
