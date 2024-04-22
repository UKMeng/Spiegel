#pragma once

#include "Scene.h"
#include "Components.h"
#include "Spiegel/Core/UUID.h"

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
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
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

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		// Relationship
		void SetParent(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
		UUID GetParent() { return GetComponent<RelationshipComponent>().ParentHandle; }
		std::vector<UUID>& GetChildren() { return GetComponent<RelationshipComponent>().Chrildren; }
		bool HasParent() { return GetComponent<RelationshipComponent>().ParentHandle != 0; }
		bool IsAncesterOf(Entity entity)
		{
			if (entity.HasParent())
			{
				if (entity.GetParent() == GetUUID())
					return true;
				else
					return IsAncesterOf(m_Scene->GetEntityByUUID(entity.GetParent()));
			}
			return false;
		}
		bool IsDescendantOf(Entity entity)
		{
			return entity.IsAncesterOf(*this);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }


	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};
}