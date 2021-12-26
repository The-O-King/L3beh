#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"

struct TransformComponent : public Component{
	// Local Transform to parent
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};

	glm::mat4 getTransformation() const;
	glm::mat4 getOrientation() const;
	glm::mat3 getRotationMat() const;
};

#endif