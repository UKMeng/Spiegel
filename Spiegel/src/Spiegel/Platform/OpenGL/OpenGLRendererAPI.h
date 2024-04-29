#pragma once

#include "Spiegel/Renderer/RendererAPI.h"

namespace spg {
	class OpenGLRendererAPI : public RendererAPI {
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual void SetLineWidth(float width) override;

		virtual void SetWireframeMode(bool flag) override;

		// Blending
		virtual void SetBlend(bool flag) override;
		virtual void SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor) override;
		virtual void SetBlendSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) override;

		// Depth Test
		virtual void SetDepthTest(bool flag) override;
		virtual void SetDepthMask(bool flag) override;
		virtual void SetDepthFunc(CompareFunc func) override;

		// Stencil Test
		virtual void SetStencilTest(bool flag) override;
		virtual void SetStencilMask(uint32_t mask) override;
		virtual void SetStencilFunc(CompareFunc func, int ref, uint32_t mask) override;
		virtual void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) override;

		// Face Culling
		virtual void SetCullFace(bool flag) override;
		virtual void SetCullFaceSide(CullFace side) override;
		virtual void SetCullFaceFrontFace(int mode) override;
	};
}