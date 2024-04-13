#pragma once

#include "Camera.h"
#include "Spiegel/Core/Timestep.h"
#include "Spiegel/Events/Event.h"
#include "Spiegel/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace spg {

	class EditorCamera :public Camera
	{
	public:
		EditorCamera() = default;

		/// @brief simple EditorCamera constructor
		/// @param fov use degrees
		/// @param aspectRatio 
		/// @param nearClip 
		/// @param farClip 
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		float GetDistance() const { return m_Distance; }
		void SetDistance(float distance) { m_Distance = distance; }

		void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;
		
		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		
		

	private:
		void UpdateProjection();
		void UpdateView();
		glm::vec3 CalculatePosition() const;

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
		float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
		float m_Distance = 10.0f;

		// Pitch Rotate on the Cross Product of the Right Vector and the Up Vector
		// Yaw Rotate on the Up Vector
		// Refererence: https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_6
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
	};
}