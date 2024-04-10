#pragma once

#include "Spiegel/Renderer/Camera.h"

namespace spg {
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }

		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float verticalFOV) { m_PerspectiveFOV = verticalFOV; RecalculateProjection(); }
		
		float GetNearClip() const { return m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveNear : m_OrthographicNear; }
		void SetNearClip(float nearCilp) { m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveNear = nearCilp : m_OrthographicNear = nearCilp; RecalculateProjection(); }

		float GetFarClip() const { return m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveFar : m_OrthographicFar; }
		void SetFarClip(float farClip) { m_ProjectionType == ProjectionType::Perspective ? m_PerspectiveFar = farClip : m_OrthographicFar = farClip; RecalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type);
	private:
		void RecalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_AspectRatio = 16.0f / 9.0f;
		float m_OrthographicSize = 10.0f, m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;
		// fov radians?
		float m_PerspectiveFOV = 45.0f, m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;
	};
}