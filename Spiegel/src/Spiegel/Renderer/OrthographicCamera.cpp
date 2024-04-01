#include "spgpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace spg {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		SPG_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		SPG_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		SPG_PROFILE_FUNCTION();

		// glm::translate(glm::mat4(1.0f), m_Position) creates a translation matrix（平移矩阵）
		// glm::rotate() creates a rotation matrix（旋转矩阵）
		// glm::radians(m_Rotation) converts the rotation from degrees to radians (旋转角度）
		// glm::vec3(0, 0, 1) is the axis of rotation（z轴）
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
	
		// 相机的位移与实际物体的相对位移是相反的，所以取逆
		m_ViewMatrix = glm::inverse(transform);

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}