#include "spgpch.h"
#include "Renderer.h"

#include "Renderer2D.h"
#include "UniformBuffer.h"

#include "Spiegel/Platform/OpenGL/OpenGLShader.h"

// Temporary
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace spg {
	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};
	static RendererData* s_Data = nullptr;

	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();
		s_Data = new RendererData;
		s_Data->m_ShaderLibrary = CreateRef<ShaderLibrary>();
		s_Data->m_ShaderLibrary->Load("ColoredQuad", "assets/shaders/ColoredQuad.glsl");
		s_Data->m_ShaderLibrary->Load("Test", "assets/shaders/Test.glsl");
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

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		SPG_PROFILE_FUNCTION();

		s_Data->CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		SPG_PROFILE_FUNCTION();

		s_Data->CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(RendererData::CameraData));
	}
	
	void Renderer::RenderScene()
	{
		struct Vertex
		{
			glm::vec3 Position;
		};

		int vertexCount = 4;
		int indexCount = 6;

		Ref<Material> material = Material::Create("Test", Renderer::GetShaderLibrary()->Get("Test"));
		Ref<VertexArray> vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexCount * sizeof(Vertex));
		vbo->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			});

		uint32_t indice[] = {
			0, 1, 3,
			1, 2, 3
		};

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(indice, indexCount);
		vao->SetIndexBuffer(quadIB);

		glm::vec3 verticesPosition[4] = {
			glm::vec3(0.5f, 0.5f, 0.0f),
			glm::vec3(0.5f, -0.5f, 0.0f),
			glm::vec3(-0.5f, -0.5f, 0.0f),
			glm::vec3(-0.5f, 0.5f, 0.0f)
		};

		vao->AddVertexBuffer(vbo);

		Vertex* vertices = new Vertex[vertexCount];

		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i].Position = verticesPosition[i];
		}
		vbo->SetData(vertices, sizeof(Vertex) * vertexCount);
		material->Upload();
		RenderCommand::DrawIndexed(vao, indexCount);
	}

	void Renderer::EndScene()
	{
	}
}