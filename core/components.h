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
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {0.0f, 0.0f, 0.0f};
	
	// Global Transform to parent
	glm::vec3 worldPosition = {0.0f, 0.0f, 0.0f};
	glm::vec3 worldRotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 worldScale = {0.0f, 0.0f, 0.0f};

	int parentEntity = -1;
	set<int> childEntities;

	glm::mat3 getRotation(){
		glm::quat test = glm::quat(glm::vec3(glm::radians(worldRotation.x), glm::radians(worldRotation.y), glm::radians(worldRotation.z)));
		return glm::mat3(test);

		glm::mat4 model = glm::rotate(glm::mat4(1), (float)(glm::radians(worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, (float)(glm::radians(worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
		model = glm::rotate(model, (float)(glm::radians(worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
		return glm::mat3(model);	
	}
	glm::mat4 getTransformation(){
        glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
        glm::mat4 scale = glm::scale(glm::mat4(1.0), worldScale);
		glm::quat test = glm::quat(glm::vec3(glm::radians(worldRotation.x), glm::radians(worldRotation.y), glm::radians(worldRotation.z)));
		return model * glm::mat4(test) * scale;

		model = glm::scale(model, worldScale);
        model = glm::rotate(model, (float)(glm::radians(worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
        model = glm::rotate(model, (float)(glm::radians(worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
		return model;
	}
	glm::mat4 getOrientation(){
        glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
		glm::quat test = glm::quat(glm::vec3(glm::radians(worldRotation.x), glm::radians(worldRotation.y), glm::radians(worldRotation.z)));
		glm::mat4 hello(test);		
		return model * hello;

		model = glm::rotate(model, (float)(glm::radians(worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, (float)(glm::radians(worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
        model = glm::rotate(model, (float)(glm::radians(worldRotation.x)), glm::vec3(1.0, 0.0, 0.0));
		return model;
	}
};

#endif