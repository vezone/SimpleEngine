#include "EditorCamera.h"

EditorCamera
editor_camera_create(f32 left, f32 right, f32 bot, f32 top)
{
    EditorCamera editorCamera = {};
    OrthographicCamera camera;

    camera.Speed = 1.0f;
    camera.Rotation = 0.0f;
    camera.Left = left;
    camera.Right = right;
    camera.Bot = bot;
    camera.Top = top;
    glm_vec3_zero(camera.Position);
    glm_ortho(left, right, bot, top, -1.0f, 1.0f, camera.ProjectionMatrix);
    glm_mat4_identity(camera.ViewMatrix);
    glm_mat4_mul(camera.ProjectionMatrix,
		 camera.ViewMatrix,
		 camera.ViewProjectionMatrix);

    editorCamera.OrthoCamera = camera;
    return editorCamera;
}

void
editor_camera_on_update(EditorCamera* editorCamera, NativeWindow* window, f32 timestep)
{
    OrthographicCamera* camera = &editorCamera->OrthoCamera;
    if (window_is_key_pressed(window, KEY_W))
    {
	camera->Position[1] += camera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_S))
    {
	camera->Position[1] -= camera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_A))
    {
	camera->Position[0] -= camera->Speed * timestep;
    }
    if (window_is_key_pressed(window, KEY_D))
    {
	camera->Position[0] += camera->Speed * timestep;
    }

    orthographic_camera_set_projection(camera, -camera->ZoomLevel, camera->ZoomLevel, -camera->AspectRatio * camera->ZoomLevel, camera->AspectRatio * camera->ZoomLevel);
    orthographic_camera_recalculate_view_matrix(camera);
}

void
editor_camera_on_event(EditorCamera* editorCamera, Event* event)
{
    OrthographicCamera* camera = &editorCamera->OrthoCamera;
    if (event->Category == MouseCategory)
    {
	if (event->Type == MouseScrolled)
	{
	    MouseScrolledEvent* mevent = (MouseScrolledEvent*)event;

	    if (camera->ZoomLevel > 0.1f)
	    {
		camera->ZoomLevel -= mevent->YOffset / 10;
		event->IsHandled = 1;
	    }
	    else if (mevent->YOffset < 0.0f)
	    {
		camera->ZoomLevel -= mevent->YOffset / 10;
		event->IsHandled = 1;
	    }

	    event->IsHandled = 1;
	}
    }
}
