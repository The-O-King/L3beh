#include "TransformComponent.h"
#include <vector>

glm::mat4 TransformComponent::getTransformation() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale);
	return model * glm::mat4_cast(rotation) * scaling;
}

glm::mat4 TransformComponent::getOrientation() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	return model * glm::mat4_cast(rotation);
}

glm::mat3 TransformComponent::getRotationMat() const {
    return glm::mat3_cast(rotation);
}