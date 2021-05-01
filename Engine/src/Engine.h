#ifndef ENGINE_H
#define ENGINE_H

#include "Application/Application.h"
#include "Event/Event.h"

//Renderer
#include "Graphics/Renderer2D/OrthographicCamera.h"
#include "Graphics/Renderer2D/EditorCamera.h"
#include "Graphics/Renderer2D/ApplicationCamera.h"
#include "Graphics/Renderer2D/Renderer2D.h"
//Graphics
#include "Graphics/Buffer.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Window.h"

//Simple ui
#include "UI/ui.h"

//ECS
#include "ECS/ECS.h"
#include "ECS/Scene.h"
#include "ECS/Components.h"

//Math
#include "Math/BaseMath.h"

//Utils
#include "Utils/Array.h"
#include "Utils/String.h"
#include "Utils/Logger.h"
#include "Utils/Types.h"
#include "Utils/IO.h"
#include "Utils/MemoryAllocator.h"
#include "Utils/Path.h"
#include "Utils/Asset.h"

//Platform
#include "Platform/Linux/Linux.h"



#endif
