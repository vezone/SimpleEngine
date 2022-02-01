#include "Serializer.h"

#include <ECS/Components/AllComponents.h>
#include <Utils/SimpleStandardLibrary.h>
#include <Math/M4.h>

JsonValue
get_tag_component(TagComponent* tag)
{
    JsonObject* tagObject = json_object_create();

    JsonValue nameJsonValue = JSON_STRING(tag->Name);
    JsonValue idJsonValue = JSON_INT(tag->ID);

    array_push(tagObject->Keys, "Name");
    array_push(tagObject->Keys, "ID");
    array_push(tagObject->Values, nameJsonValue);
    array_push(tagObject->Values, idJsonValue);

    JsonValue tagValue = JSON_OBJECT(tagObject);
    return tagValue;
}

JsonValue
get_transform_component(TransformComponent* tc)
{
    m4 val;
    m4_copy(val, tc->Transform);
    JsonValue jsonValue = JSON_F32_ARRAY_NEW(val, 16);
    JsonObject* transformObject = json_object_create();

    array_push(transformObject->Keys, "Transform");
    array_push(transformObject->Values, jsonValue);

    JsonValue tcValue = JSON_OBJECT(transformObject);
    return tcValue;
}

JsonValue
get_sprite_component(SpriteComponent* sc)
{
    JsonObject* scObj = json_object_create();

    JsonValue isTexturedJsonValue = JSON_INT(sc->IsTextured);
    JsonValue colorJsonValue = JSON_F32_ARRAY_NEW(sc->Color, 4);

    array_push(scObj->Keys, "IsTextured");
    array_push(scObj->Keys, "Color");
    array_push(scObj->Values, isTexturedJsonValue);
    array_push(scObj->Values, colorJsonValue);

    array_push(scObj->Keys, "Texture");
    if (sc->IsTextured)
    {
	JsonObject* textureObj = json_object_create();
	array_push(textureObj->Keys, "Path");
	array_push(textureObj->Values, JSON_STRING(sc->Texture->Path));
	JsonValue textureJsonValue = JSON_OBJECT(textureObj);
	array_push(scObj->Values, textureJsonValue);
    }
    else
    {
	JsonValue nullValue = JSON_NULL();
	array_push(scObj->Values, nullValue);
    }

    JsonValue scValue = JSON_OBJECT(scObj);
    return scValue;
}

char*
serialize_scene_to_json(Scene* scene)
{
    World* world = scene->World;
    Entity* entities = scene->Entities;
    JsonObject* root = json_object_create();

    i32 i, count = array_count(entities);
    for (i = 0; i < count; ++i)
    {
	Entity entity = entities[i];

	TagComponent* tgc = ECS_ENTITY_GET_COMPONENT(world, entity.ID, TagComponent);
	TransformComponent* tc = ECS_ENTITY_GET_COMPONENT(world, entity.ID, TransformComponent);
	SpriteComponent* sc = ECS_ENTITY_GET_COMPONENT(world, entity.ID, SpriteComponent);

	JsonObject* entityObj = json_object_create();
	JsonValue tagJson = get_tag_component(tgc);
	JsonValue transformJson = get_transform_component(tc);
	JsonValue spriteJson = get_sprite_component(sc);

	array_push(entityObj->Keys, "TagComponent");
	array_push(entityObj->Keys, "TransformComponent");
	array_push(entityObj->Keys, "SpriteComponent");

	array_push(entityObj->Values, tagJson);
	array_push(entityObj->Values, transformJson);
	array_push(entityObj->Values, spriteJson);

	array_push(root->Keys, entity.Name);
	array_push(root->Values, JSON_OBJECT(entityObj));
    }

    char* result = json_object_to_string(root, 0);

    json_object_free(root);

    return result;
}

void
deserialize_scene_from_json(Scene* scene, char* json)
{
#define GetJsonValues(compValue)					\
    ({									\
	JsonObject* obj = (JsonObject*) compValue.Data;			\
	vassert(obj != NULL && "compValue.Data is NULL!");		\
	vassert(obj->Values != NULL && "((JsonObject*)compValue.Data)->Values is NULL!"); \
	obj->Values;							\
    })

    scene_create(scene);

    JsonParser parser;
    json_parse_string(&parser, json);
    JsonObject* root = parser.Object;

    i32 i, entitiesCount = array_count(root->Keys);
    for (i = 0; i < entitiesCount; ++i)
    {
	const char* key = root->Keys[i];
	JsonValue value = root->Values[i];
	Entity entity = scene_entity_create(scene, key);

	vassert(value.Type == JSON_VALUE_TYPE_OBJECT && "root->Values should be JSON_VALUE_TYPE_OBJECT!");

	JsonObject* compObj = (JsonObject*) value.Data;
	i32 c, compsCount = array_count(compObj->Keys);
	for (c = 0; c < compsCount; ++c)
	{
	    const char* compKey = compObj->Keys[c];
	    JsonValue compValue = compObj->Values[c];

	    if (string_compare(compKey, "TransformComponent"))
	    {
		TransformComponent tc;
		JsonValue* vals = GetJsonValues(compValue);
		m4_copy(tc.Transform, *((m4*)vals[0].Data));

		ECS_ENTITY_SET_COMPONENT(scene->World, entity.ID, TransformComponent, tc);
	    }
	    else if (string_compare(compKey, "SpriteComponent"))
	    {
		SpriteComponent sc;
		JsonValue* vals = GetJsonValues(compValue);
		sc.IsTextured = *((i32*)vals[0].Data);
		v4_copy(sc.Color, *((v4*)vals[1].Data));

		JsonValue textJsonValue = vals[2];
		if (!sc.IsTextured || textJsonValue.Type == JSON_VALUE_TYPE_STRING)
		{
		    sc.Texture = NULL;
		}
		else
		{
		    JsonObject* textObj = ((JsonObject*)vals[2].Data);
		    const char* textPath = (const char*)textObj->Values[0].Data;
		    sc.Texture = texture2d_create(textPath);
		}

		ECS_ENTITY_ADD_COMPONENT(scene->World, entity.ID, SpriteComponent);
		ECS_ENTITY_SET_COMPONENT(scene->World, entity.ID, SpriteComponent, sc);
	    }
	}
    }

#undef GetJsonValues
}
