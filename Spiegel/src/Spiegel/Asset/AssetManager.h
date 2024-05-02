#pragma once

// #include "Asset.h"
#include "Spiegel/Renderer/Shader.h"
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
		
		static Ref<ShaderLibrary>& GetShaderLibrary() { return s_ShaderLibrary; }
		static Ref<TextureLibrary>& GetTextureLibrary() { return s_TextureLibrary; }
		static Ref<MeshLibrary>& GetMeshLibrary() { return s_MeshLibrary; }
	private:
		static Ref<ShaderLibrary> s_ShaderLibrary;
		static Ref<TextureLibrary> s_TextureLibrary;
		static Ref<MeshLibrary> s_MeshLibrary;
	};
}