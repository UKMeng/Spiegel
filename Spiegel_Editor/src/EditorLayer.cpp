#include "EditorLayer.h"

#include "Spiegel/Scene/SceneSerializer.h"
#include "Spiegel/Utils/PlatformUtils.h"
#include "Spiegel/Math/Math.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace spg {
	static const uint32_t s_MapWidth = 12;
	static const char* s_MapTiles =
		"wwwwwwwwwwww"
		"wwwDDDDDDwww"
		"wDDDDDDDDDww"
		"wwwDDDDDDwww"
		"wwwwwwwwwwww";

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		SPG_PROFILE_FUNCTION();

		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_SpriteSheet = Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

		m_TextureStairs = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
		m_TextureTree = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1, 2 });

		m_MapWidth = s_MapWidth;
		m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
		m_TextureMap['D'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 }); // grass
		m_TextureMap['w'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 }); // water

		// m_CameraController.SetZoomLevel(5.0f);

		// Framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f); // 1.778f is the aspect ratio of 16:9

#if 0
		auto blueSquare = m_ActiveScene->CreateEntity("Blue Square");
		blueSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f });

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();
	
		m_SecondCameraEntity = m_ActiveScene->CreateEntity("Second Camera");
		auto& cc = m_SecondCameraEntity.AddComponent<CameraComponent>();
		cc.Primary = false;

		
		// Native Script Example
		class CameraController : public ScriptableEntity
		{
		public:
			/*void OnCreate()
			{
				
			}

			void OnDestroy()
			{
			}*/

			void OnUpdate(Timestep ts)
			{
				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;

				if (Input::IsKeyPressed(SPG_KEY_A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(SPG_KEY_D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(SPG_KEY_W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(SPG_KEY_S))
					translation.y -= speed * ts;
			}
		};

		m_SecondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		SPG_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		SPG_PROFILE_FUNCTION();

		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Camera
		if (m_ViewportFocused) {
			m_CameraController.OnUpdate(ts);
		}

		m_EditorCamera.OnUpdate(ts);
		

		// Renderer
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
		// Clear Attachment to -1
		m_Framebuffer->ClearAttachment(1, (void*)-1);

		// Scene
		// m_ActiveScene->OnUpdateRuntime(ts);
		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

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
		
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if(m_ViewportFocused) m_CameraController.OnEvent(e);

		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(SPG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SPG_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}


	void EditorLayer::OnImGuiRender()
	{
		SPG_PROFILE_FUNCTION();

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
		style.WindowMinSize.x = 380.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = 0.0f;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
					OpenScene();
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

		// Settings Window Begin
		ImGui::Begin("Stats");
		
		std::string name = "None";
		if (m_HoveredEntity) {
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		}
		ImGui::Text("Hovered Entity: %s", name.c_str());

		// Renderer2D Stats
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
		// Settings Window End

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

		ImVec2 viewportWindowSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportWindowSize)) {
			m_Framebuffer->Resize((uint32_t)viewportWindowSize.x, (uint32_t)viewportWindowSize.y);
			m_ViewportSize = { viewportWindowSize.x, viewportWindowSize.y };
			m_CameraController.OnResize(viewportWindowSize.x, viewportWindowSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// ImGuizmo
		// TODO: Mouse Picking
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Get Primary Camera Runtime
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>();
			// const glm::mat4& cameraProjection = camera.Camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			
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
		
		ImGui::End();
		ImGui::PopStyleVar();
		// Viewport window End

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
				if (controlPressed && shiftPressed) {
					SaveSceneAs();
				}
				break;
			}
			// Gizmos Type
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
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Scene (*.yaml)\0*.yaml\0");
		if (!filepath.empty()) {
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Scene (*.yaml)\0*.yaml\0");
		if (!filepath.empty()) {
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}