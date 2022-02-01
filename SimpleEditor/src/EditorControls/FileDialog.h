#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <stdbool.h>

/*
TODO:




  REWRITE ALL
  OPTIMIZE




   file_dialog_on_attach()
   file_dialog_on_update()
   file_dialog_on_ui_render()

*/

#define igFileDialog(selectedFile, shouldOpen, filter)			\
    ({									\
	selectedFile = NULL;						\
	if (*shouldOpen == 1)						\
	{								\
	    selectedFile = _ig_file_dialog(selectedFile, shouldOpen, filter); \
	}								\
	(selectedFile != NULL) ? 1 : 0;					\
    })

typedef enum FilterFlag
{
    // Означает выводит все файлы
    FilterFlag_None = 1 << 0,
    FilterFlag_NoFiles = 1 << 1,
    FilterFlag_NoDirectories = 1 << 2,
    FilterFlag_HiddenFiles = 1 << 2,
    FilterFlag_SOURCE = 1 << 3,
    FilterFlag_IMAGE = 1 << 4,
    FilterFlag_SOUND = 1 << 5,
    FilterFlag_END = 1 << 6
} FilterFlag;

#define FILTER_FLAG_BEGIN FilterFlag_SOURCE
#define FILTER_FLAG_END FilterFlag_END

const char* _ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter);

void file_dialog_create();

#endif
