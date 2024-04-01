#include "spgpch.h"
#include "OpenGLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace spg {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		SPG_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// Create Textures 
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// 分配内存，1之mipmap级别，8位RGB格式
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		// 设置缩小过滤器，使用线性过滤器缩小
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// 设置放大过滤器，使用最近邻过滤器放大
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path) {
		SPG_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			SPG_PROFILE_SCOPE(" stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
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

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		SPG_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		// Create Textures 
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// 分配内存，1之mipmap级别，8位RGB格式
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		// 设置缩小过滤器，使用线性过滤器缩小
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// 设置放大过滤器，使用最近邻过滤器放大
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		// 将图像数据上传到纹理对象中，第一个0表示要更新的mipmap级别
		// 后面两个0，0表示图像数据在纹理中的起始位置
		// GL_RGB表示图像数据使用RGB格式，GL_UNSIGNED_BYTE表示图像数据的数据类型，这里使用的是无符号字节类型
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		SPG_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size) {
		SPG_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3; // bytes per pixel
		SPG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		SPG_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}
}