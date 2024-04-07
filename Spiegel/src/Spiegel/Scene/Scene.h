#pragma once

#include "Spiegel/Core/Timestep.h"

#include <entt/entt.hpp>

namespace spg {
	// Can not inclue Entity.h here because it will cause a circular dependency
	// So using fowrard declaration
	class Entity;
	
	class Scene
	{
		friend class Entity;

	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};
}