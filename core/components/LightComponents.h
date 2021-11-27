#ifndef LIGHTCOMPONENTS_H
#define LIGHTCOMPONENTS_H

#include <glm/glm.hpp>
#include "Component.h"

struct PointLightComponent : public Component {
	float intensity = 0.0f;
	glm::vec3 color = {0.0f, 0.0f, 0.0f};
};

struct DirectionalLightComponent : public Component {
	glm::vec3 direction = {0.0f, 0.0f, 0.0f};
	glm::vec3 color = {0.0f, 0.0f, 0.0f};
};

#endif