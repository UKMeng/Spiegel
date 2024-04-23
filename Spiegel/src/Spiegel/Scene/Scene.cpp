#include "spgpch.h"
#include "Scene.h"
#include "Entity.h"
#include "ScriptableEntity.h"

#include "Spiegel/Renderer/Renderer.h"
#include "Spiegel/Renderer/Renderer2D.h"
#include "Spiegel/Renderer/Material.h"

#include "Spiegel/Math/Math.h"

#include <glm/glm.hpp>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace spg {

	static b2BodyType GetB2BodyType(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
			case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		SPG_CORE_ASSERT(false, "Unknown Body Type!");
		return b2_staticBody;
	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	template<typename Component>
	void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, std::unordered_map<UUID, entt::entity>& uuidMap)
	{
		auto srcView = srcRegistry.view<Component>();
		for (auto srcEntity : srcView) {
			UUID uuid = srcRegistry.get<IDComponent>(srcEntity).ID;
			entt::entity dstEntityID = uuidMap.at(uuid);
			auto& srcComponent = srcView.get<Component>(srcEntity);
			auto& component = srcRegistry.get<Component>(srcEntity);
			dstRegistry.emplace_or_replace<Component>(dstEntityID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dstEntity, Entity srcEntity)
	{
		if (srcEntity.CheckComponent<Component>()) {
			auto& srcComponent = srcEntity.GetComponent<Component>();
			dstEntity.AddOrReplaceComponent<Component>(srcComponent);
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene>& other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> uuidMap;
		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;

		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView) {
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithID(uuid, name);
			uuidMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<RelationshipComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);
		CopyComponent<TextComponent>(dstSceneRegistry, srcSceneRegistry, uuidMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithID(UUID(), name);
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasParent()) {
			Entity parentEntity = GetEntityByUUID(entity.GetParent());
			auto& children = parentEntity.GetChildren();
			children.erase(std::remove(children.begin(), children.end(), entity.GetUUID()), children.end());
		}
		m_Registry.destroy(entity);
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		// TODO: Parent Child Relationship Duplication
		Entity newEntity = CreateEntity(entity.GetName() + " Duplicated");
		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<TextComponent>(newEntity, entity);
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto view = GetAllEntitiesWith<IDComponent>();
		for (auto entity : view) {
			if (view.get<IDComponent>(entity).ID == uuid)
				return Entity{ entity, this };
		}
	}

	void Scene::OnRuntimeStart()
	{
		m_b2World = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			glm::mat4 transform = GetTransformRelatedToParents(entity);
			glm::vec3 translation = glm::vec3(0.0f);
			glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 scale = glm::vec3(1.0f);
			Math::DecomposeTransform(transform, translation, rotation, scale);

			b2BodyDef bodyDef;
			bodyDef.type = GetB2BodyType(rb2d.Type);
			bodyDef.position.Set(translation.x, translation.y);
			bodyDef.angle = rotation.z;

			b2Body* body = m_b2World->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.CheckComponent<BoxCollider2DComponent>()) {
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * scale.x, bc2d.Size.y * scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
			
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}

			if (entity.CheckComponent<CircleCollider2DComponent>()) {
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * scale.x;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_b2World;
		m_b2World = nullptr;
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		// TODO: Should have One OnUpdate funciton

		Renderer::BeginScene(camera);
		Renderer::RenderScene(camera);
		Renderer::EndScene();

		Renderer2D::BeginScene(camera);
		RenderScene2D();
		Renderer2D::EndScene();

		
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.Instance) {
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->SetApplicationInstance(Application::GetApp());
					nsc.Instance->OnUpdate(ts);
				});
		}

		// Physics Update
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_b2World->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
				auto* body = static_cast<b2Body*>(rb2d.RuntimeBody);
				const auto& position = body->GetPosition();

				if (entity.HasParent()) {
					glm::mat4 parentTransform = GetTransformRelatedToParents(GetEntityByUUID(entity.GetParent()));
					glm::vec3 newPosition = glm::inverse(parentTransform) * glm::vec4(position.x, position.y, 0.0f, 1.0f);
					transform.Translation.x = newPosition.x;
					transform.Translation.y = newPosition.y;
					transform.Rotation.z = glm::degrees(body->GetAngle() - Math::DecomposeTransformToGetRotation(parentTransform).z);
				}
				else {
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = glm::degrees(body->GetAngle());
				}
			}
		}

		// Get Primary Camera
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary) {
					mainCamera = &camera.Camera;
					cameraTransform = GetTransformRelatedToParents({ entity, this });
					break;
				}
			}
		}
		
		// Render 2D
		if (mainCamera) {
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			RenderScene2D();

			Renderer2D::EndScene();
		}
	}

	glm::mat4 Scene::GetTransformRelatedToParents(Entity entity) {
		glm::mat4 parentTransform = glm::mat4(1.0f);
		if (entity.HasParent()) {
			Entity parentEntity = GetEntityByUUID(entity.GetParent());
			parentTransform = GetTransformRelatedToParents(parentEntity);
		}
		return parentTransform * entity.GetComponent<TransformComponent>().GetTransform();
	}

	void Scene::RenderScene2D()
	{
		//Ref<Material> material = Material::Create("Test", Renderer::GetShaderLibrary()->Get("ColoredQuad"));
		//material->SetFloat("u_Time", glfwGetTime());

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawSprite(GetTransformRelatedToParents({entity, this}), sprite, (int)entity);
			//Renderer2D::DrawRect(transform.GetTransform(), { 1.0f, 0.0f, 0.0f, 1.0f }, (int)entity);
			//Renderer2D::DrawMaterial(transform.GetTransform(), (int)entity);
		}

		// Renderer2D::SubmitMaterial(material);

		auto circleView = m_Registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : circleView) {
			auto [transform, circle] = circleView.get<TransformComponent, CircleRendererComponent>(entity);
			Renderer2D::DrawCircle(GetTransformRelatedToParents({ entity, this }), circle.Color, circle.Thickness, (int)entity);
		}

		auto textView = m_Registry.view<TransformComponent, TextComponent>();
		for (auto entity : textView) {
			auto [transform, text] = textView.get<TransformComponent, TextComponent>(entity);
			Renderer2D::DrawText(GetTransformRelatedToParents({ entity, this }), text, (int)entity);
		}

		// Renderer2D::DrawLine({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		// Renderer2D::DrawRect({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height) return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.Primary)
				return Entity{ entity, this };
		}
		return {};
	}


	// TODO: Refactor this
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{

	}
}