#include "WorldOutliner.h"

#include "FileDialog.h"

#include <UI/ui.h>
#include <ECS/ECS.h>
#include <ECS/Components/SpriteComponent.h>
#include <ECS/Components/TransformComponent.h>
#include <Graphics/Texture2D.h>

Entity g_SelectedEntity;

force_inline void
transform_component_panel(Scene* scene, Entity entity)
{
    v4 translation;
    v3 rotation;
    v3 scale;
    TransformComponent* transformComponent = ECS_ENTITY_GET_COMPONENT(scene->World, entity.ID, TransformComponent);
    m4_transform_decompose(transformComponent->Transform, translation, rotation, scale);

    igSliderFloat3("Translation", translation, -10.0f, 10.0f, "%0.2f", ImGuiSliderFlags_None);
    igSliderFloat3("Rotation", rotation, 0.0f, 180.0f, "%0.2f", ImGuiSliderFlags_None);
    igSliderFloat3("Scale", scale, 0.0f, 10.0f, "%0.2f", ImGuiSliderFlags_None);

    m4_transform(translation, scale, rotation, transformComponent->Transform);
}

force_inline void
sprite_component_panel(Scene* scene, Entity entity)
{
    igText("Selected entity: %d", entity.ID);
    SpriteComponent* spriteComponent = ECS_ENTITY_GET_COMPONENT(scene->World, entity.ID, SpriteComponent);

    igColorEdit4("Color", spriteComponent->Color, ImGuiColorEditFlags_None);

    if (spriteComponent->IsTextured)
    {
	Texture2D* texture = spriteComponent->Texture;
	static f32 iconWidth = 32.0f;
	static f32 iconHeight = 32.0f;
	static ImVec4 backgroundColor = ImVec4_(0, 0, 0, 1);
	static ImVec2 uv0 = ImVec2_(0, 1);
	static ImVec2 uv1 = ImVec2_(1, 0);
	static bool shouldFileDialogBeenOpened = 0;
	if (igImageButton((ImTextureID)texture->ID, ImVec2_(iconWidth, iconHeight), uv0, uv1, 0, backgroundColor, ImVec4_(1.0f, 1.0f, 1.0f, 1.0f)))
	{
	    shouldFileDialogBeenOpened = 1;
	}

	const char* selectedFile;
	if (igFileDialog(selectedFile, &shouldFileDialogBeenOpened, FilterFlag_IMAGE))
	{
	    GWARNING("Selected File: %s\n", selectedFile);
	    texture2d_delete(spriteComponent->Texture);

	    Texture2D* newTexture = texture2d_create(selectedFile);
	    spriteComponent->Texture = newTexture;

	    shouldFileDialogBeenOpened = 0;
	}
    }

}

force_inline void
properties_panel(Scene* scene, Entity entity)
{
    bool propertiesPanelOpen;
    if (igBegin("Properties Panel", &propertiesPanelOpen, ImGuiWindowFlags_None))
    {
	if (igButton("Add Component", ImVec2_(0, 0)))
	{
	}

	if (ECS_ENTITY_HAS_COMPONENT(scene->World, entity.ID, TransformComponent))
	    transform_component_panel(scene, entity);
	if (ECS_ENTITY_HAS_COMPONENT(scene->World, entity.ID, SpriteComponent))
	    sprite_component_panel(scene, entity);

    }
    igEnd();
}

force_inline f32
f32_rand_range(f32 min, f32 max)
{
    /*
	  0.1 = 10
	  0.1 / 0.01

	  0.2 = 0.04
	  20.  / 4 = 5
    */
    f32 randNumber = max / ((f32) (rand() % i32(min / (min * min))));
    return MIN(MAX(randNumber, min), max);
}

force_inline f32
f32_rand()
{
    f32 randNumber = f32_rand_range(0.1f, 1.0f);
    return randNumber;
}

void
world_outliner(Scene* scene, EditorCamera* camera)
{
    bool worldOutlinerOpen;
    if (igBegin("World Outliner", &worldOutlinerOpen, ImGuiWindowFlags_None))
    {
	Entity* entities = scene_get_entities();
	i32 count = array_count(entities);
	for (i32 e = 0; e < count; e++)
	{
	    Entity entity = entities[e];
	    if (igButton(entity.Name, ImVec2_(0, 0)))
	    {
		g_SelectedEntity = entity;
	    }
	}

	if (igBeginPopupContextWindow("NONE", ImGuiPopupFlags_MouseButtonRight))
	{
	    if (igButton("Add Entity", ImVec2_(0, 0)))
	    {
		Entity emptyEntity = entity_create_with_random_name(scene);
		ECS_ENTITY_ADD_COMPONENT(scene->World, emptyEntity.ID, SpriteComponent);
		ECS_ENTITY_SET_COMPONENT(scene->World, emptyEntity.ID, SpriteComponent, SpriteComponent_ColorXYZW(f32_rand(), f32_rand(), f32_rand(), 1.0));
		ECS_ENTITY_SET_COMPONENT(scene->World, emptyEntity.ID, TransformComponent, TransformComponent_Position(f32_rand_range(0.1, 15), f32_rand_range(0.1, 15), 1.0));
	    }
	    igEndPopup();
	}
    }
    igEnd();

    if (g_SelectedEntity.Name)
    {
	properties_panel(scene, g_SelectedEntity);
    }
}

Entity
world_outliner_get_selected_entity()
{
    return g_SelectedEntity;
}
