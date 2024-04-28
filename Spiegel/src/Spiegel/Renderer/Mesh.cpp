#include "spgpch.h"
#include "Mesh.h"
#include "Renderer.h"


namespace spg {

	Mesh::Mesh(const std::filesystem::path& path)
		: m_FilePath(path), m_Name(path.stem().string())
	{
		LoadMesh(path);
		// TODO: Need a Material Library
		Ref<Material> material = Material::Create("Mesh", Renderer::GetShaderLibrary()->Get("Mesh"));
		material->SetInt("dirLightCount", 0);
		material->SetInt("pointLightCount", 0);
		material->SetFloat3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		material->SetFloat("material.shininess", 128.0f);
		SetMaterial(material);
	}

	void Mesh::LoadMesh(const std::filesystem::path& path)
	{
		Assimp::Importer importer;

		// aiProcess_Triangulate: If the model is not made up of triangles, convert it to triangles.
		// aiProcess_FlipUVs: Flip the texture coordinates on the y-axis.
		// aiProcess_GenNormals: Generate normals if the model doesn't have them.
		// aiProcess_SplitLargeMeshes: Split large meshes into smaller sub-meshes.
		// aiProcess_OptimizeMeshes: Merge small meshes into larger meshes.
		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			SPG_CORE_ERROR("Assimp error: {0}", importer.GetErrorString());
			return;
		}

		m_FilePath = path;
		m_Name = path.stem().string();

		ProcessNode(scene->mRootNode, scene);
	}

	void Mesh::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			SubMesh subMesh;		
			for (uint32_t j = 0; j < mesh->mNumVertices; j++) {
				Vertex vertex;
				vertex.Position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
				vertex.Normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };
				if (mesh->mTextureCoords[0]) {
					vertex.TexCoords = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };
				}
				else {
					vertex.TexCoords = { 0.0f, 0.0f };
				}
				subMesh.Vertices.push_back(vertex);
			}

			for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
				aiFace face = mesh->mFaces[j];
				for (uint32_t k = 0; k < face.mNumIndices; k++) {
					subMesh.Indices.push_back(face.mIndices[k]);
				}
			}

			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
				subMesh.Textures.insert(subMesh.Textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
				subMesh.Textures.insert(subMesh.Textures.end(), specularMaps.begin(), specularMaps.end());
			}

			m_SubMeshes.push_back(subMesh);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	std::vector<std::pair<MeshTextureType, Ref<Texture2D>>> Mesh::LoadMaterialTextures(aiMaterial* material, aiTextureType type)
	{
		std::vector<std::pair<MeshTextureType, Ref<Texture2D>>> textures;
		Ref<TextureLibrary> textureLibrary = Renderer::GetTextureLibrary();
		MeshTextureType mType = (type == aiTextureType_DIFFUSE) ? MeshTextureType::DIFFUSE : MeshTextureType::SPECULAR;
		for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
			aiString str;
			material->GetTexture(type, i, &str);
			std::filesystem::path path = m_FilePath.parent_path() / str.C_Str();
			Ref<Texture2D> texture;
			if (textureLibrary->Exists(str.C_Str())) {
				texture = std::static_pointer_cast<Texture2D>(textureLibrary->Get(str.C_Str()));
			}
			else {
				texture = Texture2D::Create(path, false);
				textureLibrary->Add(str.C_Str(), texture);
			}
			textures.push_back(std::make_pair(mType, texture));
		}
		return textures;
	}
}