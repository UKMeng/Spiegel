#include "spgpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace spg {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}