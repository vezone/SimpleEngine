#include "WorldOutliner.h"

#include "FileDialog.h"

#include <UI/ui.h>
#include <ECS/ECS.h>
#include <ECS/Components/SpriteComponent.h>
#include <ECS/Components/TransformComponent.h>
#include <Graphics/Texture2D.h>

i32 g_SelectedEntity;

force_inline void
transform_component_panel(Scene* scene, Entity entity)
{
    v4 translation;
    v3 rotation;
    v3 scale;
    TransformComponent* transformComponent = ECS_ENTITY_GET_COMPONENT(scene->World, entity.ID, TransformComponent);
    m4_transform_decompose(transformComponent->Transform, translation, rotation, scale);

    igSliderFloat3("Translation", translation, -10.0f, 10.0f, "%0.2f", ImGuiSliderFlags_None);
    igSliderFloat3("Rotation", rotation, 0.0f, 90.0f, "%0.2f", ImGuiSliderFlags_None);
    igSliderFloat3("Scale", scale, 0.0f, 10.0f, "%0.2f", ImGuiSliderFlags_None);

    m4_transform(translation, scale, rotation, transformComponent->Transform);
}

force_inline void
sprite_component_panel(Scene* scene, Entity entity)
{
    igText("Selected entity: %d", entity.ID);
    SpriteComponent* spriteComponent = ECS_ENTITY_GET_COMPONENT(scene->World, entity.ID, SpriteComponent);

    //igCheckbox("IsTextured", &spriteComponent->IsTextured);
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
properties_panel(Scene* scene)
{
    bool propertiesPanelOpen;
    if (igBegin("Properties Panel", &propertiesPanelOpen, ImGuiWindowFlags_None))
    {
	Entity* entities = scene_get_entities();
	Entity entity = entities[g_SelectedEntity];

	if (igButton("Add Component", ImVec2_(0, 0)))
	{
	    //
	}

	transform_component_panel(scene, entity);
	sprite_component_panel(scene, entity);

	igEnd();
    }
}


void
world_outliner(Scene* scene)
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
		g_SelectedEntity = e;
	    }
	}

	if (igBeginPopupContextWindow("NONE", ImGuiPopupFlags_MouseButtonRight))
	{
	    if (igButton("Add Entity", ImVec2_(0, 0)))
	    {
	    }
	    igEndPopup();
	}

	igEnd();
    }

    properties_panel(scene);
}
