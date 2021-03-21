#include "SimpleTest.h"
#include <EntryPoint.h>

void create_user_application()
{
    linux_set_current_stack_size(MB(500), MB(528));

    application_init(1280, 720, "Simple Test");

    Layer simpleLayer;
    simpleLayer.Name = "Simple Test Layer";
    simpleLayer.OnAttach   = simple_test_on_attach;
    simpleLayer.OnUpdate   = simple_test_on_update;
    simpleLayer.OnUIRender = simple_test_on_ui;
    simpleLayer.OnEvent    = simple_test_on_event;
    simpleLayer.OnDestoy   = simple_test_on_destroy;

    application_push_layer(simpleLayer);
}
