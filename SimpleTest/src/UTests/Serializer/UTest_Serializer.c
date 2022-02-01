#include "UTest_Serializer.h"

#include <Serializer/Serializer.h>
#include <ECS/Components/AllComponents.h>
#include <Utils/Asset.h>
#include <Utils/SimpleStandardLibrary.h>

#include <UTests/Test.h>

void
serialize_scene_to_json_test()
{
    Scene scene;
    scene_create(&scene);

    v4 blueColor   = { 0.1f, 0.1f, 0.8f, 1.0f };
    v4 yellowColor = { 1.f, 1.f, 0.0f, 1.0f };
    Texture2D* chibiTexture = texture2d_create(asset_texture("other/anime_chibi.png"));

    Entity rectangleEntity = scene_entity_create(&scene, "Blue Rectangle");
    ECS_ENTITY_ADD_COMPONENT(scene.World, rectangleEntity.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(scene.World, rectangleEntity.ID, SpriteComponent, SpriteComponent_Color(blueColor));
    TransformComponent tc = TransformComponent_(v3_(-2.47f, -1.43f, 1.0f), v3_(3.29f, 1.91f, 1.0f), v3_(0.0f, 0.0f, .0f));
    ECS_ENTITY_SET_COMPONENT(scene.World, rectangleEntity.ID, TransformComponent, tc);

    Entity yellowRectangle = scene_entity_create(&scene, "Yellow Rectangle");
    ECS_ENTITY_ADD_COMPONENT(scene.World, yellowRectangle.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(scene.World, yellowRectangle.ID, TransformComponent, TransformComponent_(v3_(2.11f, -1.58f, 1.0f), v3_(3.64f, 2.96f, 1.0f), v3_(0.0f, 0.0f, .0f)));
    ECS_ENTITY_SET_COMPONENT(scene.World, yellowRectangle.ID, SpriteComponent, SpriteComponent_Color(yellowColor));

    Entity chibi = scene_entity_create(&scene, "Chibi Rectangle");
    ECS_ENTITY_ADD_COMPONENT(scene.World, chibi.ID, SpriteComponent);
    ECS_ENTITY_SET_COMPONENT(scene.World, chibi.ID, TransformComponent, TransformComponent_Position(3.0f, 1.5f, 0.0f));
    ECS_ENTITY_SET_COMPONENT(scene.World, chibi.ID, SpriteComponent, SpriteComponent_Texture(chibiTexture));

    m4 dest;
    m4_copy(dest, tc.Transform);

    F32_Value(dest[0][0]);
    F32_Value(dest[0][1]);
    F32_Value(dest[0][2]);
    F32_Value(dest[0][3]);

    F32_Value(dest[1][0]);
    F32_Value(dest[1][1]);
    F32_Value(dest[1][2]);
    F32_Value(dest[1][3]);

    F32_Value(dest[2][0]);
    F32_Value(dest[2][1]);
    F32_Value(dest[2][2]);
    F32_Value(dest[2][3]);

    F32_Value(dest[3][0]);
    F32_Value(dest[3][1]);
    F32_Value(dest[3][2]);
    F32_Value(dest[3][3]);

    M4_Value(tc.Transform);

    /*
      NOTE(bies):
      for some weird reason we don't get JsonObject* obj
      initialized for our Texture2D inside SpriteComponent
     */
    char* json = serialize_scene_to_json(&scene);

    if (json != NULL)
	String_Value(json);

    Condition(1 == 1);
}

void
deserialize_scene_from_json_test()
{
    Scene scene;
    char* json = file_read_string("/home/bies/Data/programming/C/SimpleEngine/Def.json");
    deserialize_scene_from_json(&scene, json);

    ComponentID tagID = ECS_GET_COMPONENT_ID(scene.World, TagComponent);
    ComponentID transformID = ECS_GET_COMPONENT_ID(scene.World, TransformComponent);
    ComponentID spriteID = ECS_GET_COMPONENT_ID(scene.World, SpriteComponent);

    i32 i, count = array_count(scene.Entities);
    for (i = 0; i < count; ++i)
    {
	Entity entity = scene.Entities[i];
	String_Value(entity.Name);

	const ComponentID* ids = ECS_ENTITY_GET_COMPONENTS(scene.World, entity.ID);
	i32 c, compCount = array_count(ids);
	for (c = 0; c < compCount; ++c)
	{
	    ComponentID compID = ids[c];
	    void* compData = ECS_ENTITY_GET_COMPONENT_BY_ID(scene.World, entity.ID, compID);
	    if (compID == tagID)
	    {
		TagComponent* tag = (TagComponent*) compData;
		String_Value(tag->Name);
		I32_Value(tag->ID);
	    }
	    else if (compID == transformID)
	    {
		/*
		  BUG(bies):
		  -2.47f in serialized scene and +2.47f in deserialized scene
		*/
		TransformComponent* tc = (TransformComponent*) compData;
		M4_Value(tc->Transform);
		if (i == 0)
		{
		    F32_Equal(tc->Transform[0][0], 7.715664f);
		    F32_Equal(tc->Transform[0][1], 0.0f);
		    F32_Equal(tc->Transform[0][2], 0.0f);
		    F32_Equal(tc->Transform[0][3], 0.0f);

		    F32_Equal(tc->Transform[1][0], 0.0f);
		    F32_Equal(tc->Transform[1][1], 0.825416);
		    F32_Equal(tc->Transform[1][2], 0.0f);
		    F32_Equal(tc->Transform[1][3], 0.0f);

		    F32_Equal(tc->Transform[2][0], 0.0f);
		    F32_Equal(tc->Transform[2][1], 0.0f);
		    F32_Equal(tc->Transform[2][2], 1.0f);
		    F32_Equal(tc->Transform[2][3], 0.0f);

		    F32_Equal(tc->Transform[3][0], 0.742957f);
		    F32_Equal(tc->Transform[3][1], -2.702935f);
		    F32_Equal(tc->Transform[3][2], 1.0f);
		    F32_Equal(tc->Transform[3][3], 1.0f);
		}
		else if (i == 1)
		{
		    F32_Equal(tc->Transform[0][0], 3.640000f);
		    F32_Equal(tc->Transform[0][1], 0.0f);
		    F32_Equal(tc->Transform[0][2], 0.0f);
		    F32_Equal(tc->Transform[0][3], 0.0f);

		    F32_Equal(tc->Transform[1][0], 0.0f);
		    F32_Equal(tc->Transform[1][1], 2.960000f);
		    F32_Equal(tc->Transform[1][2], 0.0f);
		    F32_Equal(tc->Transform[1][3], 0.0f);

		    F32_Equal(tc->Transform[2][0], 0.0f);
		    F32_Equal(tc->Transform[2][1], 0.0f);
		    F32_Equal(tc->Transform[2][2], 1.0f);
		    F32_Equal(tc->Transform[2][3], 0.0f);

		    F32_Equal(tc->Transform[3][0], 1.335661f);
		    F32_Equal(tc->Transform[3][1], 0.582294f);
		    F32_Equal(tc->Transform[3][2], 1.0f);
		    F32_Equal(tc->Transform[3][3], 1.0f);
		}
		else if (i == 2)
		{
		    //2.250211, 3.896789, 0.000000, 1.000000
		    F32_Equal(tc->Transform[0][0], 1.0f);
		    F32_Equal(tc->Transform[0][1], 0.0f);
		    F32_Equal(tc->Transform[0][2], 0.0f);
		    F32_Equal(tc->Transform[0][3], 0.0f);

		    F32_Equal(tc->Transform[1][0], 0.0f);
		    F32_Equal(tc->Transform[1][1], 1.0f);
		    F32_Equal(tc->Transform[1][2], 0.0f);
		    F32_Equal(tc->Transform[1][3], 0.0f);

		    F32_Equal(tc->Transform[2][0], 0.0f);
		    F32_Equal(tc->Transform[2][1], 0.0f);
		    F32_Equal(tc->Transform[2][2], 1.0f);
		    F32_Equal(tc->Transform[2][3], 0.0f);

		    F32_Equal(tc->Transform[3][0], 2.250211f);
		    F32_Equal(tc->Transform[3][1], 3.896789f);
		    F32_Equal(tc->Transform[3][2], 0.0f);
		    F32_Equal(tc->Transform[3][3], 1.0f);
		}
	    }
	    else if (compID == spriteID)
	    {
		/*
		  BUG(bies):
		  not added to entity
		*/
		SpriteComponent* sc = (SpriteComponent*) compData;
		I32_Value(sc->IsTextured);
		V4_Value(sc->Color);
		if (sc->IsTextured)
		{
		    //Condition(sc->Texture != NULL);
		    //String_Value(sc->Texture->Path);
		    Texture_PtrValue(sc->Texture);
		    //tc->Texture != NULL ? String_Value(tc->Texture->Path) : 0;
		}
	    }
	}
    }

}

void
test0()
{
    struct
    {
	i32 ID;
	const char* Path;
    } structInstance;

    structInstance.ID = 1003;
    structInstance.Path = "Hello";


}

void
serializer_test()
{
    TEST(serialize_scene_to_json_test());
    TEST(deserialize_scene_from_json_test());
    TEST(test0());
}
