#include "Application.h"
#include "Graphics/Renderer2D/Renderer2D.h"
#include "Graphics/OpenGLBase.h"

Application App;
i32 LayersCount;

Application* application_get()
{
    return &App;
}

void
application_push_layer(Layer layer)
{
    array_push(App.Layers, layer);
    if (layer.OnAttach != NULL)
    {
	layer.OnAttach(App.Window);
    }
}

void application_init(u32 width, u32 height, const char* name)
{
    i32 isWindowCreated;

    App.IsMinimized = 0;
    App.IsRunning = 1;
    App.Layers = NULL;

    linux_set_current_stack_size(MB(500), MB(528));

    event_init_table(App.EventTypeToString, 32);

    isWindowCreated = window_create(&App.Window, width, height, name, application_on_event);
    if (isWindowCreated == -1)
    {
	GERROR("Can't create window!\n");
	return;
    }

    //OpenGL
    opengl_context_init((GLADloadproc)glfwGetProcAddress);

    Layer layer = {};
    layer.Name = "UI Layer";
    layer.OnAttach = ui_on_attach;
    layer.OnEvent = ui_on_event;
    layer.OnDestoy = ui_on_destoy;

    application_push_layer(layer);
}

void application_start()
{
    f32 tempTime;
    f32 timeStep;
    f32 lastFrameTime;

    LayersCount = array_len(App.Layers);

    while (!window_should_close(&App.Window))
    {
	tempTime = (f32) glfwGetTime();
	timeStep = tempTime - lastFrameTime;
	lastFrameTime = tempTime;


	if (!App.IsMinimized)
	{
	    for (i32 l = 0; l < LayersCount; l++)
	    {
		if (App.Layers[l].OnUpdate != NULL)
		    App.Layers[l].OnUpdate(timeStep);
	    }

	    ui_begin();
	    for (i32 l = 0; l < LayersCount; l++)
	    {
		if (App.Layers[l].OnUIRender != NULL)
		    App.Layers[l].OnUIRender();
	    }
	    ui_end();
	}

	window_on_update(&App.Window);

    }

    for (i32 l = 0; l < LayersCount; l++)
    {
	Layer layer = App.Layers[l];
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
		App.IsMinimized = 1;
	    }

	    App.IsMinimized = 0;
	    renderer_set_viewport(wevent->Width, wevent->Height);
	}
	else if (event->Type == WindowMinimized)
	{
	    App.IsMinimized = 1;
	}
	else if (event->Type == WindowRestored)
	{
	    App.IsMinimized = 0;
	}
    }

    for (i32 l = 0; l < LayersCount; l++)
    {
	layer = App.Layers[l];
	if (layer.OnEvent != NULL && event->IsHandled == 0)
	{
	    layer.OnEvent(event);
	}
    }
}
