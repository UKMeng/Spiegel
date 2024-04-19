#include "spgpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2) return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}


namespace spg {

    YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec2& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {

    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        SPG_CORE_ASSERT(entity.CheckComponent<IDComponent>(), "Entity must have an IDComponent")

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        if (entity.CheckComponent<TagComponent>()) {
            out << YAML::Key << "TagComponent"; 
			out << YAML::BeginMap; // TagComponent
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap; // TagComponent
        }

        if (entity.CheckComponent<TransformComponent>()) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent
            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Position" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
            out << YAML::EndMap; // TransformComponent
        }

        if (entity.CheckComponent<CameraComponent>()) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent
            auto& cc = entity.GetComponent<CameraComponent>();
            auto& camera = cc.Camera;
            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; // Camera
            out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
            out << YAML::EndMap; // CameraComponent
        }

        if (entity.CheckComponent<SpriteRendererComponent>()) {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent
            auto& src = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << src.Color;
            out << YAML::EndMap; // SpriteRendererComponent
        }

        if (entity.CheckComponent<CircleRendererComponent>()) {
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent
			auto& crc = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << crc.Color;
			out << YAML::Key << "Thickness" << YAML::Value << crc.Thickness;
			out << YAML::EndMap; // CircleRendererComponent
		}

        if (entity.CheckComponent<Rigidbody2DComponent>()) {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap; // Rigidbody2DComponent
            auto& r2d = entity.GetComponent<Rigidbody2DComponent>();
            out << YAML::Key << "Type" << YAML::Value << (int)r2d.Type;
            out << YAML::Key << "FixedRotation" << YAML::Value << r2d.FixedRotation;
            out << YAML::EndMap; // Rigidbody2DComponent
        }

        if (entity.CheckComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent
			auto& b2d = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Size" << YAML::Value << b2d.Size;
			out << YAML::Key << "Offset" << YAML::Value << b2d.Offset;
            out << YAML::Key << "Density" << YAML::Value << b2d.Density;
            out << YAML::Key << "Friction" << YAML::Value << b2d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << b2d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << b2d.RestitutionThreshold;
			out << YAML::EndMap; // BoxCollider2DComponent
		}

        if (entity.CheckComponent<CircleCollider2DComponent>()) {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap; // CircleCollider2DComponent
            auto& c2d = entity.GetComponent<CircleCollider2DComponent>();
            out << YAML::Key << "Radius" << YAML::Value << c2d.Radius;
            out << YAML::Key << "Offset" << YAML::Value << c2d.Offset;
            out << YAML::Key << "Density" << YAML::Value << c2d.Density;
            out << YAML::Key << "Friction" << YAML::Value << c2d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << c2d.Restitution;
            out << YAML::Key << "RestitutionThreshold" << YAML::Value << c2d.RestitutionThreshold;
            out << YAML::EndMap; // CircleCollider2DComponent
        }

        // TODO: TextComponent

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        // Create directory if it doesn't exist
        std::filesystem::path directory = std::filesystem::path(filepath).parent_path();
        if (!std::filesystem::exists(directory)) std::filesystem::create_directories(directory);

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Scene Name";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.view<TagComponent>().each([&](auto entityID, auto& tc)
            {
                Entity entity = { entityID, m_Scene.get() };
                if (!entity) return;

                SerializeEntity(out, entity);
            });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    //void SceneSerializer::SerializeRuntime(const std::string& filepath)
    //{
    //    // Not implemented
    //    // SPG_CORE_ASSERT(false);
    //}

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"]) return false;

        std::string sceneName = data["Scene"].as<std::string>();
        SPG_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        auto entities = data["Entities"];
        if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent) {
					name = tagComponent["Tag"].as<std::string>();
				}

                SPG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
				
                Entity deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent) {
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Position"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

                auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					cc.Camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
                    // TODO: Add Texture deserialization
				}

                auto circleRendererComponent = entity["CircleRendererComponent"];
                if (circleRendererComponent) {
                    auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
                    crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
                    crc.Thickness = circleRendererComponent["Thickness"].as<float>();
                }

                auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
                if (rigidbody2DComponent) {
                    auto& r2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    r2d.Type = (Rigidbody2DComponent::BodyType)rigidbody2DComponent["Type"].as<int>();
                    r2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
                }

                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
                if (boxCollider2DComponent) {
					auto& b2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					b2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					b2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					b2d.Density = boxCollider2DComponent["Density"].as<float>();
					b2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					b2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					b2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

                auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
                if (circleCollider2DComponent) {
					auto& c2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					c2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					c2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					c2d.Density = circleCollider2DComponent["Density"].as<float>();
					c2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					c2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					c2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}
			}
		}

        return true;
    }

    //bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    //{
    //    // Not implemented
    //    // SPG_CORE_ASSERT(false);
    //    return false;
    //}
}