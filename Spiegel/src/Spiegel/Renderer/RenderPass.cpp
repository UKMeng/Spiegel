#include "spgpch.h"
#include "RenderPass.h"
#include "RenderCommand.h"

namespace spg {

	RenderPass::RenderPass(FramebufferSpecification spec, std::function<void(Ref<Framebuffer>, Ref<Scene>)> renderFunc)
		: m_RenderFunc(renderFunc)
	{
		m_Framebuffer = Framebuffer::Create(spec);
	}
	
	void RenderPass::Exec(Ref<Scene> scene)
	{
		m_Framebuffer->Bind();
		RenderCommand::Clear();
		m_RenderFunc(m_Framebuffer, scene);
	}
	
}