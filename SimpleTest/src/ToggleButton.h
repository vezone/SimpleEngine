#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#define DEFAULT_BLUE_BTN_COLOR   ((ImVec4) { 0.3f, 0.5f, 1.0f, 1.0f })
#define HOVERED_BLUE_BTN_COLOR   ((ImVec4) { 0.3f, 0.6f, 1.0f, 1.0f })
#define ACTIVE_BLUE_BTN_COLOR    ((ImVec4) { 0.3f, 0.7f, 1.0f, 1.0f })

#define DEFAULT_RED_BTN_COLOR   ((ImVec4) { 1.0f, 0.5f, 0.5f, 1.0f })
#define HOVERED_RED_BTN_COLOR   ((ImVec4) { 1.0f, 0.6f, 0.5f, 1.0f })
#define ACTIVE_RED_BTN_COLOR    ((ImVec4) { 1.0f, 0.7f, 0.5f, 1.0f })

#define DEFAULT_GREEN_BTN_COLOR ((ImVec4) { 0.3f, 0.8f, 0.6f, 1.0f })
#define HOVERED_GREEN_BTN_COLOR ((ImVec4) { 0.3f, 0.8f, 0.6f, 1.0f })
#define ACTIVE_GREEN_BTN_COLOR  ((ImVec4) { 0.3f, 0.8f, 0.6f, 1.0f })

enum ToggleButton_State
{
    ToggleButton_State_None = 0,
    ToggleButton_State_Error = 1,
    ToggleButton_State_Success = 2
};

i32 g_State = ToggleButton_State_None;

#define igToggleButton(name, size, state)				\
    igPushIDInt(1);							\
    if (g_State == ToggleButton_State_None)				\
    {									\
	igPushStyleColorVec4(ImGuiCol_Button, DEFAULT_BLUE_BTN_COLOR);	\
	igPushStyleColorVec4(ImGuiCol_ButtonHovered, HOVERED_BLUE_BTN_COLOR); \
	igPushStyleColorVec4(ImGuiCol_ButtonActive, ACTIVE_BLUE_BTN_COLOR); \
     } \
    else if (g_State == ToggleButton_State_Error)     \
    {					      \
	igPushStyleColorVec4(ImGuiCol_Button, DEFAULT_RED_BTN_COLOR);	\
	igPushStyleColorVec4(ImGuiCol_ButtonHovered, HOVERED_RED_BTN_COLOR); \
	igPushStyleColorVec4(ImGuiCol_ButtonActive, ACTIVE_RED_BTN_COLOR); \
    } \
    else if (g_State == ToggleButton_State_Success)	\
    { \
	igPushStyleColorVec4(ImGuiCol_Button, DEFAULT_GREEN_BTN_COLOR); \
	igPushStyleColorVec4(ImGuiCol_ButtonHovered, HOVERED_GREEN_BTN_COLOR); \
	igPushStyleColorVec4(ImGuiCol_ButtonActive, ACTIVE_GREEN_BTN_COLOR); \
    } \
    if (igButton(name, size))				\
    {							\
	if (g_State == ToggleButton_State_None)		\
	{						\
	    g_State = ToggleButton_State_Error;		\
	}						\
	else if (g_State == ToggleButton_State_Error)	\
	{						\
	    g_State = ToggleButton_State_Success;	\
	}						\
	else if (g_State == ToggleButton_State_Success) \
	{						\
	    g_State = ToggleButton_State_None;		\
	}						\
    }							\
    *state = g_State;					\
    igPopStyleColor(3);					\
    igPopID();


#endif
