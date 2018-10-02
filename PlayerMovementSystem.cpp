#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include "core/world.h"

PlayerMovementSystem::PlayerMovementSystem(World* w){
    mWorld = w;
    neededComponents[type_id<TransformComponent>()] = 1;
    neededComponents[type_id<PlayerMovementComponent>()] = 1;
}

void PlayerMovementSystem::init(){

}

void PlayerMovementSystem::update(double deltaTime){
    for (int e : entities){
        PlayerMovementComponent pc = mWorld->getComponent<PlayerMovementComponent>(e);
        if (pc.active){
            TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);

            if (mWorld->getKey(GLFW_KEY_W)){
                tc.position.z += deltaTime * 1;
            }
            if (mWorld->getKey(GLFW_KEY_S)){
                tc.position.z -= deltaTime * 1;
            }
            if (mWorld->getKey(GLFW_KEY_A)){
                tc.position.y -= deltaTime * 1;
            }
            if (mWorld->getKey(GLFW_KEY_D)){
                tc.position.y += deltaTime * 1;
            }
        }
    }
}