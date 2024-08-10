#pragma once

// #include "Asset.h"
#include "Shader.h"
#include "Spiegel/Renderer/Texture.h"
#include "Spiegel/Renderer/Material.h"
#include "Spiegel/Renderer/Mesh.h"

namespace spg {

	class AssetManager {
	public:

		//struct AssetMetadata {
		//	UUID AssetHandle;
		//	AssetType Type;
		//	std::string FilePath;
		//};

		static void Init();
		static void Shutdown();

		static void LoadShaders();
		static void LoadMeshes();
		
		static Ref<ShaderLibrary> GetShaderLibrary() { return s_ShaderLibrary; }
		static Ref<Shader> GetShader(const std::string& name) { return s_ShaderLibrary->Get(name); }

		static Ref<TextureLibrary> GetTextureLibrary() { return s_TextureLibrary; }
		static Ref<Texture> GetTexture(const std::string& name) { return s_TextureLibrary->Get(name); }

		static Ref<MeshLibrary> GetMeshLibrary() { return s_MeshLibrary; }
		static Ref<Mesh> GetMesh(const std::string& name) { return s_MeshLibrary->Get(name); }
	private:
		static Ref<ShaderLibrary> s_ShaderLibrary;
		static Ref<TextureLibrary> s_TextureLibrary;
		static Ref<MeshLibrary> s_MeshLibrary;
	};
}