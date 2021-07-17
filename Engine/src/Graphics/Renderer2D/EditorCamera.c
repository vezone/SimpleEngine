#include "EditorCamera.h"

//EditorCamera editor_camera_create(f32 zoomLevel, f32 aspectRatio, f32 speed, f32 left, f32 right, f32 bot, f32 top)
EditorCamera
editor_camera_create(EditorCameraSettings settings)
{
    EditorCamera editorCamera = {};
    editorCamera.ZoomLevel = settings.ZoomLevel;
    editorCamera.AspectRatio = settings.AspectRatio;
    editorCamera.Speed = settings.Speed;
    editorCamera.Rotation = 0.0f;
    editorCamera.Orthographic = orthographic_camera_create(settings.Left, settings.Right, settings.Bot, settings.Top);
    return editorCamera;
}

void
editor_camera_on_update(EditorCamera* editorCamera, NativeWindow* window, f32 timestep)
{
    OrthographicCamera* camera = &editorCamera->Orthographic;
    if (window_is_key_pressed(window, KEY_W))
    {
	camera->Position[1] += editorCamera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_S))
    {
	camera->Position[1] -= editorCamera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_A))
    {
	camera->Position[0] -= editorCamera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_D))
    {
	camera->Position[0] += editorCamera->Speed * timestep;
    }

    orthographic_camera_set_projection(camera, -editorCamera->ZoomLevel, editorCamera->ZoomLevel, -editorCamera->AspectRatio * editorCamera->ZoomLevel, editorCamera->AspectRatio * editorCamera->ZoomLevel);
    orthographic_camera_recalculate_view_matrix(camera, editorCamera->Rotation);
}

void
editor_camera_on_event(EditorCamera* editorCamera, Event* event)
{
    if (event->Category == MouseCategory)
    {
	if (event->Type == MouseScrolled)
	{
	    MouseScrolledEvent* mevent = (MouseScrolledEvent*)event;

	    if (editorCamera->ZoomLevel > 0.1f)
	    {
		editorCamera->ZoomLevel -= mevent->YOffset / 10;
	    }
	    else if (mevent->YOffset < 0.0f)
	    {
		editorCamera->ZoomLevel -= mevent->YOffset / 10;
	    }

	    event->IsHandled = 1;
	}
    }
}
