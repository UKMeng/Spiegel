#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace spg {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.view<TagComponent>().each([&](auto entityID, auto& tc)
			{
				Entity entity = { entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) m_SelectionContext = {};

		ImGui::End();

		

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		if (opened) ImGui::TreePop();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.CheckComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.CheckComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& tc = entity.GetComponent<TransformComponent>();
				ImGui::DragFloat3("Position", glm::value_ptr(tc.Transform[3]), 0.1f);
				/*ImGui::DragFloat3("Rotation", glm::value_ptr(tc.Rotation));
				ImGui::DragFloat3("Scale", glm::value_ptr(tc.Scale))*/;
				ImGui::TreePop();
			}
		}

		if (entity.CheckComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& cc = entity.GetComponent<CameraComponent>();
				auto& camera = cc.Camera;


				// TODO: Fix two cameras being primary bug
				ImGui::Checkbox("Primary", &cc.Primary);
				
				
				if (ImGui::BeginCombo("Projection", camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic ? "Orthographic" : "Perspective")) {
					if (ImGui::Selectable("Orthographic", camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)) {
						camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
					}
					if (ImGui::Selectable("Perspective", camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)) {
						camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
					}
					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("size", &orthoSize)) {
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNear = camera.GetNearClip();
					if (ImGui::DragFloat("Near", &orthoNear)) {
						camera.SetNearClip(orthoNear);
					}

					float orthoFar = camera.GetFarClip();
					if (ImGui::DragFloat("Far", &orthoFar)) {
						camera.SetFarClip(orthoFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &cc.FixedAspectRatio);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float persFov = camera.GetPerspectiveFOV();
					if (ImGui::DragFloat("FOV", &persFov)) {
						camera.SetPerspectiveFOV(persFov);
					}
					
					float orthoNear = camera.GetNearClip();
					if (ImGui::DragFloat("Near", &orthoNear)) {
						camera.SetNearClip(orthoNear);
					}

					float orthoFar = camera.GetFarClip();
					if (ImGui::DragFloat("Far", &orthoFar)) {
						camera.SetFarClip(orthoFar);
					}
				}

				ImGui::TreePop();
			}
		}

		// TODO: DrawComponent<CameraComponent>("Camera")[]() {... }

		if (entity.CheckComponent<SpriteRendererComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));
				ImGui::TreePop();
			}
		}
	}
}