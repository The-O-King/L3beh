#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/input.h"

PlayerMovementSystem::PlayerMovementSystem(World* w){
    mWorld = w;
    neededComponents[type_id<PhysicsComponent>()] = 1;
    neededComponents[type_id<PlayerMovementComponent>()] = 1;
}

void PlayerMovementSystem::init(){

}

void PlayerMovementSystem::update(float deltaTime){
    for (int e : entities){
        PlayerMovementComponent pc = mWorld->getComponent<PlayerMovementComponent>(e);
        if (pc.active){
            PhysicsComponent& phys = mWorld->getComponent<PhysicsComponent>(e);

            if (Input::getKey(GLFW_KEY_W)){
                phys.sumForces += glm::vec3(0, 0, 50) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_S)){
                phys.sumForces -= glm::vec3(0, 0, 50) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_UP)){
                phys.sumForces += glm::vec3(0, 50, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_DOWN)){
                phys.sumForces -= glm::vec3(0, 50, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_E)){
                phys.useGravity = !phys.useGravity;
            }
        }
    }
}