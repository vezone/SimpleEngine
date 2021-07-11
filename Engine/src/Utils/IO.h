#ifndef IO_H
#define IO_H

#include "Types.h"

char* file_get_name_with_extension(const char* path);
void  file_write_string(char* filePath, char* data, i32 len);
char* file_read_string(const char* filePath);
char* file_read_bytes(const char* filePath);
i32 file_get_size(const char* filePath);

#endif
