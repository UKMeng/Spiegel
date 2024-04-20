#include "spgpch.h"
#include "RenderCommand.h"

#include "Spiegel/Platform/OpenGL/OpenGLRendererAPI.h"

namespace spg {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}