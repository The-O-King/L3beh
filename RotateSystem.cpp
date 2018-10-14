#define GLFW_DLL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/glm/glm.hpp"
#include "core/glm/gtc/matrix_transform.hpp"

#include "RotateSystem.h"
#include "CustomComponents.hpp"
#include "core/world.h"

RotateSystem::RotateSystem(World* w){
    mWorld = w;
    neededComponents[type_id<TransformComponent>()] = 1;
}

void RotateSystem::init(){

}

void RotateSystem::update(float deltaTime){
    for (int e : entities){
        TransformComponent& tc = mWorld->getComponent<TransformComponent>(e);
        tc.rotation.z += 20 * deltaTime;
        tc.rotation.x += 5 * deltaTime;
    }
}