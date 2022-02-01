#ifndef WORLD_OUTLINER_H
#define WORLD_OUTLINER_H

#include <ECS/Scene.h>
#include <Utils/SimpleStandardLibrary.h>
#include <Graphics/Camera/EditorCamera.h>
#include <Graphics/Texture2D.h>

void world_outliner_on_attach(Texture2D* defaultTexture);
void world_outliner(Scene* scene, EditorCamera* camera);
Entity world_outliner_get_selected_entity();

#endif
