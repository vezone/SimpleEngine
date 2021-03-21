#ifndef APPLICATION_CAMERA_H
#define APPLICATION_CAMERA_H

#include "OrthographicCamera.h"

typedef struct ApplicationCamera
{
    OrthographicCamera OrthoCamera;
} ApplicationCamera;

ApplicationCamera application_camera_create(f32 left, f32 right, f32 bot, f32 top);
void application_camera_on_update(ApplicationCamera* appCamera, NativeWindow* window, f32 timestep);
void application_camera_on_event(ApplicationCamera* appCamera, Event* event);

#endif
