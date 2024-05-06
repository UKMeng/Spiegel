#pragma once

#include "Spiegel/Renderer/Framebuffer.h"

namespace spg {
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		/// @brief Invalidate the framebuffer
		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, const void* value) override;

		virtual uint32_t GetColorAttachmentTextureID(uint32_t index = 0) const override { SPG_CORE_ASSERT(index < m_ColorAttachments.size(), "Index out of range!"); return m_ColorAttachments[index]; }
		virtual uint32_t GetDepthAttachmentTextureID() const override { return m_DepthAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;
		
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}