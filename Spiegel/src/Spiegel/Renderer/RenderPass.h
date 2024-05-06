#pragma once

#include "Framebuffer.h"
#include "Texture.h"

namespace spg {

	class RenderPass
	{
	public:
		RenderPass();
		RenderPass(FramebufferSpecification spec);

		~RenderPass() = default;

		void Begin();
		void End();

		/*void SetClearColor(float r, float g, float b, float a);
		void Clear();*/

		/*void SetViewport(int x, int y, int width, int height);

		void Draw();

		void SetShader();
		void SetTexture();
		void SetVertexBuffer();
		void SetIndexBuffer();

		void SetUniform();
		void SetTextureUniform();

		void SetBlendMode();
		void SetDepthTest();
		void SetCullMode();

		void SetScissor();
		void SetStencil();

		void SetRenderTarget();*/

		Ref<Framebuffer> GetFramebuffer() { return m_Framebuffer; }
		Ref<Texture2D> GetDepthAttachmentTexture() { return Texture2D::Create(m_Framebuffer->GetDepthAttachmentTextureID()); }
	private:
		Ref<Framebuffer> m_Framebuffer;
	};

}