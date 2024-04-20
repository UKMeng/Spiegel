#include "spgpch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "Spiegel/Platform/OpenGL/OpenGLVertexArray.h"

namespace spg {
	
	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}