#pragma once

#include "Spiegel/Renderer/OrthographicCamera.h"
#include "Spiegel/Core/Timestep.h"

#include "Spiegel/Events/ApplicationEvent.h"
#include "Spiegel/Events/MouseEvent.h"

namespace spg {

	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;
		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); }
		float GetZoomLevel() const { return m_ZoomLevel; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;

		// Initilzing Order is Important !!!
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;
		
		
		bool m_Rotation = false;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 45.0f;

	};
}