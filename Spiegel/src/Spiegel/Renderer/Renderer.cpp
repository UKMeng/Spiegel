#include <spgpch.h>
#include "Renderer.h"

#include "Renderer2D.h"

#include "Spiegel/Platform/OpenGL/OpenGLShader.h"

namespace spg {
	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;
	};
	static RendererData* s_Data = nullptr;

	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();
		s_Data = new RendererData;
		s_Data->m_ShaderLibrary = CreateRef<ShaderLibrary>();
		s_Data->m_ShaderLibrary->Load("ColoredQuad", "assets/shaders/ColoredQuad.glsl");
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::Shutdown() {
		Renderer2D::Shutdown();
		delete s_Data;
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		/*shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);*/
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data->m_ShaderLibrary;
	}
}