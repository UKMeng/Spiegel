#pragma once

#include "RenderCommand.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "EditorCamera.h"

#include "Spiegel/Asset/Shader.h"


namespace spg {
	class Renderer {
		// store the static state of the renderer API
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void BeginScene(const Camera& camera, const glm::mat4& transform);

		static void BeginScene(const EditorCamera& camera);

		static void EndScene();

		// Draw
		static void DrawMesh(const glm::mat4& transform, Ref<Mesh> mesh, int entityID = -1);
		static void DrawMeshWithoutMaterial(const glm::mat4& transform, Ref<Mesh> mesh);
		static void DrawSkybox(const glm::mat4& view, const glm::mat4& projection, Ref<TextureCubeMap> skybox);
	};
}