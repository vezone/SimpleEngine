#include "UTest_JsonParser.h"

#include "UTests/Test.h"

#include <Utils/SimpleStandardLibrary.h>

void
json_object_test(JsonObject* root)
{
    char** keys = (char**)root->Keys;
    JsonValue* values = (JsonValue*)root->Values;

    for (i32 i = 0; i < array_count(keys); ++i)
    {
	char* key = keys[i];
	JsonValue value = values[i];

	String_Value(key);

	switch (value.Type)
	{
	case JSON_VALUE_TYPE_INT:
	{
	    I32_Value(void_to_i32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_FLOAT:
	{
	    F32_Value(void_to_f32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_NULL:
	case JSON_VALUE_TYPE_STRING:
	case JSON_VALUE_TYPE_BOOL:
	{
	    String_Value((char*)value.Data);
	    break;
	}
	case JSON_VALUE_TYPE_I32_ARRAY:
	{
	    i32* array = (i32*)value.Data;
	    for (i32 i = 0; i < array_count(array); ++i)
	    {
		I32_Value(array[i]);
	    }
	    break;
	}
	case JSON_VALUE_TYPE_F32_ARRAY:
	{
	    f32* array = (f32*)value.Data;
	    for (i32 i = 0; i < array_count(array); ++i)
	    {
		F32_Value(array[i]);
	    }
	    break;
	}
	case JSON_VALUE_TYPE_STRING_ARRAY:
	{
	    char** array = (char**)value.Data;
	    for (i32 i = 0; i < array_count(array); ++i)
	    {
		String_Value(array[i]);
	    }
	    break;
	}
	case JSON_VALUE_TYPE_OBJECT:
	{
	    JsonObject* obj = (JsonObject*) value.Data;
	    json_object_test(obj);
	    break;
	}
	}
    }
}

void
json_base_test()
{
#if 0

    JsonParser parser;
    json_init(&parser, path);

    JsonObject* root = json_parser_create_root(parser.Tokens);

    json_object_test(root);

    char* tokensAsString = json_tokens_to_string(parser.Tokens);
    json_write_file(&parser, testPath);

    char* str = json_object_to_string(root, 0);
    //GERROR("TOKENS\n\n%s\n", tokensAsString);
    //file_write_string(testPath, str, vstring_length(str));

    //char* str0 = json_tokens_to_string(parser.Tokens);
    //file_write_string(testPath0, str0, vstring_length(str0));

    JsonObject root2;
    root2.Keys = NULL;
    root2.Values = NULL;
    JsonValue va = JSON_STRING("value0");
    json_add_key_value(&root2, "key0", va);
    json_add_key_value(&root2, "key1", JSON_INT(105));
    JsonObject object2;
    object2.Keys = NULL;
    object2.Values = NULL;
    json_add_key_value(&object2, "ObjectProperty0", JSON_STRING("ObjectValue"));
    char** arr1 = NULL;
    array_push(arr1, "ARRAY0");
    array_push(arr1, "ARRAY1");
    json_add_key_value(&object2, "ObjectProperty1", JSON_STRING_ARRAY(arr1));

    JsonObject object22;
    object22.Keys = NULL;
    object22.Values = NULL;
    json_add_key_value(&object22, "Nested0", JSON_FLOAT(12.238f));
    json_add_key_value(&object22, "Nested1", JSON_INT(12));
    JsonObject object222;
    object222.Keys = NULL;
    object222.Values = NULL;
    i32 num = 100;
    JsonValue vnum;
    vnum.Data = &num;
    vnum.Type = JSON_VALUE_TYPE_INT;
    json_add_key_value(&object222, "KeyW0", vnum);
    json_add_key_value(&object222, "KeyW1", JSON_INT(101));
    json_add_key_value(&object222, "KeyW2", JSON_INT(110));
    json_add_key_value(&object222, "KeyW3", JSON_INT(111));
    json_add_key_value(&object22, "Nested2", JSON_OBJECT(&object222));

    json_add_key_value(&object2, "ObjectProperty2", JSON_OBJECT(&object22));
    json_add_key_value(&root2, "key", JSON_OBJECT(&object2));

    char* str2 = json_object_to_string(&root2, 0);
    file_write_string(testPath, str2, vstring_length(str2));

    json_parser_destroy_root(root);

#endif

}

void
json_parse_test()
{

}

void
json_parser_test()
{

    TEST(json_base_test());
}
