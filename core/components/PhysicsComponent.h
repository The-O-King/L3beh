#ifndef PHYSICSCOMPONENT_H
#define PHYISCSCOMPONENT_H

#include <glm/glm.hpp>
#include <set>
#include "Component.h"

struct PhysicsComponent : public Component{
	float gravityScale = 1;
    glm::vec3 sumForces = {0.0f, 0.0f, 0.0f};
	glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
	glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};
	float mass = 1;
	float invMass = 1;
	glm::mat3 invInertia = glm::mat3(1);
	float restitutionCoefficient = 0;
	float friction = 0;

    glm::vec3 sumTorques = {0.0f, 0.0f, 0.0f};
	glm::vec3 angularVelocity = {0.0f, 0.0f, 0.0f};
	glm::vec3 angularAcceleration = {0.0f, 0.0f, 0.0f};

	glm::vec3 lockRotation = {1.0f, 1.0f, 1.0f};
	glm::vec3 lockPosition = {1.0f, 1.0f, 1.0f};
};

enum ColliderType{
	BOX = 0,
	SPHERE = 1
};

struct ColliderComponent : public Component {
	bool isTrigger;
	ColliderType type;
	glm::vec3 offset = {0.0f, 0.0f, 0.0f};
	std::set<int> collisionEnter;
	std::set<int> collisionStay;
	std::set<int> collisionExit;
};

struct BoxColliderComponent : public Component {
	glm::vec3 halfSize = {0.0f, 0.0f, 0.0f};
};

struct SphereColliderComponent : public Component {
	float radius;
};

#endif