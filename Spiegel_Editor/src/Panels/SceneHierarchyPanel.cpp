#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
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

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Context->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext) {
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool entityDeleted = false;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity")) {
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			// TODO: Perf
			bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
			if (opened) {
				DrawComponents(entity);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (entityDeleted) {
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity) m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::PushFont(boldFont);
		ImGui::Text(label.c_str());
		ImGui::PopFont();

		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.9f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.8f, 0.15f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.2f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.1f, 0.8f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

		if (entity.CheckComponent<T>()) {
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove Component")) {
					removeComponent = true;
				}
				ImGui::EndPopup();
			}

			if (open) {
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent) entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.CheckComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		// TODO: FIX CAN ADD MULTIPLE SAME COMPONENTS
		if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				if (!m_SelectionContext.CheckComponent<CameraComponent>()) {
					m_SelectionContext.AddComponent<CameraComponent>();
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Sprite Renderer")) {
				if (!m_SelectionContext.CheckComponent<SpriteRendererComponent>()) {
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
				DrawVec3Control("Position", component.Translation);
				DrawVec3Control("Rotation", component.Rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
				auto& camera = component.Camera;
				// TODO: Fix two cameras being primary bug
				ImGui::Checkbox("Primary", &component.Primary);


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

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear)) {
						camera.SetOrthographicNearClip(orthoNear);
					}

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar)) {
						camera.SetOrthographicFarClip(orthoFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float persFov = camera.GetPerspectiveFOV();
					if (ImGui::DragFloat("FOV", &persFov)) {
						camera.SetPerspectiveFOV(persFov);
					}

					float persNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &persNear)) {
						camera.SetPerspectiveNearClip(persNear);
					}

					float persFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &persFar)) {
						camera.SetPerspectiveFarClip(persFar);
					}
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			});
	}
}