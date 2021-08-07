#include "JsonParser.h"

#include "Utils/Types.h"
#include "Utils/Array.h"
#include "Utils/IO.h"
#include "Utils/String.h"

typedef enum JsonLetterType
{
    JSON_LETTER_TYPE_NONE = 0,
    JSON_LETTER_TYPE_OPEN_BRACKET,
    JSON_LETTER_TYPE_CLOSE_BRACKET,
    JSON_LETTER_TYPE_OPEN_STRING
} JsonLetterType;


JsonParser g_JsonParser;
const char* g_TrueKeyWord;
const char* g_FalseKeyWord;
const char* g_NullKeyWord;

force_inline void
register_keywords()
{
    g_TrueKeyWord = istring("true");
    g_FalseKeyWord = istring("false");
    g_NullKeyWord = istring("null");
}

force_inline void
_skip_spaces(char* stream)
{
    while (*stream == ' ' && *stream == '\n' && *stream == '\t' && *stream == '\r')
    {
	++stream;
    }
}

JsonToken
_json_read_string_token(char* stream)
{
    const i32 bufferSize = 256;
    char tempStringBuffer[bufferSize];
    i32 index = 0;

    vmemset(tempStringBuffer, '\0', bufferSize*sizeof(char));

    /*

      we are here
      |
      v
      "string content"

     */
    ++stream;

    while (*stream != '"' && *stream != '\0')
    {
	tempStringBuffer[index] = *stream;
	++index;
	++stream;
    }

    /*

		     we are here
		     |
		     v
      "string content"

     */
    ++stream;

    JsonToken stringToken;
    stringToken.Type = JSON_TOKEN_TYPE_STRING;
    stringToken.Data = vstring(tempStringBuffer);

    return stringToken;
}


force_inline i32
check_word(char* stream, const char* keyWord)
{
    i32 i;
    char* ptr = stream;
    i32 keyWordLength = istring_length(keyWord);

    for (i = 0; i < keyWordLength; i++)
    {
	if (*ptr != keyWord[i])
	{
	    return 0;
	}

	++ptr;
    }

    return 1;
}

i32
_json_next_bool(char* stream)
{
    char* ptr = stream;
    ++ptr;

    if (*ptr != 'r') return 0;
    if (*ptr != 'u') return 0;
    if (*ptr != 'e') return 0;

    return 1;
}

void
_json_read_tokens(char* stream)
{
    char c;

    JsonLetterType state = JSON_LETTER_TYPE_NONE;

    g_JsonParser.Tokens = NULL;


    while ((c = *stream) != '\0')
    {
	_skip_spaces(stream);

	switch (c)
	{
	case '{':
	{
	    if (state == JSON_LETTER_TYPE_NONE)
	    {
		JsonToken rootToken;
		rootToken.Type = JSON_TOKEN_TYPE_ROOT;
		rootToken.Data = NULL;
		array_push(g_JsonParser.Tokens, rootToken);
	    }
	    state = JSON_LETTER_TYPE_OPEN_BRACKET;
	    break;
	}

	case '}':
	{
	    state = JSON_LETTER_TYPE_CLOSE_BRACKET;
	    break;
	}
	case '"':
	{
	    if (state != JSON_LETTER_TYPE_OPEN_STRING)
	    {
		JsonToken stringToken = _json_read_string_token(stream);
		array_push(g_JsonParser.Tokens, stringToken);
	    }

	    break;
	}

	case ':':
	{
	    JsonToken keyValueToken;
	    keyValueToken.Type = JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR;
	    keyValueToken.Data = NULL;
	    array_push(g_JsonParser.Tokens, keyValueToken);
	    break;
	}

	case 't':
	{
	    if (check_word(stream, g_TrueKeyWord))
	    {
		JsonToken trueValueToken;
		trueValueToken.Type = JSON_TOKEN_TYPE_VALUE_BOOL_TRUE;
		trueValueToken.Data = NULL;
		array_push(g_JsonParser.Tokens, trueValueToken);
	    }
	    break;
	}
	case 'f':
	{
	    if (check_word(stream, g_FalseKeyWord))
	    {
		JsonToken falseValueToken;
		falseValueToken.Type = JSON_TOKEN_TYPE_VALUE_BOOL_FALSE;
		falseValueToken.Data = NULL;

		array_push(g_JsonParser.Tokens, falseValueToken);
	    }
	    break;
	}
	case 'n':
	{
	    if (check_word(stream, g_NullKeyWord))
	    {
		JsonToken nullValueToken;
		nullValueToken.Type = JSON_TOKEN_TYPE_VALUE_NULL;
		nullValueToken.Data = NULL;
		array_push(g_JsonParser.Tokens, nullValueToken);
	    }
	    break;
	}

	case '[':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET;
	    token.Data = NULL;
	    array_push(g_JsonParser.Tokens, token);
	    break;
	}

	case ']':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET;
	    token.Data = NULL;
	    array_push(g_JsonParser.Tokens, token);
	    break;
	}

	}


	++stream;
    }
}

JsonParser
json_init(const char* path)
{
    register_keywords();

    char* data = file_read_string(path);

    char* stream = data;
    _json_read_tokens(stream);

    return g_JsonParser;
}

const char* g_TokensTypeToString[] = {
    "JSON_TOKEN_TYPE_NONE",
    "JSON_TOKEN_TYPE_ROOT",
    "JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR",
    "JSON_TOKEN_TYPE_VALUE_BOOL_TRUE",
    "JSON_TOKEN_TYPE_VALUE_BOOL_FALSE",
    "JSON_TOKEN_TYPE_VALUE_NULL",
    "JSON_TOKEN_TYPE_OBJECT",
    "JSON_TOKEN_TYPE_STRING",
    "JSON_TOKEN_TYPE_VALUE_ARRAY",
    "JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET",
    "JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET",
    "JSON_TOKEN_TYPE_COUNT"
};

const char*
json_token_to_string(JsonToken token)
{
    return g_TokensTypeToString[token.Type];
}
