#include "TransformSystem.h"
#include "../components.h"
#include "../world.h"
#include <glm/vec3.hpp>


TransformSystem::TransformSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void TransformSystem::init(){

}

void TransformSystem::update(float deltaTime){
    vector<int> toUpdate;
    TransformComponent parentTC = mWorld->getComponent<TransformComponent>(0);
    int currParent = 0;
    toUpdate.insert(toUpdate.end(), parentTC.childEntities.begin(), parentTC.childEntities.end());
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
    }
}