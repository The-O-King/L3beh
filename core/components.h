#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <set>
using namespace std;

struct TransformComponent{
	// Local Transform to parent
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {0.0f, 0.0f, 0.0f};
	
	// Global Transform to parent
	glm::vec3 worldPosition = {0.0f, 0.0f, 0.0f};
	glm::quat worldRotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 worldScale = {0.0f, 0.0f, 0.0f};

	int parentEntity = -1;
	set<int> childEntities;

	glm::mat3 getRotation(){
		return glm::mat3(worldRotation);	
	}
	glm::mat4 getTransformation(){
        glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
        glm::mat4 scale = glm::scale(glm::mat4(1.0), worldScale);
		return model * glm::mat4(worldRotation) * scale;
	}
	glm::mat4 getOrientation(){
        glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
		return model * glm::mat4(worldRotation);
	}
};

#endif