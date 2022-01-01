#ifndef TRANSFORMGLOBALCOMPONENT_H
#define TRANSFORMGLOBALCOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"

struct TransformGlobalComponent : Serialization<TransformGlobalComponent> {
	const glm::vec3& getPosition() const;
	const glm::quat& getRotationQuat() const;
	glm::mat3 getRotationMat() const;
	const glm::vec3& getScale() const;

	glm::mat4 getTransformation() const;
	glm::mat4 getOrientation() const;

	static std::string getNameImpl() { return "TransformGlobal"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformGlobalComponent, position, rotation, scale)

private:
	friend class TransformSystem;
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
};

#endif