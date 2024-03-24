#pragma once

#include "RenderCommand.h"
#include "Shader.h"

namespace spg {
	class Renderer {
		// store the static state of the renderer API
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}