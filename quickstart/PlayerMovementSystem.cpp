#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <core/components/TransformComponent.h>
#include <core/components/PhysicsComponent.h>
#include <core/world.h>
#include <core/input.h>

std::string testEntity[] = { 
    "Entity PhysicsBall\n"
    "Physics 1 1 1 .5     0 0 0   0 0 0\n"
    "Transform 0    0 0 -10    0 0 0  1 1 1\n"
    "SphereCollider 0    0 0 0   1\n"
    "Render core/assets/sphere.obj core/assets/wood_albedo.jpg  1 1 1 1 1 1 32\n"
    "Projectile 1\n"
    "/Entity\n",
    
    "Entity PhysicsCube\n"
    "Physics 1 1 1 .5     0 0 0   0 0 0\n"
    "Transform 0    0 0 -10    0 0 0  1 1 1\n"
    "BoxCollider 0      0 0 0    .5 .5 .5\n"
    "Render core/assets/cube.obj core/assets/wood_albedo.jpg  1 1 1 1 1 1 32\n"
    "Projectile 0\n"
    "/Entity\n"
};

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

        glm::quat worldRot = tc.getWorldRotationQuat(mWorld);
        glm::vec3 lookDir = glm::normalize(worldRot * glm::vec3(0, 0, -1));
        glm::vec3 forward = glm::normalize((worldRot * glm::vec3(0, 0, -1)) * glm::vec3(1, 0, 1));  
        glm::vec3 left = glm::normalize((worldRot * glm::vec3(-1, 0, 0)) * glm::vec3(1, 0, 1));  
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
            tc.translate(forward * speed * deltaTime, mWorld);
        }
        if (Input::getKey(GLFW_KEY_S)){
            tc.translate(-forward * speed * deltaTime, mWorld);
        }
        if (Input::getKey(GLFW_KEY_A)){
            tc.translate(left * speed * deltaTime, mWorld);
        }
        if (Input::getKey(GLFW_KEY_D)){
            tc.translate(-left * speed * deltaTime, mWorld);
        }
        if (tc.getWorldPosition(mWorld).y < .1){
            if (Input::getKeyDown(GLFW_KEY_SPACE)){
                phys.sumForces += glm::vec3(0, 1, 0) * pc.jumpForce;
                pc.isJumping = true;
            }
            else{
                pc.isJumping = false;
            }
        }

        double xdelta, ydelta;
        Input::getMouseDelta(&xdelta, &ydelta);
        glm::quat yawRotation = glm::vec3(0, xdelta * deltaTime, 0);
        glm::quat pitchRotation = glm::vec3(ydelta * deltaTime, 0, 0);
        tc.setRotation(yawRotation * tc.getLocalRotationQuat() * pitchRotation, mWorld);
        pc.currProjectile = glm::clamp(pc.currProjectile + (int)Input::getScrollDelta(), 0, 1);            

        if (Input::getMouseButtonDown(GLFW_MOUSE_BUTTON_1)){
            int baby = mWorld->createEntity(testEntity[pc.currProjectile]);
            TransformComponent& temp2 = mWorld->getComponent<TransformComponent>(baby);
            temp2.setPosition(tc.getWorldPosition(mWorld) + (lookDir * 10.0f), mWorld);
            temp2.setRotation(tc.getWorldRotationQuat(mWorld), mWorld);
            PhysicsComponent& temp = mWorld->getComponent<PhysicsComponent>(baby);
            temp.velocity = lookDir * 20.0f;
        }
        if (Input::getKeyDown(GLFW_KEY_EQUAL)){
            int temp = 0;
        }
    }
}