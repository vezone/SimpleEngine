#include "FileDialog.h"

#include <cimgui.h>
#include <UI/ui.h>
#include <Graphics/Texture2D.h>
#include <Utils/Path.h>
#include <Utils/IO.h>
#include <Utils/HashTable.h>

const char* g_SelectedFile;
const char* g_CurrentDirectory;
/*
  return i32
  NOTE(bies): если не нашли файл то 0, если файл Open || Close тогда 1


  TODO(bies): настилить иконки с /usr/share/icons

*/


static char* g_HomeDir;
static Texture2D* g_DirectoryIcon;
static Texture2D* g_FileIcon;
static i32 g_DistanceBetweenIconAndText = 45;
static i32 g_IsInitialized = 0;

#define igIconButton(texture, size)					\
    ({									\
	ImVec2 uv0 = ImVec2_(0, 1);					\
	ImVec2 uv1 = ImVec2_(1, 0);					\
	ImVec4 bgCol = ImVec4_(0, 0, 0, 1);				\
	ImVec4 tintCol = ImVec4_(1, 1, 1, 1);				\
	igImage(texture->ID, size, uv0, uv1, tintCol, bgCol); \
	igIsItemHovered(ImGuiHoveredFlags_None) && igIsMouseDoubleClicked(ImGuiMouseButton_Left); \
    })

force_inline void
icons_initialize()
{
    g_DirectoryIcon = texture2d_create("resources/FileManager/dir-blue.png");
    g_FileIcon = texture2d_create("resources/FileManager/file.png");
}

void
file_dialog_create()
{
    g_IsInitialized = 1;
    g_CurrentDirectory = path_get_current_directory();
    g_HomeDir = path_get_home_directory();
    icons_initialize();
}


force_inline void
entry_point_button(const char* entryPointName)
{
    if (igButton(entryPointName, ImVec2_(100, 30)))
    {
	const char* absPath = path_combine_interning(g_HomeDir, entryPointName);
	g_CurrentDirectory = absPath;
    }
}


const char** g_Extensions;

force_inline i32
filter_files(const char* extension, FilterFlag flag)
{
    i32 i;
    i32 count = array_count(g_Extensions);
    for (i = 0; i < count; i++)
    {
	//if (!vstring_compare(extension, g_Extensions))
	{
	    return 0;
	}
    }

    return 1;
}

struct { const char* Key; Texture2D* Value; }* g_SavedTextures = NULL;

force_inline Texture2D*
file_create_or_get_texture(const char* path)
{
    i32 isTextureAlreadyAdded = shash_geti(g_SavedTextures, path);
    if (isTextureAlreadyAdded != -1)
    {
	Texture2D* texture = shash_get(g_SavedTextures, path);
	return texture;
    }

    Texture2D* texture = texture2d_create(path);
    shash_put(g_SavedTextures, path, texture);
    return texture;
}

const char*
_ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter)
{
    vassert(g_IsInitialized == 1 && "Call file_dialog_create() before using igFileDialog()");

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar & ImGuiWindowFlags_HorizontalScrollbar;

    if (igBegin("Open Dialog", shouldOpen, ImGuiWindowFlags_None))
    {
	if (igButton("^", ImVec2_(0, 0)))
	{
	    char* prev = path_get_prev_directory(g_CurrentDirectory);
	    if (prev)
	    {
		g_CurrentDirectory = prev;
	    }
	}
	igSameLine(75, 0);
	igText(g_CurrentDirectory);
    }
    igEnd();

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

	}
	igEnd();

	if (igBeginTable("Table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders, ImVec2_(0,0), 10))
	{
	    igTableSetupColumn("Name", ImGuiTableColumnFlags_None, 0, 0);
	    igTableSetupColumn("Size", ImGuiTableColumnFlags_None, 0, 0);
	    igTableHeadersRow();

	    if (!(filter & FilterFlag_NoDirectories))
	    {
		i32 i;
		const char** dirs = directory_get_directories(g_CurrentDirectory);
		i32 dirsCount = array_count(dirs);
		for (i = 0; i < dirsCount; i++)
		{
		    const char* dir = dirs[i];
		    if (dir[0] == '.')
		    {
			continue;
		    }

		    igTableNextColumn();
		    if (igIconButton(g_DirectoryIcon, ImVec2_(32, 32)))
		    {
			g_CurrentDirectory = path_combine(g_CurrentDirectory, dir);
			GERROR("CurDir: %s\n", g_CurrentDirectory);
		    }

		    igSameLine(g_DistanceBetweenIconAndText, 0);
		    igText(dir);

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

		    if (!(filter & FilterFlag_HiddenFiles) && smallFile[0] == '.')
		    {
			continue;
		    }

		    igTableNextColumn();

		    if (vstring_compare(extension, ".png"))
		    {
			Texture2D* fileTexture = file_create_or_get_texture(file);
			if (igIconButton(fileTexture, ImVec2_(32, 32)))
			{
			    g_SelectedFile = path_combine_interning(g_CurrentDirectory, smallFile);
			}
		    }
		    else
		    {
			if (igIconButton(g_FileIcon, ImVec2_(32, 32)))
			{
			}
		    }

		    igSameLine(g_DistanceBetweenIconAndText, 0);
		    igText(smallFile);


		    i32 fileSize = 0;//file_get_size(file);
		    igTableNextColumn();
		    igText("%d bytes", fileSize);
		}
	    }

	}
	igEndTable();

	if (igButton("Cancel", ImVec2_(150.0f, 50.0f)))
	{
	    selectedFile = NULL;
	    *shouldOpen = 0;
	}

	igSameLine(160, 0);
	if (igButton("Open", ImVec2_(150.0f, 50.0f)))
	{
	    selectedFile = g_SelectedFile;
	}
    }

    igEnd();

    return selectedFile;
}
