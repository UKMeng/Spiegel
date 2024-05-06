#include "spgpch.h"
#include "RenderPass.h"
#include "RenderCommand.h"

namespace spg {

	RenderPass::RenderPass(FramebufferSpecification spec)
	{
		m_Framebuffer = Framebuffer::Create(spec);
	}

	void RenderPass::Begin()
	{
		m_Framebuffer->Bind();
		RenderCommand::Clear();
	}

	void RenderPass::End()
	{
		m_Framebuffer->Unbind();
	}
}