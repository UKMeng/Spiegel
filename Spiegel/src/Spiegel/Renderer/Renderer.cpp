#include "spgpch.h"
#include "Renderer.h"
#include "UniformBuffer.h"
#include "Renderer2D.h"

#include "Spiegel/Platform/OpenGL/OpenGLShader.h"

// Temporary
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


namespace spg {
	struct RendererData
	{
		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec3 ViewPosition;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
		Ref<ShaderLibrary> m_ShaderLibrary;
	};

	static RendererData* s_RendererData;
	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();
		s_RendererData = new RendererData;

		s_RendererData->m_ShaderLibrary = CreateRef<ShaderLibrary>();
		s_RendererData->m_ShaderLibrary->Load("ColoredQuad", "assets/shaders/ColoredQuad.glsl");
		s_RendererData->m_ShaderLibrary->Load("Test", "assets/shaders/Test.glsl");
		s_RendererData->m_ShaderLibrary->Load("Light", "assets/shaders/Light.glsl");
		
		// temporary
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		s_RendererData->CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 1);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::Shutdown() {
		Renderer2D::Shutdown();
		delete s_RendererData;
		s_RendererData = nullptr;
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
		return s_RendererData->m_ShaderLibrary;
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_RendererData->CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_RendererData->CameraUniformBuffer->SetData(&s_RendererData->CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		glDisable(GL_BLEND);
		// Spot Light Code
		//s_Data->CubeMaterial->SetFloat3("spotLight.position", camera.GetPosition());
		//s_Data->CubeMaterial->SetFloat3("spotLight.direction", camera.GetForwardDirection());
		//s_Data->CubeMaterial->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		//s_Data->CubeMaterial->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		s_RendererData->CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_RendererData->CameraBuffer.ViewPosition = camera.GetPosition();
		s_RendererData->CameraUniformBuffer->SetData(&s_RendererData->CameraBuffer, sizeof(RendererData::CameraData));
	}
	
	void Renderer::DrawCube(const glm::mat4& transform, Ref<Material> material, int entityID)
	{
		
		struct Vertex
		{
			glm::vec4 Position;
			glm::vec2 TexCoord;
			glm::vec3 Normal;
			int EntityID;
		};

		constexpr int vertexCount = 36;
		constexpr int indexCount = 6;
		constexpr glm::vec3 verticesPosition[] = {
			{-0.5f, -0.5f, -0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f,  0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{-0.5f, -0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{ 0.5f,  0.5f, -0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{-0.5f, -0.5f, -0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{-0.5f, -0.5f,  0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f, -0.5f}
		};
		constexpr glm::vec2 textureCoords[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f},
			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f}
		};
		constexpr glm::vec3 normals[] = {
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f, -1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{ 0.0f,  0.0f,  1.0f },
			{-1.0f,  0.0f,  0.0f },
			{-1.0f,  0.0f,  0.0f },
			{-1.0f,  0.0f,  0.0f },
			{-1.0f,  0.0f,  0.0f },
			{-1.0f,  0.0f,  0.0f },
			{-1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 1.0f,  0.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f, -1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f },
			{ 0.0f,  1.0f,  0.0f }
		};
		

		Ref<VertexArray> vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexCount * sizeof(Vertex));
		
		vbo->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Int, "a_EntityID"},
		});
		vao->AddVertexBuffer(vbo);
		
		/*uint32_t indice[] = {
			0, 1, 2,
			2, 3, 0
		};
		Ref<IndexBuffer> ibo = IndexBuffer::Create(indice, indexCount);
		vao->SetIndexBuffer(ibo);*/
		
		Vertex* vertices = new Vertex[vertexCount];
		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i].Position = transform * glm::vec4(verticesPosition[i], 1.0);
			vertices[i].TexCoord = textureCoords[i];
			// use Normal Matrix to transform normals
			// Reference: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
			vertices[i].Normal = glm::mat3(glm::transpose(glm::inverse(transform))) * normals[i];
			vertices[i].EntityID = entityID;
		}
		vbo->SetData(vertices, sizeof(Vertex) * vertexCount);
		delete[] vertices;

		material->Upload();
		vao->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		// RenderCommand::DrawIndexed(vao, indexCount);
	}

	void Renderer::EndScene()
	{
		// temporary
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}