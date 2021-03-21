#include "IO.h"

#include <stdlib.h>
#include <stdio.h>

#include "String.h"

char*
file_get_name_with_extension(const char* path)
{
    i32 i, last_index = 0, name_index, new_length;
    i32 path_length = vstring_length(path);
    char* file_name;

    for (i = 0; i < path_length; i++)
    {
        if (path[i] == '/')
        {
            last_index = i;
        }
    }

    if (last_index != 0)
    {
        name_index = (last_index + 1);
    }
    else
    {
        name_index = 0;
    }

    new_length = path_length - name_index;
    file_name = malloc((new_length + 1) * sizeof(char));
    for (i = name_index; i < path_length; i++)
    {
        file_name[i - name_index] = path[i];
    }
    file_name[new_length] = '\0';
    return file_name;
}

void
file_write_string(char* file_path, char* data, i32 len)
{
    FILE* file;
    file = fopen(file_path, "w");
    fwrite(data, 1, len, file);
    fclose(file);
}

char*
file_read_string(const char* file_path)
{
    FILE* file;
    char* result;
    i32 fileLength;

    file = fopen(file_path, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        fileLength = (ftell(file));
        fseek(file, 0, SEEK_SET);
        result = malloc((fileLength + 1) * sizeof(char));

        fread(result, sizeof(char), (fileLength), file);
        result[fileLength] = '\0';

        fclose(file);
        return((char*)result);
    }

    return NULL;
}

char*
file_read_bytes(const char* file_path)
{
    FILE* file;
    char* result;
    i32 fileLength;

    file = fopen(file_path, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        fileLength = (i32)ftell(file);
        fseek(file, 0, SEEK_SET);
        result = malloc(fileLength * sizeof(char));

        fread(result, sizeof(char), fileLength, file);

        fclose(file);
        return((char*)result);
    }

    return NULL;
}
