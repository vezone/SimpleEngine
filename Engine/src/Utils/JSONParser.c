#include "JSONParser.h"

#include "Utils/IO.h"

typedef enum JSONToken
{
    JSON_TOKEN_NONE = 0,
    JSON_TOKEN_OPEN_BRACKET,
    JSON_TOKEN_CLOSE_BRACKET

} JSONToken;

void
_json_read_tokens(char* stream)
{
    char c;

    while ((c = *stream) != '\0')
    {

	switch (c)
	{
	case '{':
	{

	    break;
	}
	}

	++stream;
    }
}

void
json_init(const char* path)
{
    char* data = file_read_string(path);

    char* stream = data;
    _json_read_tokens(stream);
}
