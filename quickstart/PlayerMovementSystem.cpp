#include "PlayerMovementSystem.h"

PlayerMovementSystem::PlayerMovementSystem(World* w){
    mWorld = w;
    componentSignature main;
    main[type_id<TransformComponent>()] = 1;
    main[type_id<PlayerMovementComponent>()] = 1;
    main[type_id<PhysicsComponent>()] = 1;
    neededComponentSignatures.push_back(main);
}

void PlayerMovementSystem::init(){

}

void PlayerMovementSystem::update(float deltaTime){
    for (int e : entities){
        PlayerMovementComponent& pc = mWorld->getComponent<PlayerMovementComponent>(e);
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        PhysicsComponent& phys = mWorld->getComponent<PhysicsComponent>(e);
        glm::vec3 forward = glm::vec3(0, 0, -1);  
        glm::vec3 left = glm::vec3(-1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);  
        float speed = 0;
        if (Input::getKey(GLFW_KEY_LEFT_SHIFT)){
            speed = pc.runSpeed;
            pc.isRunning = true; 
        }
        else{
            speed = pc.walkSpeed;
            pc.isRunning = false;
        }
        if (Input::getKey(GLFW_KEY_W)){      
            tc.position += forward * speed * deltaTime;
        }
        if (Input::getKey(GLFW_KEY_S)){
            tc.position -= forward * speed * deltaTime;
        }
        if (Input::getKey(GLFW_KEY_A)){
            tc.position += left * speed * deltaTime;
        }
        if (Input::getKey(GLFW_KEY_D)){
            tc.position -= left * speed * deltaTime;
        }
        if (tc.worldPosition.y < .1){
            if (Input::getKeyDown(GLFW_KEY_SPACE)){
                phys.sumForces += up * pc.jumpForce;
                pc.isJumping = true;
            }
            else{
                pc.isJumping = false;
            }
        }
    }
}