#pragma once

// For use by Spiegel applications

#include <stdio.h>
#include "Spiegel/Core/Application.h"
#include "Spiegel/Core/Layer.h"
#include "Spiegel/Core/Log.h"
#include "Spiegel/Core/Input.h"
#include "Spiegel/Core/MouseButtonCodes.h"
#include "Spiegel/Core/KeyCodes.h"
#include "Spiegel/Core/Timestep.h"

#include "Spiegel/ImGui/ImGuiLayer.h"

#include "Spiegel/Scene/Scene.h"
#include "Spiegel/Scene/SceneCamera.h"
#include "Spiegel/Scene/Components.h"
#include "Spiegel/Scene/Entity.h"
#include "Spiegel/Scene/ScriptableEntity.h"

#include "Spiegel/Renderer/Renderer.h"
#include "Spiegel/Renderer/Renderer2D.h"
#include "Spiegel/Renderer/RenderCommand.h"
#include "Spiegel/Renderer/Buffer.h"
#include "Spiegel/Renderer/Framebuffer.h"
#include "Spiegel/Renderer/RendererAPI.h"
#include "Spiegel/Renderer/Shader.h"
#include "Spiegel/Renderer/Texture.h"
#include "Spiegel/Renderer/SubTexture2D.h"
#include "Spiegel/Renderer/VertexArray.h"
#include "Spiegel/Renderer/EditorCamera.h"
#include "Spiegel/Renderer/OrthographicCamera.h"
#include "Spiegel/Renderer/OrthographicCameraController.h"