#ifndef UI_H
#define UI_H

#include <Event/Event.h>
#include <Graphics/Window.h>

#include <cimgui.h>
#include <cimguizmo.h>

#define ImVec2_(x, y) ((ImVec2) {x, y})
#define ImVec3_(x, y, z) ((ImVec3) {x, y, z})
#define ImVec4_(x, y, z, w) ((ImVec4) {x, y, z, w})
#define ImVec4_Array(xyzw) (ImVec4_(xyzw[0], xyzw[1], xyzw[2], xyzw[3]))

#define ImVec2_Assign(v1, v2) v1.x=v2.x; v1.y=v2.y
#define ImVec2_Equals(v1, v2) ((v1.x == v2.x && v1.y == v2.y) ? 1 : 0)

#define ImVec4_Assign(v1, v2) v1[0]=v2[0]; v1[1]=v2[1]; v1[2]=v2[2]; v1[3]=v2[3]

#define igBtn(name) igButton(name, ImVec2_(0.0f, 0.0f))

void ui_on_attach(NativeWindow window);
void ui_on_event(Event* event);
void ui_on_destoy();

//#define ui_block_event(block) GERROR("Block: %d, File: %s, Line: %d\n", block, __FILE__, __LINE__)
void ui_block_event(i8 block);

//NOTE(vez): we need write ui code between this two
void ui_begin();
void ui_end();

ImFont** ui_get_fonts();

#endif
