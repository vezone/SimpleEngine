#ifndef IO_H
#define IO_H

#include "Types.h"

char* file_get_name_with_extension(const char* path);
void  file_write_string(char* file_path, char* data, i32 len);
char* file_read_string(const char* file_path);
char* file_read_bytes(const char* file_path);

#endif
