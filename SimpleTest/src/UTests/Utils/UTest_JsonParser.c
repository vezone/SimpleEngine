#include "UTest_JsonParser.h"

#include "UTests/Test.h"

#include "Utils/JsonParser.h"


void
json_base_test()
{
    const char* path = "/home/bies/Data/programming/C/SimpleEngine/Engine/src/Serializer/example.json";

    JsonParser parser = json_init(path);

    i32 count = array_count(parser.Tokens);
    for (i32 i = 0; i < count; i++)
    {
	JsonToken token = parser.Tokens[i];
	const char* tokenTypeName = json_token_to_string(token);
	String_Value(tokenTypeName);
	String_Value(token.Data);
    }

}

void
json_parser_test()
{


    TEST(json_base_test());
}
