#ifndef PHYSICSCOMPONENT_H
#define PHYISCSCOMPONENT_H

#include <glm/glm.hpp>
#include <set>
#include "Component.h"

struct PhysicsComponent : Serialization<PhysicsComponent> {
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

	static std::string getNameImpl() { return "Physics"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(PhysicsComponent, gravityScale, mass, invMass, restitutionCoefficient, friction, lockRotation, lockPosition)
};

enum ColliderType{
	BOX = 0,
	SPHERE = 1
};

struct ColliderComponent : Serialization<ColliderComponent> {
	bool isTrigger;
	ColliderType type;
	glm::vec3 offset = {0.0f, 0.0f, 0.0f};
	std::set<int> collisionEnter;
	std::set<int> collisionStay;
	std::set<int> collisionExit;

	static std::string getNameImpl() { return "Collider"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ColliderComponent, isTrigger, type, offset)
};

struct BoxColliderComponent : Serialization<BoxColliderComponent> {
	glm::vec3 halfSize = {0.0f, 0.0f, 0.0f};

	static std::string getNameImpl() { return "BoxCollider"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BoxColliderComponent, halfSize)
};

struct SphereColliderComponent : Serialization<SphereColliderComponent> {
	float radius;

	static std::string getNameImpl() { return "SphereCollider"; }

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SphereColliderComponent, radius)
};

#endif