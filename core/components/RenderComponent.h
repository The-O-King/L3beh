#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H


#include <string>
#include <glm/glm.hpp>
#include "Component.h"

struct RenderComponent : Serialization<RenderComponent> {
	std::string modelFileName;
	std::string textureName;

	glm::vec3 diffuse = {0.0f, 0.0f, 0.0f};
	glm::vec3 specular = {0.0f, 0.0f, 0.0f};
	float shininess = 0;

	static std::string getNameImpl() { return "Render"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderComponent, modelFileName, textureName, diffuse, specular, shininess)
};

#endif