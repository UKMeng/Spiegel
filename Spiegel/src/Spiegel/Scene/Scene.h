#pragma once

#include "Components.h"
#include "Spiegel/Core/UUID.h"
#include "Spiegel/Core/Timestep.h"
#include "Spiegel/Renderer/EditorCamera.h"
#include "Spiegel/Renderer/Material.h"

#include <entt/entt.hpp>

class b2World;

namespace spg {
	// Can not inclue Entity.h here because it will cause a circular dependency
	// So using fowrard declaration
	class Entity;
	
	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene>& other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetEntityByUUID(UUID uuid);

		Entity GetPrimaryCameraEntity();

		glm::mat4 GetTransformRelatedToParents(Entity entity);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		// Temporary, Should Defined by Render Pass
		void OnUpdateShadow(Timestep ts);

		void SetEnvironment(uint32_t shadowTextureID) { if(m_ShadowMap == nullptr) m_ShadowMap = Texture2D::Create(shadowTextureID); }
		void SetEnvironment(Ref<Texture2D> shadowMap) { m_ShadowMap = shadowMap; }
		Ref<Texture2D> GetEnvironment() { return m_ShadowMap; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void RenderScene();
		void RenderScene2D();
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_b2World = nullptr;

		// environment
		glm::mat4 m_LightSpaceMatrix = glm::mat4(1.0f);
		Ref<Texture2D> m_ShadowMap;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	};
}