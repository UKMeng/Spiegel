#pragma once

#include "Spiegel/Renderer/Camera.h"

namespace spg {
	enum class ProjectionType
	{
		Perspective = 0,
		Orthographic = 1
	};

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetProjectionType(ProjectionType type);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
	private:
		void RecalculateProjection();
	private:
		float m_AspectRatio = 16.0f / 9.0f;
		float m_OrthographicSize = 10.0f, m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
		float m_PerspectiveFOV = 45.0f, m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
	};
}