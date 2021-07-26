#ifndef ENGINE_H
#define ENGINE_H

#include "Application/Application.h"
#include "Event/Event.h"

//Graphics
#include "Graphics/Buffer.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
//Graphics -> Renderer
#include "Graphics/Renderer2D/OrthographicCamera.h"
#include "Graphics/Renderer2D/EditorCamera.h"
#include "Graphics/Renderer2D/Renderer2D.h"

//Simple ui
#include "UI/ui.h"

//ECS
#include "ECS/ECS.h"
#include "ECS/ECSTypes.h"
#include "ECS/Scene.h"
#include "ECS/Components/TagComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/SpriteComponent.h"
#include "ECS/Components/NativeScriptComponent.h"
#include "ECS/Components/CameraComponent.h"

//Math
#include "Math/Math.h"

//Utils
#include "Utils/Array.h"
#include "Utils/HashTable.h"
#include "Utils/String.h"
#include "Utils/Logger.h"
#include "Utils/Types.h"
#include "Utils/IO.h"
#include "Utils/MemoryAllocator.h"
#include "Utils/Path.h"
#include "Utils/Asset.h"
#include "Utils/JSONParser.h"

//Platform
#include "Platform/Linux/Linux.h"

#include "Script/Script.h"

#endif
