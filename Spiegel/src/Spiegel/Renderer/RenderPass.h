#pragma once

#include "Framebuffer.h"
#include "Texture.h"
#include "Spiegel/Scene/Scene.h"

namespace spg {

	class RenderPass
	{
	public:
		RenderPass();
		RenderPass(FramebufferSpecification spec, std::function<void(Ref<Framebuffer>, Ref<Scene>)> renderFunc);

		~RenderPass() = default;

		void Exec(Ref<Scene> scene);

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

		std::function<void(Ref<Framebuffer>, Ref<Scene>)> m_RenderFunc;
	};

}