#pragma once

#include "Shader.h"
#include "Texture.h"

namespace spg {
	class Material
	{
	public:
		virtual ~Material() = default;
		
		/// @brief Bind the Shader and Upload the Uniforms
		virtual void Upload() = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetTexture2D(uint32_t slot, const Ref<Texture2D>& texture) = 0;


		static Ref<Material> Create(const std::string& name, const Ref<Shader>& shader);
	
	};
}