#pragma once

#include "Spiegel.h"
#include "Panels/SceneHierarchyPanel.h"

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
		OrthographicCameraController m_CameraController;

		Ref<Texture2D> m_CheckerboardTexture;

		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs, m_TextureTree;
		Ref<Scene> m_ActiveScene;

		size_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_TextureMap;

		Ref<Framebuffer> m_Framebuffer;

		glm::vec2 m_ViewportSize;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		Entity m_CameraEntity;
		Entity m_SecondCameraEntity;

		bool m_PrimaryCamera = true;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
