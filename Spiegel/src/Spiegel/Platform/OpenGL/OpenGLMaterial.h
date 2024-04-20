#pragma once

#include "Spiegel/Renderer/Material.h"

namespace spg {

	class OpenGLMaterial : public Material
	{
	public:
		OpenGLMaterial(const std::string& name, const Ref<Shader>& shader);

		virtual void Upload() override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetTexture2D(uint32_t slot, const Ref<Texture2D>& texture) override;

	private:
		std::string m_Name;
		Ref<Shader> m_Shader;
		// TODO: Need to Refactor this
		std::unordered_map<std::string, int> m_UniformInt;
		std::unordered_map<std::string, float> m_UniformFloat;
		std::unordered_map<std::string, glm::vec3> m_UniformFloat3;
		std::unordered_map<std::string, glm::vec4> m_UniformFloat4;
		std::unordered_map<std::string, glm::mat4> m_UniformMat4;

		std::unordered_map<uint32_t, Ref<Texture2D>> m_Textures;
	};
}