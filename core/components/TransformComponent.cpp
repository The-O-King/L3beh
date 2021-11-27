#include "TransformComponent.h"
#include <vector>

void TransformComponent::updateDirty(World* mWorld) {
    if (dirty)
        return;
    
    std::vector<int> toUpdate;
    toUpdate.push_back(owner);
    while (!toUpdate.empty()){
        int currUpdate = toUpdate.back(); toUpdate.pop_back();
        TransformComponent& curr = mWorld->getComponent<TransformComponent>(currUpdate);
        toUpdate.insert(toUpdate.end(), curr.childEntities.begin(), curr.childEntities.end());
        curr.dirty = true;
    }
}

void TransformComponent::cleanDirty(World *mWorld){
    if (!dirty)
        return;

    std::vector<int> toUpdate;
    toUpdate.push_back(owner);
    int currParent = parentEntity;
    TransformComponent parentTC = mWorld->getComponent<TransformComponent>(parentEntity);
    while (!toUpdate.empty()){
        int currUpdate = toUpdate.back(); toUpdate.pop_back();
        TransformComponent& curr = mWorld->getComponent<TransformComponent>(currUpdate);
        toUpdate.insert(toUpdate.end(), curr.childEntities.begin(), curr.childEntities.end());
        if (curr.parentEntity != currParent){
            parentTC = mWorld->getComponent<TransformComponent>(curr.parentEntity);
            currParent = curr.parentEntity;
        }
        curr.worldPosition = curr.position + parentTC.worldPosition;
        curr.worldRotation = curr.rotation * parentTC.worldRotation;
        curr.worldScale = curr.scale + parentTC.worldScale;
        curr.dirty = false;
    }
}

void TransformComponent::translate(const glm::vec3 &translation, World* mWorld) {
    updateDirty(mWorld);
    position += translation;
}

void TransformComponent::setPosition(const glm::vec3 &newPos, World* mWorld) {
    updateDirty(mWorld);
    position = newPos;
}

void TransformComponent::setRotation(const glm::vec3 &newRot, World* mWorld) {
    updateDirty(mWorld);
    rotation = newRot;
}

void TransformComponent::setRotation(const glm::quat &newRot, World* mWorld) {
    updateDirty(mWorld);
    rotation = newRot;
}

void TransformComponent::setScale(const glm::vec3 &newScale, World* mWorld) {
    updateDirty(mWorld);
    scale = newScale;
}

glm::vec3 TransformComponent::getLocalPosition(){
    return position;
}

glm::quat TransformComponent::getLocalRotationQuat(){
	return rotation;	
}

glm::mat3 TransformComponent::getLocalRotationMat(){
	return glm::mat3(rotation);	
}

glm::vec3 TransformComponent::getLocalScale(){
    return scale;
}

glm::mat4 TransformComponent::getLocalTransformation(){
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale);
	return model * glm::mat4(rotation) * scaling;
}

glm::mat4 TransformComponent::getLocalOrientation(){
	glm::mat4 model = glm::translate(glm::mat4(1.0), position);
	return model * glm::mat4(rotation);
}

glm::vec3 TransformComponent::getWorldPosition(World* mWorld){ 
    cleanDirty(mWorld);
    return worldPosition;
}

glm::quat TransformComponent::getWorldRotationQuat(World *mWorld){
    cleanDirty(mWorld);
	return worldRotation;	
}

glm::mat3 TransformComponent::getWorldRotationMat(World *mWorld){
    cleanDirty(mWorld);
	return glm::mat3(worldRotation);	
}

glm::vec3 TransformComponent::getWorldScale(World *mWorld){
    cleanDirty(mWorld);
    return worldScale;
}

glm::mat4 TransformComponent::getWorldTransformation(World *mWorld){
    cleanDirty(mWorld);
	glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
	glm::mat4 scale = glm::scale(glm::mat4(1.0), worldScale);
	return model * glm::mat4(worldRotation) * scale;
}

glm::mat4 TransformComponent::getWorldOrientation(World *mWorld){
    cleanDirty(mWorld);
	glm::mat4 model = glm::translate(glm::mat4(1.0), worldPosition);
	return model * glm::mat4(worldRotation);
}

int TransformComponent::getParentEntity() const {
    return parentEntity;
}

void TransformComponent::setParent(int newParent, World* mWorld) {
    updateDirty(mWorld);
    if (parentEntity != -1) {
        TransformComponent& parentTransform = mWorld->getComponent<TransformComponent>(parentEntity);
        parentTransform.removeChildEntity(owner);    
    }
    parentEntity = newParent;
    TransformComponent& parentTransform = mWorld->getComponent<TransformComponent>(newParent);
    parentTransform.childEntities.insert(owner);
}

void TransformComponent::removeChildEntity(int entity) {
    childEntities.erase(entity);
}

std::set<int> TransformComponent::getChildEntities() const{
    return childEntities;
}