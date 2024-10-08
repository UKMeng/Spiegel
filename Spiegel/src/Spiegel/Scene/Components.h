#pragma once

#include "SceneCamera.h"
#include "Spiegel/Renderer/Texture.h"
#include "Spiegel/Renderer/Font.h"
#include "Spiegel/Renderer/Mesh.h"
#include "Spiegel/Core/UUID.h"
#include "Spiegel/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace spg {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id)
			: ID(id) {}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Chrildren;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
		RelationshipComponent(UUID parentHandle)
			: ParentHandle(parentHandle) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct TextComponent
	{
		std::wstring Text;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Font> Font = Font::GetDefaultFont();

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
		TextComponent(const std::wstring& text)
			: Text(text) {}
	};

	// Forward Declaration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		//template<typename T>
		void Bind()
		{
			InstantiateScript = ScriptEngine::GetInstantiateScript("CreateCameraController");
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// Physics 2D

	struct Rigidbody2DComponent
	{
		enum class BodyType
		{
			Static = 0,
			Dynamic = 1,
			Kinematic = 2
		};

		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Size = { 0.5f, 0.5f };
		glm::vec2 Offset = { 0.0f, 0.0f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };

		float Radius = 0.5f;
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	// 3D
	
	struct LightComponent
	{
		enum class LightType
		{
			Directional = 0,
			Point = 1,
			Spot = 2
		};

		struct DirectionalLight
		{
			glm::vec3 direction = { -0.2f, -1.0f, -0.3f };
			float ambient = 0.05f;
			float diffuse = 0.8f;
			float specular = 1.0f;
			float znear = 1.0f;
			float zfar = 100.0f;
			float frustumWidth = 100.f;
		};

		struct PointLight
		{
			float ambient = 0.05f;
			float diffuse = 0.8f;
			float specular = 1.0f;
			float constant = 1.0f;
			float linear = 0.09f;
			float quadratic = 0.032f;
		};

		struct SpotLight
		{
			// direction is bind to entity
			float cutOff = 12.5f;
			float outerCutOff = 15.0f;
			float ambient = 0.05f;
			float diffuse = 0.8f;
			float specular = 1.0f;
			float constant = 1.0f;
			float linear = 0.09f;
			float quadratic = 0.032f;
		};


		LightType Type = LightType::Directional;
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float HDRIntensity = 1.0f;
		DirectionalLight Dir;
		PointLight Point;
		SpotLight Spot;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
	};

	struct MeshComponent
	{
		Ref<Mesh> Mesh;
		bool isBasic = true;
		std::string CurrentMesh = "Cube";
		
		// PBR Material
		glm::vec3 Albedo = { 0.5f, 0.0f, 0.0f };
		float Metallic = 0.0f;
		float Roughness = 0.5f;
		float AO = 1.0f;
		Ref<Texture2D> AlbedoMap;
		Ref<Texture2D> NormalMap;
		Ref<Texture2D> MetallicMap;
		Ref<Texture2D> RoughnessMap;
		Ref<Texture2D> AOMap;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct SkyboxComponent
	{
		Ref<TextureCubeMap> Skybox;

		SkyboxComponent() = default;
		SkyboxComponent(const SkyboxComponent&) = default;
	};
}