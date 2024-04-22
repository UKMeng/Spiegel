#pragma once

#include <glm/glm.hpp>

namespace spg::Math {
	
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	glm::vec3 DecomposeTransformToGetRotation(const glm::mat4& transform);
}