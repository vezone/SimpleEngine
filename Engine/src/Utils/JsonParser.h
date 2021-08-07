#ifndef JSON_PARSER_H
#define JSON_PARSER_H

typedef enum JsonTokenType
{
    JSON_TOKEN_TYPE_NONE = 0,
    JSON_TOKEN_TYPE_ROOT,

    JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR,
    JSON_TOKEN_TYPE_VALUE_BOOL_TRUE,
    JSON_TOKEN_TYPE_VALUE_BOOL_FALSE,
    JSON_TOKEN_TYPE_VALUE_NULL,

    JSON_TOKEN_TYPE_OBJECT,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_VALUE_ARRAY,

    JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET,
    JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET,

    JSON_TOKEN_TYPE_COUNT
} JsonTokenType;

typedef struct JsonToken
{
    JsonTokenType Type;
    void* Data;
} JsonToken;

typedef struct JsonParser
{
    JsonToken* Tokens;
} JsonParser;

JsonParser json_init(const char* path);
void json_read_file();

const char* json_token_to_string(JsonToken token);

#endif
