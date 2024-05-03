#include "spgpch.h"
#include "Renderer.h"
#include "UniformBuffer.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "Spiegel/Asset/AssetManager.h"
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

		static const uint32_t MaxTextureSlots = 32;
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		Ref<Texture2D> WhiteTexture;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
	};

	static RendererData* s_RendererData;
	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();
		s_RendererData = new RendererData;

		s_RendererData->CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 1);

		s_RendererData->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_RendererData->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_RendererData->TextureSlots[0] = s_RendererData->WhiteTexture;
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

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_RendererData->CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_RendererData->CameraUniformBuffer->SetData(&s_RendererData->CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		RenderCommand::SetBlend(false);
		// Spot Light Code
		//s_Data->CubeMaterial->SetFloat3("spotLight.position", camera.GetPosition());
		//s_Data->CubeMaterial->SetFloat3("spotLight.direction", camera.GetForwardDirection());
		//s_Data->CubeMaterial->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		//s_Data->CubeMaterial->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

		s_RendererData->CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_RendererData->CameraBuffer.ViewPosition = camera.GetPosition();
		s_RendererData->CameraUniformBuffer->SetData(&s_RendererData->CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::DrawSkybox(const glm::mat4& view, const glm::mat4& projection, Ref<TextureCubeMap> skybox)
	{
		RenderCommand::SetDepthFunc(CompareFunc::LessEqual);
		Ref<Shader> skyboxShader = AssetManager::GetShader("Skybox");

		constexpr size_t vertexCount = 36;
		glm::vec3 skyboxVertices[] = {
			// positions          
			{-1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f, -1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f},
			{-1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{ 1.0f, -1.0f, -1.0f},
			{-1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f}
		};

		skyboxShader->Bind();
		skyboxShader->SetMat4("u_View", glm::mat4(glm::mat3(view)));
		skyboxShader->SetMat4("u_Projection", projection);
		skybox->Bind(32);
		Ref<VertexArray> vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexCount * sizeof(glm::vec3));
		vbo->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			});
		vao->AddVertexBuffer(vbo);
		vbo->SetData(skyboxVertices, vertexCount * sizeof(glm::vec3));
		RenderCommand::DrawArrays(vao, vertexCount);
		
		RenderCommand::SetDepthFunc(CompareFunc::Less);
	}

	void Renderer::DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, int entityID)
	{
		RenderCommand::SetCullFace(true);

		struct MeshVertex
		{
			glm::vec4 Position;
			glm::vec2 TexCoord;
			glm::vec3 Normal;
			float DiffuseTextureID;
			float SpecularTextureID;
			int EntityID;
		};

		size_t vertexCount = 0;
		size_t indexCount = 0;

		for (auto& subMesh : mesh->GetSubMeshes()) {
			vertexCount += subMesh.Vertices.size();
			indexCount += subMesh.Indices.size();
		}

		Ref<Material> material = mesh->GetMaterial();

		MeshVertex* MeshVertexBase = nullptr;
		MeshVertex* MeshVertexPtr = nullptr;

		for (auto& subMesh : mesh->GetSubMeshes())
		{
			Ref<VertexArray> vao = VertexArray::Create();
			Ref<VertexBuffer> vbo = VertexBuffer::Create(vertexCount * sizeof(MeshVertex));
			vbo->SetLayout({
				{ ShaderDataType::Float4, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float, "a_DiffuseTextureID"},
				{ ShaderDataType::Float, "a_SpecularTextureID"},
				{ ShaderDataType::Int, "a_EntityID"},
				});
			vao->AddVertexBuffer(vbo);

			// Index Buffer
			size_t indexCount = subMesh.Indices.size();
			uint32_t* meshIndices = new uint32_t[indexCount];		
			for (auto i = 0; i < subMesh.Indices.size(); i++) {
				meshIndices[i] = subMesh.Indices[i];
			}
			Ref<IndexBuffer> ibo = IndexBuffer::Create(meshIndices, indexCount);
			vao->SetIndexBuffer(ibo);
			delete[] meshIndices;

			float dtextureIndex = 0.0f;
			float stextureIndex = 0.0f;
			for (auto& [type, texture] : subMesh.Textures)
			{
				float textureIndex = 0.0f;

				for (uint32_t i = 1; i < s_RendererData->TextureSlotIndex; i++) {
					if (*s_RendererData->TextureSlots[i] == *texture) {
						// texture is already submitted
						textureIndex = (float)i;
						break;
					}
				}

				if (textureIndex == 0.0f) {
					textureIndex = (float)s_RendererData->TextureSlotIndex;
					s_RendererData->TextureSlots[s_RendererData->TextureSlotIndex] = texture;
					s_RendererData->TextureSlotIndex++;
				}

				if (type == MeshTextureType::DIFFUSE) {
					material->SetTexture2D((int)textureIndex, texture);
					dtextureIndex = textureIndex;
				}
				else {
					material->SetTexture2D((int)textureIndex, texture);
					stextureIndex = textureIndex;
				}
			}

			MeshVertex* MeshVertexBase = new MeshVertex[subMesh.Vertices.size()];
			MeshVertex* MeshVertexPtr = MeshVertexBase;

			// Vertex Buffer Data
			for (uint32_t i = 0; i < subMesh.Vertices.size(); i++)
			{
				Mesh::Vertex vertex = subMesh.Vertices[i];
				MeshVertexPtr->Position = transform * glm::vec4(vertex.Position, 1.0);
				MeshVertexPtr->TexCoord = vertex.TexCoords;
				// use Normal Matrix to transform normals
				// Reference: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
				MeshVertexPtr->Normal = glm::mat3(glm::transpose(glm::inverse(transform))) * vertex.Normal;
				MeshVertexPtr->EntityID = entityID;
				MeshVertexPtr->DiffuseTextureID = dtextureIndex;
				MeshVertexPtr->SpecularTextureID = stextureIndex;
				MeshVertexPtr++;
			}

			uint32_t dataSize = (uint8_t*)MeshVertexPtr - (uint8_t*)MeshVertexBase;
			vbo->SetData(MeshVertexBase, dataSize);

			material->Upload();
			RenderCommand::DrawIndexed(vao, indexCount);

			delete[] MeshVertexBase;
			MeshVertexBase = nullptr;
			MeshVertexPtr = nullptr;
		}
		RenderCommand::SetCullFace(false);
	}

	void Renderer::EndScene()
	{
		RenderCommand::SetBlend(true);
	}
}