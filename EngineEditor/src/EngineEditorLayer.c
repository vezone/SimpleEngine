#include "EngineEditor.h"
#include <EntryPoint.h>

void create_user_application()
{
    linux_set_current_stack_size(MB(500), MB(528));

    application_init(1280, 720, "Simple Editor");

    Layer editorLayer;
    editorLayer.Name = "Engine Editor Layer";
    editorLayer.OnAttach = engine_editor_on_attach;
    editorLayer.OnUpdate = engine_editor_on_update;
    editorLayer.OnUIRender = engine_editor_on_ui_render;
    editorLayer.OnEvent = engine_editor_on_event;
    editorLayer.OnDestoy = engine_editor_on_destroy;

    application_push_layer(editorLayer);

}
