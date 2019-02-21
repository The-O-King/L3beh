#include "ProjectileSystem.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/components.h"
#include "core/world.h"

ProjectileSystem::ProjectileSystem(World* w){
    mWorld = w;
    componentSignature need;
    need[type_id<ProjectileComponent>()] = 1;
    need[type_id<ColliderComponent>()] = 1;
    neededComponentSignatures.push_back(need);
}

void ProjectileSystem::init(){

}

void ProjectileSystem::update(float deltaTime){
    for (int e : entities){
        ColliderComponent& cc = mWorld->getComponent<ColliderComponent>(e);
        if (cc.collisionEnter.size() != 0){
            mWorld->destroyEntity(e);
        }
    } 
}