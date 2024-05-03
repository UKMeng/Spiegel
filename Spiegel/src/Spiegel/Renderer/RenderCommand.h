#pragma once

#include "RendererAPI.h"

namespace spg {
	class RenderCommand {
	public:
		static void Init() {
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear() {
			s_RendererAPI->Clear();
		}

		static void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t count = 0) {
			s_RendererAPI->DrawArrays(vertexArray, count);
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) {
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) {
			s_RendererAPI->DrawLines(vertexArray, count);
		}
		
		static void SetLineWidth(float width) {
			s_RendererAPI->SetLineWidth(width);
		}

		static void SetWireframeMode(bool flag) {
			s_RendererAPI->SetWireframeMode(flag);
		}

		// Blending
		static void SetBlend(bool flag) {
			s_RendererAPI->SetBlend(flag);
		}

		static void SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor) {
			s_RendererAPI->SetBlendFunc(sfactor, dfactor);
		}

		static void SetBlendSeparate(BlendFactor srcRGB, BlendFactor dstRGB, BlendFactor srcAlpha, BlendFactor dstAlpha) {
			s_RendererAPI->SetBlendSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
		}

		// Depth Test
		static void SetDepthTest(bool flag) {
			s_RendererAPI->SetDepthTest(flag);
		}

		static void SetDepthMask(bool flag) {
			s_RendererAPI->SetDepthMask(flag);
		}

		static void SetDepthFunc(CompareFunc func) {
			s_RendererAPI->SetDepthFunc(func);
		}

		// Stencil Test
		static void SetStencilTest(bool flag) {
			s_RendererAPI->SetStencilTest(flag);
		}

		static void SetStencilMask(uint32_t mask) {
			s_RendererAPI->SetStencilMask(mask);
		}

		static void SetStencilFunc(CompareFunc func, int ref, uint32_t mask) {
			s_RendererAPI->SetStencilFunc(func, ref, mask);
		}

		static void SetStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) {
			s_RendererAPI->SetStencilOp(sfail, dpfail, dppass);
		}

		// Face Culling
		static void SetCullFace(bool flag) {
			s_RendererAPI->SetCullFace(flag);
		}

		static void SetCullFaceSide(CullFace side) {
			s_RendererAPI->SetCullFaceSide(side);
		}

		static void SetCullFaceFrontFace(int mode) {
			s_RendererAPI->SetCullFaceFrontFace(mode);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}