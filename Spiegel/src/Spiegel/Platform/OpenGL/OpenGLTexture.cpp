#include "spgpch.h"
#include "OpenGLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace spg {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// Create Textures 
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		glTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool flip) : m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(flip);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
		}
		SPG_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 1) {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		SPG_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		// Create Textures 
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, internalFormat, m_Width, m_Height);
		
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		m_LoadStatus = true;

		// Extract shader name from filepath
		auto lastSlash = path.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = path.rfind('.');
		auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
		m_Name = path.substr(lastSlash, count);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const FT_Face& face)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		m_Width = face->glyph->bitmap.width;
		m_Height = face->glyph->bitmap.rows;

		m_InternalFormat = GL_R8;
		m_DataFormat = GL_RED;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, m_InternalFormat, m_Width, m_Height);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		
		m_LoadStatus = true;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	OpenGLTexture2D::OpenGLTexture2D(const uint32_t textureID)
		: m_TextureID(textureID)
	{
		//SPG_CORE_WARN("Texture {0} {1} is being created!", m_TextureID, m_Name);
		m_Name = "Debug";
		m_LoadStatus = true;
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		//SPG_CORE_WARN("Texture {0} {1} is being destroyed!", m_TextureID, m_Name);
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3; // bytes per pixel
		SPG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_TextureID);
	}

	OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::filesystem::path& path)
		: m_Path(path.string())
	{
		int width, height, channels;
		const char* faces[6] = { "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };
		stbi_set_flip_vertically_on_load(false);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_TextureID);
		for (size_t i = 0; i < 6; i++) {
			std::string facePath = m_Path + "/" + faces[i];
			stbi_uc* data = stbi_load(facePath.c_str(), &width, &height, &channels, 0);
			if (i == 0) glTextureStorage2D(m_TextureID, 1, m_InternalFormat, width, height);
			SPG_CORE_ASSERT(data, "Failed to load image!");
			glTextureSubImage3D(m_TextureID, 0, 0, 0, i, width, height, 1, m_DataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		m_Width = width;
		m_Height = height;
		m_LoadStatus = true;
	}

	OpenGLTextureCubeMap::~OpenGLTextureCubeMap()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTextureCubeMap::SetData(void* data, uint32_t size)
	{
		SPG_CORE_ASSERT(false, "Not implemented!");
	}

	void OpenGLTextureCubeMap::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_TextureID);
	}
	
}