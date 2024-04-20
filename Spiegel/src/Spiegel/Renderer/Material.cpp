#include "spgpch.h"
#include "Material.h"
#include "RendererAPI.h"
#include "Spiegel/Platform/OpenGL/OpenGLMaterial.h"

namespace spg {
	Ref<Material> Material::Create(const std::string& name, const Ref<Shader>& shader)
	{
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None:    SPG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLMaterial>(name, shader);
		}

		SPG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}