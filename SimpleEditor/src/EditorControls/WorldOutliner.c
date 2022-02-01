#include "WorldOutliner.h"

#include "FileDialog.h"

#include <UI/ui.h>
#include <ECS/ECS.h>
#include <ECS/Components/SpriteComponent.h>
#include <ECS/Components/TransformComponent.h>

Entity g_SelectedEntity;
Texture2D* g_DefaultTexture = NULL;

typedef void (*ComponentPanelDelegate)(Scene* scene, Entity entity);

void
world_outliner_on_attach(Texture2D* defaultTexture)
{
    g_DefaultTexture = defaultTexture;
}

force_inline void
base_component_panel(Scene* scene, Entity entity, const char* compName, ComponentPanelDelegate componentPanelDelegate)
{
    if (igCollapsingHeader_BoolPtr(compName, NULL, ImGuiWindowFlags_None))
    {
	componentPanelDelegate(scene, entity);
    }
}

void
_tag_component_panel(Scene* scene, Entity entity)
{
    igText("Name: %s", entity.Name);
    const i32 newNameLength = 128;
    char newName[newNameLength];
    if (igInputText("Name", newName, newNameLength, ImGuiInputTextFlags_None, NULL, NULL))
    {
	GSUCCESS("PIZZZ\n");
    }
}

void
tag_component_panel(Scene* scene, Entity entity)
{
    base_component_panel(scene, entity, "TagComponent", _tag_component_panel);
}

void
_transform_component_panel(Scene* scene, Entity entity)
{
    v3 translation;
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
transform_component_panel(Scene* scene, Entity entity)
{
    base_component_panel(scene, entity, "TransformComponent", _transform_component_panel);
}

void
_sprite_component_panel(Scene* scene, Entity entity)
{
    igText("Selected entity: %d", entity.ID);
    SpriteComponent* spriteComponent = ECS_ENTITY_GET_COMPONENT(scene->World, entity.ID, SpriteComponent);

    igColorEdit4("Color", spriteComponent->Color, ImGuiColorEditFlags_None);

    Texture2D* texture;

    if (spriteComponent->IsTextured)
    {
	texture = spriteComponent->Texture;
    }
    else
    {
	texture = g_DefaultTexture;
    }

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

	if (texture != g_DefaultTexture && spriteComponent->Texture != NULL)
	{
	    texture2d_delete(spriteComponent->Texture);
	}

	spriteComponent->IsTextured = 1;

	Texture2D* newTexture = texture2d_create(selectedFile);
	spriteComponent->Texture = newTexture;

	shouldFileDialogBeenOpened = 0;
    }
}

force_inline void
sprite_component_panel(Scene* scene, Entity entity)
{
    base_component_panel(scene, entity, "SpriteComponent", _sprite_component_panel);
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

	if (ECS_ENTITY_HAS_COMPONENT(scene->World, entity.ID, TagComponent))
	    tag_component_panel(scene, entity);
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

Entity g_ActionEntity;

void
world_outliner(Scene* scene, EditorCamera* camera)
{
    ImGuiID id = 1337;
    ImVec2 defSize = (ImVec2) { 0.0f, 0.0f };

    bool worldOutlinerOpen;
    if (igBegin("World Outliner", &worldOutlinerOpen, ImGuiWindowFlags_None))
    {
	Entity* entities = scene->Entities;
	i32 count = array_count(entities);
	for (i32 e = 0; e < count; e++)
	{
	    Entity entity = entities[e];
	    if (igTreeNodeEx_Str(entity.Name, ImGuiTreeNodeFlags_OpenOnArrow))
	    {
		igTreePop();
	    }

	    if (igIsItemClicked(ImGuiMouseButton_Left))
	    {
		g_SelectedEntity = entity;
	    }
	    else if (igIsItemClicked(ImGuiMouseButton_Right))
	    {
		igOpenPopup_ID(id, ImGuiPopupFlags_None);
		g_ActionEntity = entity;
	    }
	}

	if (igBeginPopupEx(id, ImGuiWindowFlags_None))
	{
	    igText("Actions");
	    igSeparator();
	    static bool addSelected = false;
	    static bool removeSelected = false;
	    if (igSelectable_Bool("Add Entity", addSelected, ImGuiSelectableFlags_SelectOnClick, defSize))
	    {
		Entity emptyEntity = scene_entity_create_default(scene);
		ECS_ENTITY_ADD_COMPONENT(scene->World, emptyEntity.ID, SpriteComponent);
		ECS_ENTITY_SET_COMPONENT(scene->World, emptyEntity.ID, SpriteComponent, SpriteComponent_ColorXYZW(f32_rand(), f32_rand(), f32_rand(), 1.0));
		//ECS_ENTITY_SET_COMPONENT(scene->World, emptyEntity.ID, TransformComponent, TransformComponent_Position(f32_rand_range(0.1, 15), f32_rand_range(0.1, 15), 1.0));
	    }

	    if (igSelectable_Bool("Remove", removeSelected, ImGuiSelectableFlags_SelectOnClick, defSize))
	    {
		scene_entity_destroy(scene, g_ActionEntity);
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
