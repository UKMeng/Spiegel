#pragma once

#include "Scene.h"

#include <entt/entt.hpp>

namespace spg {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			SPG_CORE_ASSERT(!CheckComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			SPG_CORE_ASSERT(CheckComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			SPG_CORE_ASSERT(CheckComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}	

		template<typename T>
		bool CheckComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
	
	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};
}