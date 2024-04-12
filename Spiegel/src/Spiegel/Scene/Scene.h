#pragma once

#include "Spiegel/Core/Timestep.h"

#include <entt/entt.hpp>

namespace spg {
	// Can not inclue Entity.h here because it will cause a circular dependency
	// So using fowrard declaration
	class Entity;
	
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};
}