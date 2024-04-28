#pragma once

#include "Texture.h"
#include "Material.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace spg {

	enum class MeshTextureType
	{
		DIFFUSE,
		SPECULAR,
	};

	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoords;
			glm::vec3 Normal;
		};

		struct SubMesh
		{
			std::vector<Vertex> Vertices;
			std::vector<uint32_t> Indices;
			std::vector<std::pair<MeshTextureType, Ref<Texture2D>>> Textures;
		};

		Mesh(const std::filesystem::path& path);
		Mesh(const std::string& name, const std::vector<SubMesh>& subMeshes);
		~Mesh() = default;

		void Draw();

		void SetMaterial(const Ref<Material>& material) { m_Material = material; }
		const Ref<Material>& GetMaterial() const { return m_Material; }

		const std::string& GetName() const { return m_Name; }
		const std::filesystem::path& GetFilePath() const { return m_FilePath; }
		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }

	private:
		void LoadMesh(const std::filesystem::path& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		std::vector<std::pair<MeshTextureType, Ref<Texture2D>>> LoadMaterialTextures(aiMaterial* material, aiTextureType type);
	private:
		std::vector<SubMesh> m_SubMeshes;
		std::filesystem::path m_FilePath;
		std::string m_Name;
		Ref<Material> m_Material;
	};

	
}