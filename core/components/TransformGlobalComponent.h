#ifndef TRANSFORMGLOBALCOMPONENT_H
#define TRANSFORMGLOBALCOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include <core/systems/TransformSystem.h>

struct TransformGlobalComponent : public Component{
	const glm::vec3& getPosition() const;
	const glm::quat& getRotationQuat() const;
	glm::mat3 getRotationMat() const;
	const glm::vec3& getScale() const;

	glm::mat4 getTransformation() const;
	glm::mat4 getOrientation() const;

private:
	friend class TransformSystem;
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
};

#endif