#pragma once

#include "Spiegel/Core/Base.h"

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace spg {
	enum class TextureType {
		Texture2D = 0,
		TextureCubeMap = 1
	};

	class Texture {
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetTextureID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual const std::string& GetName() const = 0;
		// virtual void Unbind() const = 0;

		// virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture {
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::filesystem::path& path, bool flip = true);
		static Ref<Texture2D> Create(const FT_Face& face);
	};

	class TextureCubeMap : public Texture {
	public:
		static Ref<TextureCubeMap> Create(const std::filesystem::path& path);
	};

	class TextureLibrary {
	public:
		void Add(const Ref<Texture>& texture);
		void Add(const std::string& name, const Ref<Texture>& texture);
		Ref<Texture> Load(const std::string& filepath, TextureType type, bool flip = true);
		Ref<Texture> Load(const std::string& name, const std::string& filepath, TextureType type, bool flip = true);

		Ref<Texture> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
	};
}