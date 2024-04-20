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

	private:
		static RendererAPI* s_RendererAPI;
	};
}