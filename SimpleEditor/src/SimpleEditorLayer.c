#include "SimpleEditor.h"
#include "EditorPanels/ProfilerPanel.h"
#include <EntryPoint.h>
#include <Engine.h>

void
create_user_application()
{
#if LINUX_PLATFORM
    linux_set_current_stack_size(MB(500), MB(528));
#endif

    application_init(1600, 900, "Simple Editor");

    Layer editorLayer;
    editorLayer.Name = "Simple Editor Layer";
    editorLayer.OnAttach = simple_editor_on_attach;
    editorLayer.OnUpdate = simple_editor_on_update;
    editorLayer.OnUIRender = simple_editor_on_ui_render;
    editorLayer.OnEvent = simple_editor_on_event;
    editorLayer.OnDestoy = simple_editor_on_destroy;

    application_push_layer(editorLayer);
}
