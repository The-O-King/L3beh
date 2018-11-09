#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>
#include "glm/vec3.hpp"
#include <set>
using namespace std;

struct TransformComponent{
	// Local Transform to parent
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {0.0f, 0.0f, 0.0f};
	
	// Global Transform to parent
	glm::vec3 worldPosition = {0.0f, 0.0f, 0.0f};
	glm::vec3 worldRotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 worldScale = {0.0f, 0.0f, 0.0f};

	int parentEntity = -1;
	set<int> childEntities;
};

#endif