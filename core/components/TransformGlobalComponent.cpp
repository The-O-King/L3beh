#include "TransformGlobalComponent.h"

const glm::vec3& TransformGlobalComponent::getPosition() const {
	return position;
}

const glm::quat& TransformGlobalComponent::getRotationQuat() const {
	return rotation;
}

glm::mat3 TransformGlobalComponent::getRotationMat() const {
    return glm::mat3_cast(rotation);
}

const glm::vec3& TransformGlobalComponent::getScale() const {
	return scale;
}

glm::mat4 TransformGlobalComponent::getTransformation() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale);
	return model * glm::mat4_cast(rotation) * scaling;
}

glm::mat4 TransformGlobalComponent::getOrientation() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	return model * glm::mat4_cast(rotation);
}