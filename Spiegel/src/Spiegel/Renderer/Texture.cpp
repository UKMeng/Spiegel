#include "spgpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Spiegel/Platform/OpenGL/OpenGLTexture.h"

namespace spg {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI()) {
			case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path, bool flip)
	{
		switch (RendererAPI::GetAPI()) {
			case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path.string(), flip);
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const FT_Face& face)
	{
		switch (RendererAPI::GetAPI()) {
			case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(face);
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureCubeMap> TextureCubeMap::Create(const std::filesystem::path& path)
	{
		switch (RendererAPI::GetAPI()) {
			case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCubeMap>(path);
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void TextureLibrary::Add(const std::string& name, const Ref<Texture>& texture) {
		SPG_CORE_ASSERT(!Exists(name), "Texture already exist!");
		m_Textures[name] = texture;
	}

	void TextureLibrary::Add(const Ref<Texture>& texture) {
		auto& name = texture->GetName();
		Add(name, texture);
	}

	Ref<Texture> TextureLibrary::Load(const std::filesystem::path& filepath, TextureType type, bool flip) {
		auto name = filepath.stem().string();
		return Load(name, filepath, type, flip);
	}

	Ref<Texture> TextureLibrary::Load(const std::string& name, const std::filesystem::path& filepath, TextureType type, bool flip) {
		if (Exists(name))
		{
			SPG_CORE_WARN("Texture {0} already exist!", name);
			return Get(name);
		}
		Ref<Texture> texture;
		switch (type)
		{
		case TextureType::Texture2D:
			texture = Texture2D::Create(filepath, flip);
			break;
		case TextureType::TextureCubeMap:
			texture = TextureCubeMap::Create(filepath);
			break;
		default:
			SPG_CORE_ASSERT(false, "Unknown TextureType!");
			break;
		}
		Add(name, texture);
		return texture;
	}

	Ref<Texture> TextureLibrary::Get(const std::string& name) {
		SPG_CORE_ASSERT(Exists(name), "Texture not found!");
		return m_Textures[name];
	}

	bool TextureLibrary::Exists(const std::string& name) const
	{
		return m_Textures.find(name) != m_Textures.end();
	}
}