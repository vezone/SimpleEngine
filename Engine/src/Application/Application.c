#include "Application.h"
#include "Graphics/Renderer2D/Renderer2D.h"
#include "Graphics/OpenGLBase.h"

Application g_Application;

Application*
application_get()
{
    return &g_Application;
}

void
application_push_layer(Layer layer)
{
    array_push(g_Application.Layers, layer);
    if (layer.OnAttach != NULL)
    {
	layer.OnAttach(g_Application.Window);
    }
}

void
application_init(u32 width, u32 height, const char* name)
{
    i32 isWindowCreated;

    g_Application.IsMinimized = 0;
    g_Application.IsRunning = 1;
    g_Application.Layers = NULL;

    linux_set_current_stack_size(MB(500), MB(528));

    event_init_table(g_Application.EventTypeToString, 32);

    isWindowCreated = window_create(&g_Application.Window, width, height, name, application_on_event);
    if (isWindowCreated == -1)
    {
	GERROR("Can't create window!\n");
	return;
    }

    //OpenGL
    opengl_context_init((GLADloadproc)glfwGetProcAddress);

    Layer layer = {
	.Name = "UI Layer",
	.OnAttach = ui_on_attach,
	.OnEvent = ui_on_event,
	.OnDestoy = ui_on_destoy
    };

    application_push_layer(layer);
}

void
application_start()
{
    f32 tempTime;
    f32 timeStep;
    f32 lastFrameTime;

    g_Application.IsRunning = 1;
    i32 layersCount = array_count(g_Application.Layers);

    while (g_Application.IsRunning)
    {
	tempTime = (f32) glfwGetTime();
	timeStep = tempTime - lastFrameTime;
	lastFrameTime = tempTime;

	if (!g_Application.IsMinimized)
	{
	    for (i32 l = 0; l < layersCount; l++)
	    {
		if (g_Application.Layers[l].Name == NULL)
		    continue;
		if (g_Application.Layers[l].OnUpdate != NULL)
		    g_Application.Layers[l].OnUpdate(timeStep);
	    }

	    ui_begin();
	    for (i32 l = 0; l < layersCount; l++)
	    {
		if (g_Application.Layers[l].Name == NULL)
		    continue;
		if (g_Application.Layers[l].OnUIRender != NULL)
		    g_Application.Layers[l].OnUIRender();
	    }
	    ui_end();
	}

	window_on_update(&g_Application.Window);
    }
}

void
application_on_event(Event* event)
{
    Layer layer;
    if (event->Category == WindowCategory)
    {
	if (event->Type == WindowResized)
	{
	    WindowResizedEvent* wevent = (WindowResizedEvent*) event;

	    if (wevent->Width == 0 || wevent->Height == 0)
	    {
		g_Application.IsMinimized = 1;
	    }

	    renderer_set_viewport(wevent->Width, wevent->Height);
	}
	else if (event->Type == WindowMinimized)
	{
	    g_Application.IsMinimized = 1;
	}
	else if (event->Type == WindowRestored)
	{
	    g_Application.IsMinimized = 0;
	}
    }

    i32 layersCount = array_count(g_Application.Layers);
    for (i32 l = 0; l < layersCount; l++)
    {
	layer = g_Application.Layers[l];
	if (layer.OnEvent != NULL && event->IsHandled == 0)
	{
	    layer.OnEvent(event);
	}
    }
}

void
application_close()
{
    g_Application.IsRunning = 0;
}

void
application_end()
{
    i32 layersCount = array_count(g_Application.Layers);
    for (i32 l = 0; l < layersCount; l++)
    {
	Layer layer = g_Application.Layers[l];
	if (layer.OnDestoy != NULL)
	{
	    if (layer.Name != NULL)
	    {
		GDEBUG("Destoying layer: %s\n", layer.Name);
	    }

	    layer.OnDestoy();
	}
    }

    window_terminate();
}
