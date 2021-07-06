#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <stdbool.h>
#include "Utils/Types.h"

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
    FilterFlag_None = 1 << 0,
    FilterFlag_NoFiles = 1 << 1,
    FilterFlag_NoDirectories = 1 << 2,
    FilterFlag_NoHiddenFiles = 1 << 2,
    FilterFlag_PNG = 1 << 3
} FilterFlag;

const char* _ig_file_dialog(const char* selectedFile, bool* shouldOpen, FilterFlag filter);

#endif
