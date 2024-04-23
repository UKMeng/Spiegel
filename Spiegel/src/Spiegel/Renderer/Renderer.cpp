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
			glm::vec3 ViewPosition;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Temporary
		Ref<Material> CubeMaterial;
		Ref<Material> LightMaterial;
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

		// temporary
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

		s_Data->CubeMaterial = Material::Create("Test", Renderer::GetShaderLibrary()->Get("Test"));
		s_Data->LightMaterial = Material::Create("Light", Renderer::GetShaderLibrary()->Get("Light"));

		s_Data->CubeMaterial->SetFloat4("objectColor", glm::vec4(1.0f, 0.5f, 0.31f, 1.0f));
		s_Data->CubeMaterial->SetFloat3("pointLight.position", lightPos);
		s_Data->CubeMaterial->SetFloat3("pointLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
		s_Data->CubeMaterial->SetFloat3("pointLight.ambient", glm::vec3(0.2f));
		s_Data->CubeMaterial->SetFloat3("pointLight.diffuse", glm::vec3(0.8f));
		s_Data->CubeMaterial->SetFloat3("pointLight.specular", glm::vec3(1.0f));
		s_Data->CubeMaterial->SetFloat("pointLight.constant", 1.0f);
		s_Data->CubeMaterial->SetFloat("pointLight.linear", 0.09f);
		s_Data->CubeMaterial->SetFloat("pointLight.quadratic", 0.032f);

		s_Data->CubeMaterial->SetFloat3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		s_Data->CubeMaterial->SetFloat3("dirLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
		s_Data->CubeMaterial->SetFloat3("dirLight.ambient", glm::vec3(0.2f));
		s_Data->CubeMaterial->SetFloat3("dirLight.diffuse", glm::vec3(0.8f));
		s_Data->CubeMaterial->SetFloat3("dirLight.specular", glm::vec3(1.0f));

		s_Data->CubeMaterial->SetFloat3("spotLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
		s_Data->CubeMaterial->SetFloat3("spotLight.ambient", glm::vec3(0.5f));
		s_Data->CubeMaterial->SetFloat3("spotLight.diffuse", glm::vec3(10.0f));
		s_Data->CubeMaterial->SetFloat3("spotLight.specular", glm::vec3(15.0f));
		s_Data->CubeMaterial->SetFloat("spotLight.constant", 1.0f);
		s_Data->CubeMaterial->SetFloat("spotLight.linear", 0.09f);
		s_Data->CubeMaterial->SetFloat("spotLight.quadratic", 0.032f);


		s_Data->CubeMaterial->SetFloat3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		s_Data->CubeMaterial->SetFloat3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		s_Data->CubeMaterial->SetFloat3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		s_Data->CubeMaterial->SetFloat("material.shininess", 128.0f);
		Ref<Texture2D> texture3 = Texture2D::Create("assets/textures/container.png");
		Ref<Texture2D> texture4 = Texture2D::Create("assets/textures/container_specular.png");
		s_Data->CubeMaterial->SetTexture2D(18, texture3);
		s_Data->CubeMaterial->SetTexture2D(19, texture4);

		s_Data->CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
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
		s_Data->CubeMaterial->SetFloat3("spotLight.position", camera.GetPosition());
		s_Data->CubeMaterial->SetFloat3("spotLight.direction", camera.GetForwardDirection());
		s_Data->CubeMaterial->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		s_Data->CubeMaterial->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		s_Data->CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data->CameraBuffer.ViewPosition = camera.GetPosition();
		s_Data->CameraUniformBuffer->SetData(&s_Data->CameraBuffer, sizeof(RendererData::CameraData));
	}
	
	void Renderer::RenderScene(const EditorCamera& camera)
	{
		glDisable(GL_BLEND);

		// Draw 10 Cubes
		glm::vec3 cubePositions[] = {
		  glm::vec3(0.0f,  0.0f,  0.0f),
		  glm::vec3(2.0f,  5.0f, -15.0f),
		  glm::vec3(-1.5f, -2.2f, -2.5f),
		  glm::vec3(-3.8f, -2.0f, -12.3f),
		  glm::vec3(2.4f, -0.4f, -3.5f),
		  glm::vec3(-1.7f,  3.0f, -7.5f),
		  glm::vec3(1.3f, -2.0f, -2.5f),
		  glm::vec3(1.5f,  2.0f, -2.5f),
		  glm::vec3(1.5f,  0.2f, -1.5f),
		  glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			DrawCube(model, s_Data->CubeMaterial);
		}


		// DrawCube(glm::mat4(1.0f), s_Data->CubeMaterial);

		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		glm::mat4 lightTransform = glm::mat4(1.0f);
		lightTransform = glm::translate(glm::mat4(1.0f), lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
		DrawCube(lightTransform, s_Data->LightMaterial);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::DrawCube(const glm::mat4& transform, Ref<Material> material, int entityID)
	{
		struct Vertex
		{
			glm::vec4 Position;
			glm::vec2 TexCoord;
			glm::vec3 Normal;
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
			{ ShaderDataType::Float3, "a_Normal" }
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
		
	}
}