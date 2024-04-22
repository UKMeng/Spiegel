#include <spgpch.h>
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace spg {
	
	// Vertex Buffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
		SPG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
		SPG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		SPG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const {
		SPG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const {
		SPG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		SPG_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// Index Buffer

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count) {
		SPG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);

		// use GL_ARRAY_BUFFER: https://github.com/TheCherno/Hazel/pull/107
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		SPG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const {
		SPG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const {
		SPG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}