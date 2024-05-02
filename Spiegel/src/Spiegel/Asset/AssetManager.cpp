#include "spgpch.h"
#include "AssetManager.h"

namespace spg {

	void AssetManager::Init()
	{
		s_ShaderLibrary = CreateRef<ShaderLibrary>();
		s_ShaderLibrary->Load("ColoredQuad", "assets/shaders/ColoredQuad.glsl");
		s_ShaderLibrary->Load("Test", "assets/shaders/Test.glsl");
		s_ShaderLibrary->Load("Light", "assets/shaders/Light.glsl");
		s_ShaderLibrary->Load("Mesh", "assets/shaders/Mesh.glsl");
		
		
		s_TextureLibrary = CreateRef<TextureLibrary>();
		
		
		s_MeshLibrary = CreateRef<MeshLibrary>();
		s_MeshLibrary->Add("Cube", Mesh::CreateCube());
		s_MeshLibrary->Add("Shpere", Mesh::CreateSphere());
	}

	void AssetManager::Shutdown()
	{
		s_ShaderLibrary.reset();
		s_TextureLibrary.reset();
	}

	Ref<ShaderLibrary> AssetManager::s_ShaderLibrary;
	Ref<TextureLibrary> AssetManager::s_TextureLibrary;
	Ref<MeshLibrary> AssetManager::s_MeshLibrary;
}