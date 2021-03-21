#ifndef UI_H
#define UI_H

#include "Utils/Types.h"
#include "Event/Event.h"
#include "Graphics/Window.h"

#include <cimgui.h>

void ui_on_attach(NativeWindow window);
void ui_on_event(Event* event);
void ui_on_destoy();

void ui_block_event(i8 block);

//NOTE(vez): we need write ui code between this two
void ui_begin();
void ui_end();


#endif
