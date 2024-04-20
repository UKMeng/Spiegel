#include "spgpch.h"
#include "Framebuffer.h"
#include "Renderer.h"

#include "Spiegel/Platform/OpenGL/OpenGLFramebuffer.h"

namespace spg {
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:   SPG_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFramebuffer>(spec);
		}
		SPG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}