#include "spgpch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace spg {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path) {
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);
		SPG_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		// Create Textures 
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		// �����ڴ棬1֮mipmap����8λRGB��ʽ
		glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

		// ������С��������ʹ�����Թ�������С
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// ���÷Ŵ��������ʹ������ڹ������Ŵ�
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// ��ͼ�������ϴ�����������У���һ��0��ʾҪ���µ�mipmap����
		// ��������0��0��ʾͼ�������������е���ʼλ��
		// GL_RGB��ʾͼ������ʹ��RGB��ʽ��GL_UNSIGNED_BYTE��ʾͼ�����ݵ��������ͣ�����ʹ�õ����޷����ֽ�����
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}
}