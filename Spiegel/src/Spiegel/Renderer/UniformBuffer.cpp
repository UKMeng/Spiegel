#include "spgpch.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "Spiegel/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace spg {
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:   SPG_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLUniformBuffer>(size, binding);
		}
		SPG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}