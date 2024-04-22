#include "spgpch.h"
#include "Renderer.h"

#include "Renderer2D.h"
#include "UniformBuffer.h"

#include "Spiegel/Platform/OpenGL/OpenGLShader.h"

// Temporary
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


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
		s_Data->m_ShaderLibrary->Load("Light", "assets/shaders/Light.glsl");
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
		s_Data->CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		s_Data->CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(RendererData::CameraData));
	}
	
	void Renderer::RenderScene(const EditorCamera& camera)
	{
		glDisable(GL_BLEND);

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
			glm::vec3 Normal;
		};

		int vertexCount = 36;
		int indexCount = 6;

		// Light 
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		Ref<VertexArray> lightVAO = VertexArray::Create();

		Ref<Material> material = Material::Create("Test", Renderer::GetShaderLibrary()->Get("Test"));
		Ref<Material> lightMaterial = Material::Create("Light", Renderer::GetShaderLibrary()->Get("Light"));
		
		
		Ref<VertexArray> vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexCount * sizeof(Vertex));
		vbo->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		});

		uint32_t indice[] = {
			0, 1, 2,
			2, 3, 0
		};

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(indice, indexCount);
		vao->SetIndexBuffer(quadIB);

		glm::vec3 verticesPosition[] = {
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

		vao->AddVertexBuffer(vbo);
		lightVAO->AddVertexBuffer(vbo);

		Vertex* vertices = new Vertex[vertexCount];
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


		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i].Position = verticesPosition[i];
			vertices[i].TexCoord = textureCoords[i];
			vertices[i].Normal = normals[i];
		}
		vbo->SetData(vertices, sizeof(Vertex) * vertexCount);

		

		// Render a Cube
		// TODO: Calculate Normal Matrix
		glm::mat4 model = glm::mat4(1.0f);

		material->SetMat4("model", model);
		material->SetFloat4("objectColor", glm::vec4(1.0f, 0.5f, 0.31f, 1.0f));
		material->SetFloat3("viewPos", camera.GetPosition());
		
		material->SetFloat3("light.position", lightPos);
		material->SetFloat3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));
		material->SetFloat3("light.ambient", glm::vec3(0.2f));
		material->SetFloat3("light.diffuse", glm::vec3(0.6f));
		material->SetFloat3("light.specular", glm::vec3(1.0f));
		material->SetFloat3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		material->SetFloat("material.shininess", 32.0f);
		Ref<Texture2D> texture1 = Texture2D::Create("assets/textures/Checkerboard.png");
		Ref<Texture2D> texture2 = Texture2D::Create("assets/textures/ustc.png");
		Ref<Texture2D> texture3 = Texture2D::Create("assets/textures/container.png");
		Ref<Texture2D> texture4 = Texture2D::Create("assets/textures/container_specular.png");
		material->SetTexture2D(16, texture1);
		material->SetTexture2D(17, texture2);
		material->SetTexture2D(18, texture3);
		material->SetTexture2D(19, texture4);
		material->Upload();
		//RenderCommand::DrawIndexed(vao, indexCount);
		vao->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);


		// Render a Light
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel =  glm::translate(glm::mat4(1.0f), lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		lightMaterial->SetMat4("model", lightModel);
		lightMaterial->Upload();
		lightVAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::EndScene()
	{
	}
}