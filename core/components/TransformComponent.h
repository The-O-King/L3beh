#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <core/world.h>
#include "Component.h"

struct TransformComponent : public Component{

	void translate(const glm::vec3 &position, World* mWorld);

	void setPosition(const glm::vec3 &position, World* mWorld);
	void setRotation(const glm::vec3 &rotation, World* mWorld);
	void setRotation(const glm::quat &rotation, World* mWorld);
	void setScale(const glm::vec3 &scale, World* mWorld);
	void setParent(int parent, World* mWorld);

	glm::vec3 getLocalPosition();
	glm::quat getLocalRotationQuat();
	glm::mat3 getLocalRotationMat();
	glm::vec3 getLocalScale();
	glm::mat4 getLocalTransformation();
	glm::mat4 getLocalOrientation();	

	glm::vec3 getWorldPosition(World* mWorld);
	glm::quat getWorldRotationQuat(World* mWorld);
	glm::mat3 getWorldRotationMat(World* mWorld);
	glm::vec3 getWorldScale(World* mWorld);
	glm::mat4 getWorldTransformation(World* mWorld);
	glm::mat4 getWorldOrientation(World* mWorld);

	int getParentEntity() const;
	std::set<int> getChildEntities() const;
	void removeChildEntity(int entity);

private:
	// Local Transform to parent
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
	
	// Global Transform to parent
	glm::vec3 worldPosition = {0.0f, 0.0f, 0.0f};
	glm::quat worldRotation = {1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 worldScale = {0.0f, 0.0f, 0.0f};

	int parentEntity = -1;
	std::set<int> childEntities;

	bool dirty = true;
	void updateDirty(World* mWorld);
	void cleanDirty(World* mWorld);
};

#endif