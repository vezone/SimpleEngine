#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H

#include <Engine.h>

void simple_test_on_attach(NativeWindow window);
void simple_test_on_update(f32 timestep);
void simple_test_on_event(Event* event);
void simple_test_on_ui();
void simple_test_on_destroy();

#endif
