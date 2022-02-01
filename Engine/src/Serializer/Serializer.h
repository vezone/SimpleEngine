#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <ECS/Scene.h>
#include <ECS/ECSTypes.h>

void serializer_serialize_scene(Scene* scene, const char* path);
void serializer_deserialize_scene(Scene* scene, const char* path);

char* serialize_scene_to_json(Scene* scene);
void deserialize_scene_from_json(Scene* scene, char* json);

#endif
