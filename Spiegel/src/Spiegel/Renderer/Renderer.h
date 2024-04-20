#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Texture.h"

namespace spg {
	class Renderer {
		// store the static state of the renderer API
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		
		static Ref<ShaderLibrary> GetShaderLibrary();

	private:
		
	};
}