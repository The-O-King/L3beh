#ifndef CUSTOMCOMPONENT_HPP
#define CUSTOMCOMPONENT_HPP

#include <string>
#include "core/glm/vec3.hpp"
using namespace std;

struct TransformComponent{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {0.0f, 0.0f, 0.0f};
};

struct RenderComponent{
	bool initialized = false;
	string vertShaderFileName;
	string fragShaderFileName;
	string modelFileName;

	unsigned int vbo = 0;
    unsigned int numVert = 0;
	unsigned int program = 0;
};

struct PlayerMovementComponent{
	bool active;
};

struct PhysicsComponent{
    glm::vec3 sumForces;
    glm::vec3 sumTorques;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;
	float drag;
};

#endif