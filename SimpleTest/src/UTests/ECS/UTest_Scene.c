#include "UTest_Scene.h"

#include "UTests/Test.h"
#include <ECS/Scene.h>

#if 0
void scene_create(Scene* scene);
void scene_on_update(Scene* scene);
void scene_destroy(Scene* scene);
Entity* scene_get_entities(Scene* scene);

Entity scene_entity_create(Scene* scene, const char* name);
Entity scene_entity_create_with_random_name(Scene* scene);
void scene_entity_destroy(Scene* scene, Entity entity);
#endif

void
scene_entity_create_test()
{
    Scene scene;
    scene_create(&scene);
    Entity entity = scene_entity_create(&scene, "EntityName");

    String_Equal(entity.Name, "EntityName");
    Condition(entity.ID == ENTITIES_FIRST_ID);
}

void
scene_entity_destroy_test()
{
    Scene scene;
    scene_create(&scene);
    Entity entity = scene_entity_create(&scene, "EntityName");

    Condition(array_count(scene.Entities) == 1);
    I32_Value(array_count(scene.Entities));

    scene_entity_destroy(&scene, entity);

    Condition(array_count(scene.Entities) == 0);
    I32_Value(array_count(scene.Entities));
}

void
scene_test()
{
    TEST(scene_entity_create_test());
    TEST(scene_entity_destroy_test());
}
