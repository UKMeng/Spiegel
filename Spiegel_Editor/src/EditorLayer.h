#pragma once

#include "Spiegel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

namespace spg {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& filepath);

		void OnScenePlay();
		void OnSceneStop();

		void OnDulicateEntity();

		void OnOverlayRender();

		// UI Panels
		void UI_Toolbar();
	private:
		// Scene
		enum class SceneState {
			Edit = 0, Play = 1
		};

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		SceneState m_SceneState = SceneState::Edit;
		std::filesystem::path m_EditorScenePath;

		// Texture
		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_IconPlay, m_IconStop;
		

		size_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;

		Ref<Framebuffer> m_Framebuffer;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		Entity m_CameraEntity;
		Entity m_SecondCameraEntity;
		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		// Editor Camera
		EditorCamera m_EditorCamera;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Gizmos
		int m_GizmoType = -1;

		// Settings
		bool m_ShowPhysicsColliders = false;
		bool m_WireframeMode = false;
		bool m_VSync = true;

		// Info
		float m_FrameTime = 0.0f;
		float m_TempFrameTimeForRefresh = 0.0f;

		// Render Pipeline
		Ref<Pipeline> m_Pipeline;

		// RenderPass temporary
		Ref<RenderPass> m_ShadowPass;
	};
}
