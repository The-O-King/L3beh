#ifndef CUSTOMCOMPONENT_HPP
#define CUSTOMCOMPONENT_HPP

#include <string>
#include "core/glm/vec3.hpp"
using namespace std;

struct RenderComponent{
	string modelFileName;
	string textureName;

	unsigned int vertex_vbo = 0;
	unsigned int texCoord_vbo = 0;
	unsigned int normal_vbo = 0;
    unsigned int numVert = 0;
	unsigned int texture = 0;

	glm::vec3 diffuse = {0.0f, 0.0f, 0.0f};
	glm::vec3 specular = {0.0f, 0.0f, 0.0f};
	float shininess = 0;
};

struct PlayerMovementComponent{
	bool active;
};

enum PhysicsType{
	STATIC = 0,
	KINEMATIC = 1,
	DYNAMIC = 2
};

struct PhysicsComponent{
	int type = 2;
	float gravityScale = 1;
    glm::vec3 sumForces = {0.0f, 0.0f, 0.0f};
	glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
	glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};
	float mass = 1;
	float invMass = 1;
	float restitutionCoefficient = 0;
	float friction = 0;

    glm::vec3 sumTorques = {0.0f, 0.0f, 0.0f};
	glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};
	glm::vec3 angularAcceleration = {0.0f, 0.0f, 0.0f};
};

enum ColliderType{
	BOX = 0,
	SPHERE = 1
};

struct ColliderComponent{
	bool isTrigger;
	ColliderType type;
	set<int> collisionEnter;
	set<int> collisionStay;
	set<int> collisionExit;
};

struct BoxColliderComponent : ColliderComponent {
	BoxColliderComponent(){
		type = ColliderType::BOX;
	}
	glm::vec3 boxMin = {0.0f, 0.0f, 0.0f};
	glm::vec3 boxMax = {0.0f, 0.0f, 0.0f};
};

struct SphereColliderComponent : ColliderComponent {
	SphereColliderComponent(){
		type = ColliderType::SPHERE;
	}
	float radius;
};

struct CameraComponent{
	bool isActive;
	float fov;
};

struct PointLightComponent{
	float intensity = 0.0f;
	glm::vec3 color = {0.0f, 0.0f, 0.0f};
};

#endif