#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/input.h"

PlayerMovementSystem::PlayerMovementSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformComponent>()] = 1;
    main[type_id<PlayerMovementComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void PlayerMovementSystem::init(){

}

void PlayerMovementSystem::update(float deltaTime){
    for (int e : entities){
        PlayerMovementComponent pc = mWorld->getComponent<PlayerMovementComponent>(e);
        if (pc.active){
            TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);

            if (Input::getKey(GLFW_KEY_W)){
                tc.position += glm::vec3(0, 0, 3) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_S)){
                tc.position -= glm::vec3(0, 0, 3) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_UP)){
                tc.position += glm::vec3(0, 3, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_DOWN)){
                tc.position -= glm::vec3(0, 3, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_LEFT)){
                tc.position -= glm::vec3(3, 0, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_RIGHT)){
                tc.position += glm::vec3(3, 0, 0) * (float)deltaTime;
            } 
            // if (Input::getKeyDown(GLFW_KEY_E)){
            //     phys.useGravity = !phys.useGravity;
            //     phys.isKinematic = !phys.useGravity; 
            // }
        }
    }
}