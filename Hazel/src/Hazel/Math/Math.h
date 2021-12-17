#pragma once

#include <glm/glm.hpp>
#include "Hazel/Scene/Components.h"

namespace Hazel::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
	bool DecomposeTransform(const glm::mat4& transform, Components::Transform& newTransform);
}
