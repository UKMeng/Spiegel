#include "EditorLayer.h"

#include "Spiegel/Scene/SceneSerializer.h"
#include "Spiegel/Utils/PlatformUtils.h"
#include "Spiegel/Math/Math.h"
#include "Spiegel/Scripting/ScriptEngine.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace spg {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{

	}

	void EditorLayer::OnAttach()
	{
		ScriptEngine::Init();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_IconPlay = Texture2D::Create("assets/icons/Play.png");
		m_IconStop = Texture2D::Create("assets/icons/Stop.png");

		// m_CameraController.SetZoomLevel(5.0f);

		// Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER,  FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		// temporary RenderPass test
		FramebufferSpecification shadowMapSpec;
		shadowMapSpec.Attachments = { FramebufferTextureFormat::Depth };
		shadowMapSpec.Width = 1024;
		shadowMapSpec.Height = 1024;

		std::function<void(Ref<Framebuffer>, Ref<Scene>)> shadowPassFunc = [](Ref<Framebuffer> framebuffer, Ref<Scene> scene)
		{
			int pointLightCount = 0;
			
			glm::mat4& lightSpaceMatrix = scene->GetLightSpaceMatrix();
			auto view = scene->GetAllEntitiesWith<TransformComponent, LightComponent>();
			for (auto entity : view) {
				auto [transform, light] = view.get<TransformComponent, LightComponent>(entity);
				if (light.Type == LightComponent::LightType::Directional) {
					// Maybe Use a Fake Camera
					float Near = light.Dir.znear, Far = light.Dir.zfar;
					float right = light.Dir.frustumWidth / 2.0f, left = -right, top = right, bottom = -top;
					glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, Near, Far);
					glm::mat4 lightView = glm::lookAt(-light.Dir.direction * 1.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					
					lightSpaceMatrix = lightProjection * lightView;
					pointLightCount = 1;
					break;
				}
			}
		
			if (pointLightCount == 0) return;
		
			Ref<Shader> shader = AssetManager::GetShader("ShadowMap");
			shader->Bind();
			shader->SetMat4("u_ViewProjection", lightSpaceMatrix);

			{
				auto view = scene->GetAllEntitiesWith<TransformComponent, MeshComponent>();
				for (auto entity : view) {
					auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
					Renderer::DrawMeshWithoutMaterial(scene->GetTransformRelatedToParents({ entity, scene.get() }), mesh.Mesh);
				}
			}

			framebuffer->Unbind();

			scene->SetEnvironment(framebuffer->GetDepthAttachmentTextureID());
		};

		m_Pipeline = CreateRef<Pipeline>();
		m_Pipeline->AddPass(CreateRef<RenderPass>(shadowMapSpec, shadowPassFunc));
		
		//m_ShadowPass = CreateRef<RenderPass>(shadowMapSpec, shadowPassFunc);

		NewScene();

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f); // 1.778f is the aspect ratio of 16:9

		// Setting Init
		m_VSync = Application::Get().GetWindow().IsVSync();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_TempFrameTimeForRefresh += ts;
		if (m_TempFrameTimeForRefresh > 0.5f) {
			m_TempFrameTimeForRefresh = 0.0f;
			m_FrameTime = ts;
		}
		
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Renderer
		Renderer2D::ResetStats();

		// Shadow Pass
		// m_ShadowPass->Begin();
		// m_ActiveScene->OnUpdateShadow(ts);
		// m_ShadowPass->End();
		//
		// m_ActiveScene->SetEnvironment(m_ShadowPass->GetFramebuffer()->GetDepthAttachmentTextureID());
		//m_ShadowPass->Exec(m_ActiveScene);

		m_Pipeline->Exec(m_ActiveScene);
		
		// Main Pass
		m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
		// Clear Attachment to -1
		m_Framebuffer->ClearAttachment(1, (void*)-1);
		
		switch (m_SceneState)
		{
			case SceneState::Edit:
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			case SceneState::Play:
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
		}

		// Mouse Picking
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my; // make left bottom corner the origin
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();
		
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SPG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SPG_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}


	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
			window_flags |= ImGuiWindowFlags_NoBackground;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);


		// Dock Space
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
					OpenScene();
				}

				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		// Stats Panel Begin
		ImGui::Begin("Stats");
		
		std::string name = "None";
		if (m_HoveredEntity) {
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		}
		ImGui::Text("Hovered Entity: %s", name.c_str());

		// Render Info
		ImGui::Text("Frame Rate: %.1f (%.1fms)", 1.0f / m_FrameTime, m_FrameTime * 1000);

		// Renderer2D Stats
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
		// Stats Panel End

		// Setting Panel Begin
		ImGui::Begin("Settings");
		ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
		if (ImGui::Checkbox("Wireframe Mode", &m_WireframeMode)) {
			RenderCommand::SetWireframeMode(m_WireframeMode);
		};
		if (ImGui::Checkbox("VSync", &m_VSync)) {
			Application::Get().GetWindow().SetVSync(m_VSync);
		};
		if (ImGui::Button("Reload Shaders"))
		{
			// For debug shaders
			AssetManager::LoadShaders();
			AssetManager::LoadMeshes();
			// OpenScene(m_EditorScenePath);
		}
		ImGui::End();
		// Setting Panel End

		// Viewport window Begin
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		// Get Viewport Bounds
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		if (!ImGui::IsAnyItemActive())
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		else
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);


		// Draw Viewport Through ImGui
		ImVec2 viewportWindowSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportWindowSize)) {
			m_Framebuffer->Resize((uint32_t)viewportWindowSize.x, (uint32_t)viewportWindowSize.y);
			m_ViewportSize = { viewportWindowSize.x, viewportWindowSize.y };
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
		uint32_t textureID = m_Framebuffer->GetColorAttachmentTextureID(0);
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Drag and Drop to Viewport
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path filePath = std::filesystem::path(path);
				std::string extension = filePath.extension().string();
				if (extension == ".scene") {
					OpenScene(path);
				}
				else if (extension == ".png" || extension == ".jpg") {
					Ref<Texture2D> texture = Texture2D::Create(filePath.string());
					if (texture->IsLoaded()) {
						if (m_HoveredEntity && m_HoveredEntity.CheckComponent<SpriteRendererComponent>()) {
							m_HoveredEntity.GetComponent<SpriteRendererComponent>().Texture = texture;
						}
					}
					else {
						SPG_CORE_WARN("Failed to load texture from path: {0}", filePath.string());
					}
				}
				else {
					SPG_CORE_WARN("Unsupported file format: {0}", extension);
				}
				
			}
			ImGui::EndDragDropTarget();
		}

		// ImGuizmo
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);
			
			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity Transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) 
				snapValue = 45.0f; // Snap to 45 degrees for rotation
			float snapValues[3] = { snapValue, snapValue, snapValue };

			if (selectedEntity.HasParent()) {
				glm::mat4 parentTransform = m_ActiveScene->GetTransformRelatedToParents(m_ActiveScene->GetEntityByUUID(selectedEntity.GetParent()));
				glm::mat4 overallTransform = parentTransform * transform;
				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(overallTransform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing()) {
					glm::vec3 translation, rotation, scale;

					glm::mat4 newLoaclTransform = glm::inverse(parentTransform) * overallTransform;

					Math::DecomposeTransform(newLoaclTransform, translation, rotation, scale);
					tc.Translation = translation;
					glm::vec3 deltaRotation = glm::degrees(rotation) - tc.Rotation;
					tc.Rotation += deltaRotation;

					tc.Scale = scale;
				}
			}
			else {
				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing()) {
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);
					tc.Translation = translation;

					// to avoid gimbal lock
					// but why?
					glm::vec3 deltaRotation = glm::degrees(rotation) - tc.Rotation;
					tc.Rotation += deltaRotation;

					tc.Scale = scale;
				}
			}

		}
		
		ImGui::End();
		ImGui::PopStyleVar();
		// Viewport window End

		// Shadow Viewport Begin
		ImGui::Begin("Shadow Map");
		ImVec2 ShadowMapWindowSize = ImGui::GetContentRegionAvail();
		const Ref<Texture2D>& shadowMap =  m_ActiveScene->GetEnvironment();
		if (shadowMap != nullptr)
		{
			uint32_t ShadowMapID = shadowMap->GetTextureID();
			ImGui::Image((void*)ShadowMapID, ShadowMapWindowSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}
		ImGui::End();
		// Shadow Viewport End
		
		UI_Toolbar();

		ImGui::End();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0) return false;

		bool controlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shiftPressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			
			case Key::N:
			{
				if (controlPressed) {
					NewScene();
				}
				break;
			}
			case Key::O:
			{
				if (controlPressed) {
					OpenScene();
				}
				break;
			}
			case Key::S:
			{
				if (controlPressed) {
					if (shiftPressed)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			}

			case Key::D:
			{
				if (controlPressed) {
					OnDulicateEntity();
				}
				break;
			}

			// Gizmos Type
			if (m_SceneState == SceneState::Edit) {
				case Key::Q:
				{
					m_GizmoType = -1;
					break;
				}
				case Key::W:
				{
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					break;
				}
				case Key::E:
				{
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					break;
				}
				case Key::R:
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
					break;
				}
			}
			
				

			break;
		}

		
		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft) {
			// TODO: abstract this condition -- when you can select entities
			if (m_HoveredEntity && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt)) {
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty()) OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit) 
			OnSceneStop();

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string())) {
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
			m_EditorScene->SetEnvironment(m_ActiveScene->GetEnvironment());
			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}

		// temporary
		auto view = m_EditorScene->GetAllEntitiesWith<CameraComponent>();
		for (auto e : view) {
			Entity entity = { e, m_EditorScene.get() };
			entity.AddComponent<NativeScriptComponent>().Bind();
		}
	}

	void EditorLayer::SaveScene()
	{
		if (m_EditorScenePath.empty()) {
			SaveSceneAs();
		}
		else {
			SerializeScene(m_EditorScene, m_EditorScenePath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty()) {
			SerializeScene(m_EditorScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& filepath)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(filepath.string());
	
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;
		m_GizmoType = -1; // turn off gizmo
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDulicateEntity()
	{
		if (m_SceneState != SceneState::Edit) return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity) {
			m_EditorScene->DuplicateEntity(selectedEntity);
		}
	}

	void EditorLayer::OnOverlayRender()
	{
		
		if (m_SceneState == SceneState::Play) {
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else {
			Renderer2D::BeginScene(m_EditorCamera);
		}
		
		if (m_ShowPhysicsColliders) {
			// Box Collider
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entityHandle : view) {
					Entity entity = { entityHandle, m_ActiveScene.get() };
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entityHandle);
					glm::vec3 translation = glm::vec3(0.0f);
					glm::vec3 rotation = glm::vec3(0.0f);
					glm::vec3 scale = glm::vec3(1.0f);

					if (entity.HasParent()) {
						glm::mat4 transform = m_ActiveScene->GetTransformRelatedToParents(entity);
						Math::DecomposeTransform(transform, translation, rotation, scale);
					}
					else {
						translation = tc.Translation;
						rotation = glm::radians(tc.Rotation);
						scale = tc.Scale;
					}
					
					scale = scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
						* glm::scale(glm::mat4(1.0f), scale);
					Renderer2D::DrawRect(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1));
				}
			}

			// Circle Collider
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entityHandle : view) {
					Entity entity = { entityHandle, m_ActiveScene.get() };
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entityHandle);

					glm::vec3 translation = glm::vec3(0.0f);
					glm::vec3 rotation = glm::vec3(0.0f);
					glm::vec3 scale = glm::vec3(1.0f);

					if (entity.HasParent()) {
						glm::mat4 transform = m_ActiveScene->GetTransformRelatedToParents(entity);
						Math::DecomposeTransform(transform, translation, rotation, scale);
					}
					else {
						translation = tc.Translation;
						rotation = glm::radians(tc.Rotation);
						scale = tc.Scale;
					}

					translation += glm::vec3(cc2d.Offset, 0.001f);
					scale = scale * glm::vec3(cc2d.Radius * 2.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);
					Renderer2D::DrawCircle(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1), 0.05f);
				}
			}
		}	

		Renderer2D::EndScene();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 2 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;

		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		
		ImGui::SetCursorPosX(size * 0.5f);
		if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), { size, size }, { 0, 0 }, { 1, 1 }, 0)) {
			if (m_SceneState == SceneState::Edit) {
				OnScenePlay();
			}
			else if (m_SceneState == SceneState::Play) {
				OnSceneStop();
			}
		}
		
		ImGui::End();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
	}

}