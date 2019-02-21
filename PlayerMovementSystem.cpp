#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/glm/gtc/matrix_transform.hpp"
#include "core/components.h"
#include "core/world.h"
#include "core/input.h"

std::string testEntity[] = { 
    "Entity PhysicsBall\n"
    "Physics 2 1 1 1 .5\n"
    "Transform 0    0 0 -10    0 0 0  1 1 1\n"
    "SphereCollider 0 1\n"
    "Render cSphere.obj cube.jpg  1 1 1 1 1 1 32\n"
    "Projectile 0\n"
    "/Entity\n",
    
    "Entity PhysicsBall\n"
    "Physics 2 1 1 1 .5\n"
    "Transform 0    0 0 -10    0 0 0  1 1 1\n"
    "BoxCollider 0  -.5 -.5 -.5 .5 .5 .5\n"
    "Render cube.obj cube.jpg  1 1 1 1 1 1 32\n"
    "Projectile 0\n"
    "/Entity\n"
};

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
        PlayerMovementComponent& pc = mWorld->getComponent<PlayerMovementComponent>(e);
        if (pc.active){
            TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
            glm::vec3 lookDir = glm::normalize(tc.worldRotation * glm::vec3(0, 0, -1));
            glm::vec3 forward = glm::normalize((tc.worldRotation * glm::vec3(0, 0, -1)) * glm::vec3(1, 0, 1));  
            glm::vec3 left = glm::normalize((tc.worldRotation * glm::vec3(-1, 0, 0)) * glm::vec3(1, 0, 1));  
            float speed = 0;
            if (Input::getKey(GLFW_KEY_LEFT_SHIFT)){
                speed = pc.runSpeed;
            }
            else{
                speed = pc.walkSpeed;
            }
            if (Input::getKey(GLFW_KEY_W)){      
                tc.position += forward * 5.0f * deltaTime;
            }
            if (Input::getKey(GLFW_KEY_S)){
                tc.position -= forward * 5.0f * deltaTime;
            }
            if (Input::getKey(GLFW_KEY_A)){
                tc.position += left * 5.0f * deltaTime;
            }
            if (Input::getKey(GLFW_KEY_D)){
                tc.position -= left * 5.0f * deltaTime;
            }

            double xdelta, ydelta;
            Input::getMouseDelta(&xdelta, &ydelta);
            glm::quat yawRotation = glm::vec3(0, xdelta * deltaTime, 0);
            glm::quat pitchRotation = glm::vec3(ydelta * deltaTime, 0, 0);
            tc.rotation = yawRotation * tc.rotation * pitchRotation;
            pc.currProjectile = glm::clamp(pc.currProjectile + (int)Input::getScrollDelta(), 0, 1);            

            if (Input::getMouseButtonDown(GLFW_MOUSE_BUTTON_1)){
                int baby = mWorld->createEntity(testEntity[pc.currProjectile]);
                TransformComponent& temp2 = mWorld->getComponent<TransformComponent>(baby);
                temp2.position = tc.worldPosition + (lookDir * 4.0f);
                temp2.rotation = tc.worldRotation;
                PhysicsComponent& temp = mWorld->getComponent<PhysicsComponent>(baby);
                temp.velocity = lookDir * 100.0f;
                temp.gravityScale = .1;
            }
            if (Input::getKeyDown(GLFW_KEY_EQUAL)){
                int temp = 0;
            }
        }
    }
}