#include "Event.h"

void event_init_table(char** EventTypeToString, i32 count)
{
    if (count < 16)
    {
        return;
    }

    EventTypeToString[None] = "None";
    EventTypeToString[WindowClosed] = "WindowClosed";
    EventTypeToString[WindowResized] = "WindowResized";
    EventTypeToString[WindowFocused] = "WindowFocused";
    EventTypeToString[WindowUnfocused] = "WindowUnfocused";
    EventTypeToString[WindowMoved] = "WindowMoved";
    EventTypeToString[AppTick] = "AppTick";
    EventTypeToString[AppUpdate] = "AppUpdate";
    EventTypeToString[AppRender] = "AppRender";
    EventTypeToString[KeyPressed] = "KeyPressed";
    EventTypeToString[KeyRealeased] = "KeyRealeased";
    EventTypeToString[KeyTyped] = "KeyTyped";
    EventTypeToString[MouseButtonPressed] = "MouseButtonPressed";
    EventTypeToString[MouseButtonReleased] = "MouseButtonReleased";
    EventTypeToString[MouseMoved] = "MouseMoved";
    EventTypeToString[MouseScrolled] = "MouseScrolled";
}
