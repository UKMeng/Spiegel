#include "spgpch.h"
#include "OpenGLMaterial.h"

namespace spg {
	
	OpenGLMaterial::OpenGLMaterial(const std::string& name, const Ref<Shader>& shader)
		: m_Name(name), m_Shader(shader)
	{
	}

	void OpenGLMaterial::Upload()
	{
		m_Shader->Bind();
		for (auto& [name, value] : m_UniformInt)
			m_Shader->SetInt(name, value);
		for (auto& [name, value] : m_UniformFloat)
			m_Shader->SetFloat(name, value);
		for (auto& [name, value] : m_UniformFloat3)
			m_Shader->SetFloat3(name, value);
		for (auto& [name, value] : m_UniformFloat4)
			m_Shader->SetFloat4(name, value);
		for (auto& [name, value] : m_UniformMat4)
			m_Shader->SetMat4(name, value);
		for (auto& [slot, texture] : m_Textures)
			texture->Bind(slot);
	}

	void OpenGLMaterial::SetInt(const std::string& name, int value)
	{
		if (m_UniformInt.find(name) == m_UniformInt.end())
			m_UniformInt[name] = value;
		else
			m_UniformInt[name] = value;
	}

	void OpenGLMaterial::SetFloat(const std::string& name, float value)
	{
		if (m_UniformFloat.find(name) == m_UniformFloat.end())
			m_UniformFloat[name] = value;
		else
			m_UniformFloat[name] = value;
	}

	void OpenGLMaterial::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		if (m_UniformFloat3.find(name) == m_UniformFloat3.end())
			m_UniformFloat3[name] = value;
		else
			m_UniformFloat3[name] = value;
	
	}

	void OpenGLMaterial::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		if (m_UniformFloat4.find(name) == m_UniformFloat4.end())
			m_UniformFloat4[name] = value;
		else
			m_UniformFloat4[name] = value;
	}

	void OpenGLMaterial::SetMat4(const std::string& name, const glm::mat4& value)
	{
		if (m_UniformMat4.find(name) == m_UniformMat4.end())
			m_UniformMat4[name] = value;
		else
			m_UniformMat4[name] = value;
	
	}

	void OpenGLMaterial::SetTexture2D(uint32_t slot, const Ref<Texture2D>& texture)
	{
		m_Textures[slot] = texture;
	}
}