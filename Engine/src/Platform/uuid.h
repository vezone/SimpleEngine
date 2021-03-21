#ifndef UUID
#define UUID

#ifdef LINUX_PLATFORM

#include "uuid/uuid.h"
void create_guid(char guid[])
{

    uuid_t out;
    uuid_t_generate(out);
    uuid_unparse_lower(out, guid);
}

#else

#include <stdio.h>
#include <string.h>
#include "<objbase.h>"
void create_guid(char guid[])
{
    GUID out = {};
    char* ptr;
    CoCreateGuid(&out);
    sprintf(guid, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", out.Data1, out.Data2, out.Data3, out.Data4[0], out.Data4[1], out.Data4[2], out.Data4[3], out.Data4[4], out.Data4[5], out.Data4[6], out.Data4[7]);
    for (ptr = (char*)guid; *ptr; ++ptr)
    {
	*ptr = tolower(*ptr);
    }
}

#endif
