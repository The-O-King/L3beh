#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H


#include <string>
#include <glm/glm.hpp>
#include "Component.h"

struct RenderComponent : public Component{
	std::string modelFileName;
	std::string textureName;

	glm::vec3 diffuse = {0.0f, 0.0f, 0.0f};
	glm::vec3 specular = {0.0f, 0.0f, 0.0f};
	float shininess = 0;
};

#endif