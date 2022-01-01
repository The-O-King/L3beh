#ifndef LIGHTCOMPONENTS_H
#define LIGHTCOMPONENTS_H

#include <glm/glm.hpp>
#include "Component.h"

struct PointLightComponent : Serialization<PointLightComponent> {
	float intensity = 0.0f;
	glm::vec3 color = {0.0f, 0.0f, 0.0f};

	static std::string getNameImpl() { return "PointLightComponent"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PointLightComponent, intensity, color)
};

struct DirectionalLightComponent : Serialization<DirectionalLightComponent> {
	glm::vec3 direction = {0.0f, 0.0f, 0.0f};
	glm::vec3 color = {0.0f, 0.0f, 0.0f};

	static std::string getNameImpl() { return "DirectionalLightComponent"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(DirectionalLightComponent, direction, color)
};

#endif