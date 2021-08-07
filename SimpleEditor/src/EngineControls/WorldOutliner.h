#ifndef WORLD_OUTLINER_H
#define WORLD_OUTLINER_H

#include <ECS/Scene.h>
#include <Utils/Types.h>
#include <Graphics/Camera/EditorCamera.h>

void world_outliner(Scene* scene, EditorCamera* camera);
Entity world_outliner_get_selected_entity();

#endif
