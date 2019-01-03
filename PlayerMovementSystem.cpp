#include "PlayerMovementSystem.h"
#include "CustomComponents.hpp"
#include "core/glm/glm.hpp"
#include "core/glm/gtc/matrix_transform.hpp"
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
                glm::mat4 dir = glm::rotate(glm::mat4(1.0), (float)(glm::radians(tc.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
                dir = glm::rotate(dir, (float)(glm::radians(tc.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
                tc.position += glm::vec3(dir * glm::vec4(0, 0, -1, 0) * 3.0f * deltaTime);
            }
            if (Input::getKey(GLFW_KEY_S)){
                glm::mat4 dir = glm::rotate(glm::mat4(1.0), (float)(glm::radians(tc.worldRotation.y)), glm::vec3(0.0, 1.0, 0.0));
                dir = glm::rotate(dir, (float)(glm::radians(tc.worldRotation.z)), glm::vec3(0.0, 0.0, 1.0));
                tc.position -= glm::vec3(dir * glm::vec4(0, 0, -1, 0) * 3.0f * deltaTime);
            }
            if (Input::getKey(GLFW_KEY_A)){
                tc.position -= glm::vec3(3, 0, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_D)){
                tc.position += glm::vec3(3, 0, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_Q)){
                tc.position += glm::vec3(0, 3, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_E)){
                tc.position -= glm::vec3(0, 3, 0) * (float)deltaTime;
            }
            if (Input::getKey(GLFW_KEY_UP)){
                tc.rotation *= glm::normalize(glm::quat(glm::vec3(glm::radians(30.0f)*(float)deltaTime, 0, 0))); 
            }
            if (Input::getKey(GLFW_KEY_DOWN)){
                tc.rotation *= glm::normalize(glm::quat(glm::vec3(glm::radians(-30.0f)*(float)deltaTime, 0, 0))); 
            }
            if (Input::getKey(GLFW_KEY_LEFT)){
                tc.rotation *= glm::normalize(glm::quat(glm::vec3(0, glm::radians(-30.0f)*(float)deltaTime, 0))); 
            }
            if (Input::getKey(GLFW_KEY_RIGHT)){
                tc.rotation *= glm::normalize(glm::quat(glm::vec3(0, glm::radians(30.0f)*(float)deltaTime, 0))); 
            }
            if (Input::getKey(GLFW_KEY_1)){
                int child = *tc.childEntities.begin();
                PointLightComponent& pointLight = mWorld->getComponent<PointLightComponent>(child);
                pointLight.color = {1, 0, 0};
            }
            
            if (Input::getKey(GLFW_KEY_2)){
                int child = *tc.childEntities.begin();
                PointLightComponent& pointLight = mWorld->getComponent<PointLightComponent>(child);
                pointLight.color = {0, 1, 0};
            }
            
            if (Input::getKey(GLFW_KEY_3)){
                int child = *tc.childEntities.begin();
                PointLightComponent& pointLight = mWorld->getComponent<PointLightComponent>(child);
                pointLight.color = {0, 0, 1};
            }
            if (Input::getKey(GLFW_KEY_0)){
                int child = *tc.childEntities.begin();
                PointLightComponent& pointLight = mWorld->getComponent<PointLightComponent>(child);
                pointLight.color = {1, 1, 1};
            }
            if (Input::getKeyDown(GLFW_KEY_EQUAL)){
                int temp = 0;
            }
            // if (Input::getKeyDown(GLFW_KEY_E)){
            //     phys.useGravity = !phys.useGravity;
            //     phys.isKinematic = !phys.useGravity; 
            // }
        }
    }
}