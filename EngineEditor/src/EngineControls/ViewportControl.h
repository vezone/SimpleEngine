#ifndef VIEWPORT_CONTROL_H
#define VIEWPORT_CONTROL_H

#include <Utils/Types.h>
#include <Graphics/Renderer2D/EditorCamera.h>

typedef struct ViewportSettings
{
    EditorMatrixType MatrixType;
} ViewportSettings;

void viewport_menu_item();

ViewportSettings viewport_get_settings();

#endif
