#include "Sandbox.h"
#include <EntryPoint.h>

void create_user_application()
{
    linux_set_current_stack_size(MB(500), MB(528));

    application_init(1600, 900, "Sandbox");

    Layer sandboxLayer;
    sandboxLayer.OnAttach = sandbox_on_attach;
    sandboxLayer.OnUpdate = sandbox_on_update;
    sandboxLayer.OnUIRender = sandbox_on_ui_render;
    sandboxLayer.OnEvent = sandbox_on_event;
    sandboxLayer.OnDestoy = sandbox_on_destroy;

    application_push_layer(sandboxLayer);
}
