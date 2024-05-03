#pragma once

#include "Spiegel/Renderer/Texture.h"

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace spg {
	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path, bool flip = true);
		OpenGLTexture2D(const FT_Face& face);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }

		virtual void SetData(void* data, uint32_t size) override;
		
		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_LoadStatus; }

		virtual const std::string& GetPath() const override { return m_Path; }

		virtual const std::string& GetName() const override { return m_Name; }

		virtual bool operator==(const Texture& other) const override {
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		}
	private:
		uint32_t m_TextureID;
		std::string m_Path = std::string();
		std::string m_Name = std::string();
		uint32_t m_Width, m_Height;
		GLenum m_InternalFormat, m_DataFormat;
		bool m_LoadStatus = false;
	};

	class OpenGLTextureCubeMap : public TextureCubeMap {
	public:
		OpenGLTextureCubeMap(const std::filesystem::path& path);

		virtual ~OpenGLTextureCubeMap();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetTextureID() const override { return m_TextureID; }

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void SetData(void* data, uint32_t size) override;

		virtual bool IsLoaded() const override { return m_LoadStatus; }

		virtual const std::string& GetPath() const override { return m_Path; }

		virtual const std::string& GetName() const override { return m_Name; }

		virtual bool operator==(const Texture& other) const override {
			return m_TextureID == ((OpenGLTextureCubeMap&)other).m_TextureID;
		}
	private:
		uint32_t m_TextureID;
		std::string m_Path = std::string();
		std::string m_Name = std::string();
		uint32_t m_Width, m_Height;
		GLenum m_InternalFormat = GL_RGB8, m_DataFormat = GL_RGB;
		bool m_LoadStatus = false;
	};
}