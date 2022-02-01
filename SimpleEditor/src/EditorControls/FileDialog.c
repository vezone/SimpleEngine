#include "FileDialog.h"

#include <cimgui.h>
#include <UI/ui.h>
#include <Graphics/Texture2D.h>
#include <Utils/SimpleStandardLibrary.h>

const char* g_SelectedFile;
const char* FileDialogCurrentDirectory;
/*
  return i32
  NOTE(bies): если не нашли файл то 0, если файл Open || Close тогда 1


  TODO(bies): настилить иконки с /usr/share/icons

*/


static Texture2D* g_DirectoryIcon;
static Texture2D* g_FileIcon;
static Texture2D* g_SelectedFileIcon;
static i32 g_DistanceBetweenIconAndText = 45;
static i32 g_IsInitialized = 0;

static i32 g_IsDirectoryChanged = 1;
static char** g_Dirs     = NULL;
static char** g_Files    = NULL;
static char* g_HomeDir   = NULL;
static char* g_Desktop   = NULL;
static char* g_Documents = NULL;
static char* g_Pictures  = NULL;
static char* g_Videos    = NULL;
static char* g_Music     = NULL;

force_inline void
icons_initialize()
{
    g_DirectoryIcon = texture2d_create("resources/FileManager/Directory.png");
    g_FileIcon = texture2d_create("resources/FileManager/File.png");
    g_SelectedFileIcon = texture2d_create("resources/FileManager/SelectedFile.png");
}

void
file_dialog_create()
{
    g_IsInitialized = 1;
    FileDialogCurrentDirectory = path_get_current_directory();
    g_HomeDir = path_get_home_directory();
    icons_initialize();

    g_Desktop = ielement(g_HomeDir, "");
}

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
entry_point_button(const char* entryPointName)
{
    char* name = ielement_name(entryPointName);
    if (igButton(name, ImVec2_(100, 30)))
    {
	FileDialogCurrentDirectory = entryPointName;
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
	//if (!string_compare(extension, g_Extensions))
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

force_inline void
file_dialog_free_all()
{
    if (g_Dirs)
    {
	array_free(g_Dirs);
    }
    if (g_Files)
    {
	array_free(g_Files);
    }

    g_Dirs = NULL;
    g_Files = NULL;
}

static i32 g_SelectedIndex = 0;

force_inline void
_file_table(FilterFlag filter)
{
    if (igBeginTable("Table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders, ImVec2_(0,0), 10))
    {
	igTableSetupColumn("Name", ImGuiTableColumnFlags_None, 0, 0);
	igTableSetupColumn("Size", ImGuiTableColumnFlags_None, 0, 0);
	igTableHeadersRow();

	if (g_IsDirectoryChanged)
	{
	    file_dialog_free_all();

	    g_IsDirectoryChanged = 0;
	}

	if (g_Dirs == NULL)
	{
	    g_Dirs = directory_get_directories(FileDialogCurrentDirectory);
	}

	if (g_Files == NULL)
	{
	    g_Files = directory_get_files(FileDialogCurrentDirectory);
	}

	if (!(filter & FilterFlag_NoDirectories))
	{
	    i32 i, dirsCount = array_count(g_Dirs);
	    for (i = 0; i < dirsCount; i++)
	    {
		const char* dir = g_Dirs[i];
		char* name = ielement_name(dir);
		if (name[0] == '.')
		{
		    continue;
		}

		igTableNextColumn();
		if (igIconButton(g_DirectoryIcon, ImVec2_(32, 32)))
		{
		    FileDialogCurrentDirectory = dir;
		    g_IsDirectoryChanged = 1;
		    g_SelectedIndex = - 1;
		}

		igSameLine(g_DistanceBetweenIconAndText, 0);
		igText(name);

		igTableNextColumn();
		igText("0 bytes");
	    }
	}

	if (!(filter & FilterFlag_NoFiles))
	{
	    i32 i, filesCount = array_count(g_Files);
	    for (i = 0; i < filesCount; i++)
	    {
		const char* file = g_Files[i];
		char* name = ielement_name(file);
		char* ext = ielement_extension(file);

		if (!(filter & FilterFlag_HiddenFiles) && name[0] == '.')
		{
		    continue;
		}

		igTableNextColumn();

		if (string_compare(ext, ".png"))
		{
		    Texture2D* fileTexture = file_create_or_get_texture(file);
		    if (igIconButton(fileTexture, ImVec2_(32, 32)))
		    {
			g_SelectedIndex = i;
		    }
		}
		else
		{
		    if (igIconButton(g_FileIcon, ImVec2_(32, 32)))
		    {
			g_SelectedIndex = i;

			GERROR("SelectedFile: %s\n", g_SelectedFile);
		    }
		}

		igSameLine(g_DistanceBetweenIconAndText, 0);

		if (i == g_SelectedIndex)
		{
		    igPushStyleColor_Vec4(ImGuiCol_Text, ImVec4_(1.0f, 0.8f, 0.6f, 1.0f));
		}

		igText(name);

		i32 fileSize = 0;//file_get_size(file);
		igTableNextColumn();
		igText("%d bytes", fileSize);

		if (i == g_SelectedIndex)
		{
		    igPopStyleColor(1);
		}

	    }
	}
    }

    igEndTable();
}

force_inline void
file_dialog_close(bool* shouldOpen)
{
    file_dialog_free_all();
    ielement_free_all();
    *shouldOpen = false;
}

const char*
_ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter)
{
    vassert((g_IsInitialized == 1) && "Call file_dialog_create() before using igFileDialog()");

    ImGuiWindowFlags_ flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking;

    if (igBegin("Open File Dialog", shouldOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking))
    {
	f32 contentWidth = igGetWindowContentRegionWidth();

	igSetCursorPosX(15.0f);
	if (igButton("^", ImVec2_(0, 0)))
	{
	    char* prev = path_get_prev_directory(FileDialogCurrentDirectory);
	    char* iprev = ielement(prev, "");
	    memory_free(prev);
	    if (iprev)
	    {
		FileDialogCurrentDirectory = iprev;
		g_IsDirectoryChanged = 1;
	    }
	}
	igSameLine(0.15f * contentWidth, 0);
	igText(FileDialogCurrentDirectory);
	igSeparator();

	if (igBeginChild_Str("EntryPoint", ImVec2_(0.2f * contentWidth, 100), 0, flags))
	{
	    // TODO(bies): this should be a function
	    entry_point_button(g_Desktop);
	}
	igEndChild();

	igSameLine(110, 0);

	if (igBeginChild_Str("ChildR", ImVec2_(0.85f * contentWidth, 500), 0, flags | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
	    _file_table(filter);
	}
	igEndChild();

	igSeparator();

	f32 cursorPosX = igGetCursorPosX();
	f32 actionBtnWidth = 150.0f;
	f32 actionBtnHeight = 35.0f;
	f32 cancelPosX = cursorPosX + contentWidth * 0.6f;
	igSetCursorPosX(cancelPosX);
	if (igButton("Cancel", ImVec2_(actionBtnWidth, actionBtnHeight)))
	{
	    g_SelectedIndex = -1;
	    file_dialog_close(shouldOpen);
	}

	igSameLine(cancelPosX + actionBtnWidth + 15.0f, 0);
	if (igButton("Open", ImVec2_(actionBtnWidth, actionBtnHeight)))
	{
	    selectedFile = g_Files[g_SelectedIndex];
	    file_dialog_close(shouldOpen);
	}
    }

    igEnd();

    return selectedFile;
}
