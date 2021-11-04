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
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
	
	// Global Transform to parent
	glm::vec3 worldPosition = {0.0f, 0.0f, 0.0f};
	glm::quat worldRotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 worldScale = {0.0f, 0.0f, 0.0f};

	int parentEntity = -1;
	set<int> childEntities;
};

inline glm::mat3 getRotation(TransformComponent& tc){
	return glm::mat3(tc.worldRotation);	
}

inline glm::mat4 getTransformation(TransformComponent& tc){
	glm::mat4 model = glm::translate(glm::mat4(1.0), tc.worldPosition);
	glm::mat4 scale = glm::scale(glm::mat4(1.0), tc.worldScale);
	return model * glm::mat4(tc.worldRotation) * scale;
}

inline glm::mat4 getOrientation(TransformComponent& tc){
	glm::mat4 model = glm::translate(glm::mat4(1.0), tc.worldPosition);
	return model * glm::mat4(tc.worldRotation);
}

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

struct PhysicsComponent{
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

struct ColliderComponent{
	bool isTrigger;
	ColliderType type;
	glm::vec3 offset = {0.0f, 0.0f, 0.0f};
	set<int> collisionEnter;
	set<int> collisionStay;
	set<int> collisionExit;
};

struct BoxColliderComponent {
	glm::vec3 halfSize = {0.0f, 0.0f, 0.0f};
};

struct SphereColliderComponent {
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

struct DirectionalLightComponent{
	glm::vec3 direction = {0.0f, 0.0f, 0.0f};
	glm::vec3 color = {0.0f, 0.0f, 0.0f};
};

#endif