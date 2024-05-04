#include "spgpch.h"
#include "Mesh.h"
#include "Renderer.h"

#include "Spiegel/Asset/AssetManager.h"


namespace spg {

	Mesh::Mesh(const std::filesystem::path& path)
		: m_FilePath(path), m_Name(path.stem().string())
	{
		LoadMesh(path);
		// TODO: Need a Material Library
		Ref<Material> material = Material::Create("Mesh", AssetManager::GetShaderLibrary()->Get("Mesh"));
		material->SetInt("dirLightCount", 0);
		material->SetInt("pointLightCount", 0);
		material->SetFloat3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		material->SetFloat("material.shininess", 128.0f);
		SetMaterial(material);
	}

	Mesh::Mesh(const std::string& name, const std::vector<SubMesh>& subMeshes)
		: m_Name(name), m_SubMeshes(subMeshes)
	{
		// TODO: Need a Material Library
		Ref<Material> material = Material::Create("Mesh", AssetManager::GetShaderLibrary()->Get("PBR_Mesh"));
		material->SetInt("dirLightCount", 0);
		material->SetInt("pointLightCount", 0);
		material->SetFloat3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		material->SetFloat3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		material->SetFloat("material.shininess", 128.0f);
		SetMaterial(material);
	}

	Ref<Mesh> Mesh::Create(const std::filesystem::path& path)
	{
		return CreateRef<Mesh>(path);
	}

	Ref<Mesh> Mesh::CreateCube()
	{
		SubMesh subMesh;
		subMesh.Vertices = {
			// Back
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
			{ { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
			{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
			{ { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
			{ {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
			// Front
			{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
			{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
			{ {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} },
			{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
			// Left
			{ {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
			{ {-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
			{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
			{ {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
			// Right
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} },
			// Bottom
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
			{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
			{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
			// Top
			{ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
			{ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
			{ {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
		};

		subMesh.Indices = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

		return CreateRef<Mesh>("Cube", std::vector<SubMesh>{ subMesh });
	}

	Ref<Mesh> Mesh::CreateSphere()
	{
		// TODO: use cube sphere https://catlikecoding.com/unity/tutorials/cube-sphere/
		SubMesh subMesh;
		const uint32_t X_SEGMENTS = 32;
		const uint32_t Y_SEGMENTS = 32;
		const float PI = 3.14159265359f;
		for (uint32_t x = 0; x <= X_SEGMENTS; ++x) {
			for (uint32_t y = 0; y <= Y_SEGMENTS; ++y) {
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				subMesh.Vertices.push_back({ {xPos, yPos, zPos}, {xSegment, ySegment}, {xPos, yPos, zPos} });
			}
		}
		// Counter-clockwise
		for (uint32_t x = 0; x < X_SEGMENTS; ++x) {
			for (uint32_t y = 0; y < Y_SEGMENTS; ++y) {
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * x + y);
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * (x + 1) + y);
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * (x + 1) + y + 1);
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * x + y);
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * (x + 1) + y + 1);
				subMesh.Indices.push_back((Y_SEGMENTS + 1) * x + y + 1);
			}
		}
		return CreateRef<Mesh>("Sphere", std::vector<SubMesh>{ subMesh });
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
		Ref<TextureLibrary> textureLibrary = AssetManager::GetTextureLibrary();
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

	void MeshLibrary::Add(const Ref<Mesh>& mesh)
	{
		auto& name = mesh->GetName();
		Add(name, mesh);
	}

	void MeshLibrary::Add(const std::string& name, const Ref<Mesh>& mesh)
	{
		SPG_CORE_ASSERT(!Exists(name), "Shader already exist!");
		m_Meshes[name] = mesh;
	}

	Ref<Mesh> MeshLibrary::Load(const std::filesystem::path& filepath)
	{
		if (Exists(filepath.stem().string())) {
			SPG_CORE_WARN("Mesh already exist: {0}", filepath.stem().string());
			return Get(filepath.stem().string());
		}
		return Load(filepath.stem().string(), filepath);
	}

	Ref<Mesh> MeshLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
	{
		if (Exists(name)) {
			SPG_CORE_WARN("Mesh already exist: {0}", name);
			return Get(name);
		}
		auto mesh = Mesh::Create(filepath);
		Add(name, mesh);
		return mesh;
	}

	Ref<Mesh> MeshLibrary::Get(const std::string& name)
	{
		SPG_CORE_ASSERT(Exists(name), "Mesh not found!");
		return m_Meshes[name];
	}

	bool MeshLibrary::Exists(const std::string& name) const
	{
		return m_Meshes.find(name) != m_Meshes.end();
	}
}