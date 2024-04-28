#include "SceneHierarchyPanel.h"

#include <windows.h>
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
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.view<TagComponent>().each([&](auto entityID, auto& tc)
			{
				Entity entity = { entityID, m_Context.get() };
				if(!entity.HasParent()) DrawEntityNode(entity);
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

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
		
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetName();

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool entityDeleted = false;

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Create New Child Entity")) {
				Entity childEntity = m_Context->CreateEntity("Empty Child Entity");
				childEntity.SetParent(entity.GetUUID());
				entity.GetChildren().emplace_back(childEntity.GetUUID());
			}
			if (ImGui::MenuItem("Delete Entity")) {
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			UUID entityID = entity.GetUUID();
			ImGui::Text(entity.GetName().c_str());
			ImGui::SetDragDropPayload("SCENE_ENTITY", &entityID, sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget()) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY");

			if (payload) {
				UUID uuid = *(UUID*)payload->Data;
				Entity child = m_Context->GetEntityByUUID(uuid);

				if (!entity.IsDescendantOf(child)) {
					// Remove from previous parent
					if (child.HasParent()) {
						Entity previousParent = m_Context->GetEntityByUUID(child.GetParent());
						if (previousParent) {
							auto parentChildren = previousParent.GetChildren();
							parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), uuid), parentChildren.end());
						}
					}
					// Add to new parent
					child.SetParent(entity.GetUUID());
					entity.GetChildren().emplace_back(uuid);
				}
			}

			ImGui::EndDragDropTarget();
		}


		if (opened) {
			for (auto child : entity.GetChildren()) {
				Entity e = m_Context->GetEntityByUUID(child);
				if (e) {
					DrawEntityNode(e);
				}
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

	/// @brief Template of drawing components with a diy funciton
	/// @tparam T type of component
	/// @tparam UIFunction 
	/// @param name - name of the component show in the tree node
	/// @param entity - entity that contains the component
	/// @param uiFunction - function defined by user to show the component's properties
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
		if (ImGui::Button("Add Component")) 
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			if (!m_SelectionContext.CheckComponent<CameraComponent>()) {
				if (ImGui::MenuItem("Camera")) {
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<SpriteRendererComponent>()) {
				if (ImGui::MenuItem("Sprite Renderer")) {
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<CircleRendererComponent>()) {
				if (ImGui::MenuItem("Circle Renderer")) {
					m_SelectionContext.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<Rigidbody2DComponent>()) {
				if (ImGui::MenuItem("Rigidbody 2D")) {
					m_SelectionContext.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<BoxCollider2DComponent>()) {
				if (ImGui::MenuItem("BoxCollider 2D")) {
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<CircleCollider2DComponent>()) {
				if (ImGui::MenuItem("CircleCollider 2D")) {
					m_SelectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.CheckComponent<TextComponent>()) {
				if (ImGui::MenuItem("Text")) {
					m_SelectionContext.AddComponent<TextComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.CheckComponent<LightComponent>()) {
				if (ImGui::MenuItem("Light")) {
					m_SelectionContext.AddComponent<LightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.CheckComponent<MeshComponent>()) {
				if (ImGui::MenuItem("Mesh")) {
					m_SelectionContext.AddComponent<MeshComponent>();
					ImGui::CloseCurrentPopup();
				}
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

				if (component.Texture && component.Texture->IsLoaded()) {
					ImGui::ImageButton((ImTextureID)component.Texture->GetTextureID(), { 128.0f, 128.0f }, { 0, 1 }, { 1, 0 });
				}
				else {
					ImGui::Button("Texture", { 128.0f, 128.0f });
				}
				
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
						const wchar_t* path = (const wchar_t*)payload->Data;
						component.Texture = Texture2D::Create(path);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f, "%.2f");
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.02f, 0.0f, 1.0f, "%.2f");
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component) {
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString)) {
					for (int i = 0; i < 3; i++) {
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected)) {
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component) {
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size), 0.1f);
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component) {
			ImGui::DragFloat("Radius", &component.Radius, 0.1f);
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<TextComponent>("Text", entity, [](auto& component) {
				auto& text = component.Text;

				int size_needed = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
				std::string str(size_needed, 0);
				WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &str[0], size_needed, NULL, NULL);

				char buffer[256];
				strncpy_s(buffer, str.c_str(), sizeof(buffer));
				buffer[sizeof(buffer) - 1] = 0;
				if (ImGui::InputTextMultiline("##text", buffer, sizeof(buffer))) {
					str = buffer;
					size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
					text = std::wstring(size_needed, 0);
					MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &text[0], size_needed);
				}
				
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				
				ImGui::Button("Font", { 128.0f, 128.0f });
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
						const wchar_t* path = (const wchar_t*)payload->Data;
						component.Font = CreateRef<Font>(path, component.Font->GetFontSize());
					}
					ImGui::EndDragDropTarget();
				}
			});

		DrawComponent<LightComponent>("Light", entity, [](auto& component) {
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				const char* lightTypeStrings[] = { "Directional Light", "Point Light", "Spot Light" };
				const char* currentLightTypeString = lightTypeStrings[(int)component.Type];

				if (ImGui::BeginCombo("Light Type", currentLightTypeString)) {
					for (int i = 0; i < 3; i++) {
						bool isSelected = currentLightTypeString == lightTypeStrings[i];
						if (ImGui::Selectable(lightTypeStrings[i], isSelected)) {
							currentLightTypeString = lightTypeStrings[i];
							component.Type = (LightComponent::LightType)i;
						}

						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (component.Type == LightComponent::LightType::Directional) {
					DrawVec3Control("Direction", component.Dir.direction);
					ImGui::DragFloat("Ambient", &component.Dir.ambient, 0.01f);
					ImGui::DragFloat("Diffuse", &component.Dir.diffuse, 0.1f);
					ImGui::DragFloat("Specular", &component.Dir.specular, 0.1f);
				}
				if (component.Type == LightComponent::LightType::Point) {
					ImGui::DragFloat("Ambient", &component.Point.ambient, 0.01f);
					ImGui::DragFloat("Diffuse", &component.Point.diffuse, 0.1f);
					ImGui::DragFloat("Specular", &component.Point.specular, 0.1f);
					ImGui::DragFloat("Constant", &component.Point.constant, 0.1f);
					ImGui::DragFloat("Linear", &component.Point.linear, 0.01f);
					ImGui::DragFloat("Quadratic", &component.Point.quadratic, 0.001f);
				}
				if (component.Type == LightComponent::LightType::Spot) {
					ImGui::DragFloat("Cut Off", &component.Spot.cutOff, 0.1f);
					ImGui::DragFloat("Outer Cut Off", &component.Spot.outerCutOff, 0.1f);
					ImGui::DragFloat("Ambient", &component.Spot.ambient, 0.01f);
					ImGui::DragFloat("Diffuse", &component.Spot.diffuse, 0.1f);
					ImGui::DragFloat("Specular", &component.Spot.specular, 0.1f);
					ImGui::DragFloat("Constant", &component.Spot.constant, 0.1f);
					ImGui::DragFloat("Linear", &component.Spot.linear, 0.01f);
					ImGui::DragFloat("Quadratic", &component.Spot.quadratic, 0.001f);
				}
		});

		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component) {
			ImGui::Button("Model", { 128.0f, 128.0f });
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					component.Mesh = CreateRef<Mesh>(path);
				}
				ImGui::EndDragDropTarget();
			}
			});
	}
}